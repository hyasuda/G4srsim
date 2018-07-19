/***************************************************************************
 *  musrSim - the program for the simulation of (mainly) muSR instruments. *
 *          More info on http://lmu.web.psi.ch/simulation/index.html .     *
 *          musrSim is based od Geant4 (http://geant4.web.cern.ch/geant4/) *
 *                                                                         *
 *  Copyright (C) 2009 by Paul Scherrer Institut, 5232 Villigen PSI,       *
 *                                                       Switzerland       *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              *
 ***************************************************************************/

#include "musrTabulatedElementField.hh"
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
#include "G4UnitsTable.hh"

musrTabulatedElementField::musrTabulatedElementField( const char* filename, const char* fldTableType, G4double fieldValue, G4LogicalVolume* logVolume, G4ThreeVector positionOfTheCenter) : F04ElementField(positionOfTheCenter, logVolume),
  ffieldValue(fieldValue)
{    
  // The following posibilities of the format of the field map are distinguieshed:
  //  3DBOpera = 3D ... 3D field , magnetic, Opera format (Kamil-like)
  //  3DE           ... 3D field , electric, Toni-like                                      (3DE WAS TESTED)
  //  3DB           ... 3D field , magnetic, Toni-like 
  //  2DBOpera = 2D .... 2D field, magnetic, Opera format (Kamil-like), X and Z components  (2D WAS TESTED)
  //  2DBOperaXY = 2D_OperaXY .... 2D field, magnetic, Opera format with (Kamil-like), X and Y components
  //  2DE           ... 2D field , electric, Toni-like                                      (2DE WAS TESTED)
  //  2DB           ... 2D field , magnetic, Toni-like
  
  G4double lenUnit = 1*m;                // length unit of the field map grid coordinates
  G4double fieldNormalisation = 1.;      // Normalisation factor by which the the field map has to be multiplied
                                         // in order to get 1T (in the case of magnetic field)
                                         // "lenUnit" and "fieldNormalisation" are needed only if not specified
                                         // inside the fieldmap file;
  strcpy(fieldTableType,fldTableType);
  fldType = 'B';
  fUnit = "T";     
  fieUnit = tesla;
  if (fieldTableType[2]=='E') {
    fldType = 'E';
    fUnit = "kV/mm"; 
    fieUnit= kilovolt/mm;
  }
  
  fldDim = (fieldTableType[0]=='3') ? 3:2;  
  if (fldDim==2) ny=1;

  G4cout << "\n-----------------------------------------------------------" << G4endl;
  G4cout << "      Field (of type "<<fieldTableType<<") set to "<< fieldValue/fieUnit << " "<< fUnit << G4endl;  
  G4cout << "\n ---> " "Reading the field grid from " << filename << " ... " << G4endl; 
  std::ifstream file( filename ); // Open the file for reading.
  if (!(file.is_open())) {
    G4cout << "Fieldmap file \""<< filename << "\" not opened (found) !!!"<<G4endl; 
    musrErrorMessage::GetInstance()->musrError(FATAL,"musrTabulatedElementField: Field map file not found !",false);
  }

  char buffer[256];
  G4bool boolMinimaAndMaximaDefinedInTheFile = false;
  if (fldType=='E') G4cout<<"      Electric field ";
  if (fldType=='B') G4cout<<"      Magnetic field ";
  if ((strcmp(fieldTableType,"3DE")==0)||(strcmp(fieldTableType,"3DB")==0)) {
    // File is in Toni Shiroka format:
    // Read the number of arguments and decide filetype - 3 or 6 columns
    G4cout << "3D, field-map file format by Toni Shiroka" << G4endl;
    char   lenUnitFromFile[50];
    double fieldNormalisationFromFile;
    file.getline(buffer,256);
    int n_arg = sscanf (buffer,"%d %d %d %s %lf %lf %lf %lf %lf %lf %lf",
			&nx, &ny, &nz, lenUnitFromFile, &fieldNormalisationFromFile,
			&minimumx, &maximumx, &minimumy, &maximumy, &minimumz, &maximumz);
    lenUnit = G4UnitDefinition::GetValueOf(lenUnitFromFile);
    fieldNormalisation = fieldNormalisationFromFile;
    if (n_arg==11) {
      // The length unit and norm. factor have to be manually added to the field-map file!
      G4cout << " ---> Assumed order (3 col.):   "<<fldType<<"x, "<<fldType<<"y, "<<fldType<<"z"<<G4endl;
      boolMinimaAndMaximaDefinedInTheFile = true;
      minimumx = minimumx * lenUnit;
      minimumy = minimumy * lenUnit;
      minimumz = minimumz * lenUnit;
      maximumx = maximumx * lenUnit;
      maximumy = maximumy * lenUnit;
      maximumz = maximumz * lenUnit;
    }
    else {
      G4cout << " ---> Assumed order (6 col.):  x, y, z, "<<fldType<<"x, "<<fldType<<"y, "<<fldType<<"z"<<G4endl;
    }

    // Ignore header information. All lines whose first character
    // is '%' are considered to be part of the header.
    do {
      file.ignore(256, '\n');
    } while (file.peek() == '%');
  }
  else if ((strcmp(fieldTableType,"3D")==0)||(strcmp(fieldTableType,"3DBOpera")==0)) {
    // OPERA format of the input file:
    // Read table dimensions 
    lenUnit = 1*m;
    fieldNormalisation = 1.;
    G4cout << "3D, field-map file format from OPERA (Kamil)" << G4endl;
    G4cout << " ---> Assumed order (7 col.): x, y, z, "<<fldType<<"x, "<<fldType<<"y, "<<fldType<<"z, Dummy"<<G4endl;

    file.getline(buffer,256); // Skip the first empty line of the file
    file >> nx >> ny >> nz; // Note dodgy order
    
    // Ignore other header information    
    // The first line whose second character is '0' is considered to
    // be the last line of the header.
    do {
      file.getline(buffer,256);
    } while ( buffer[1]!='0');
  }
  else if ((strcmp(fieldTableType,"2DE")==0)||(strcmp(fieldTableType,"2DB")==0)||(strcmp(fieldTableType,"2DEf")==0)) {
    // File is in Toni Shiroka format:
    G4cout << "2D, field-map file format by Toni Shiroka" << G4endl;
    G4cout << " ---> Assumed order (4 col.): r, z, "<<fldType<<"r, "<<fldType<<"z"<<G4endl;
    char   lenUnitFromFile[50];
    file >> nx >> nz >> lenUnitFromFile >> fieldNormalisation;
    lenUnit = G4UnitDefinition::GetValueOf(lenUnitFromFile);
    // Ignore header information. All lines whose first character
    // is '%' are considered to be part of the header.
    do {
      file.ignore(256, '\n');
    } while (file.peek() == '%');
  }
  else if ((strcmp(fieldTableType,"2D_OperaXY")==0)||(strcmp(fieldTableType,"2DBOperaXY")==0)) {
    int nDummy;
    lenUnit = 1*cm;
    fieldNormalisation = 0.00001;
    G4cout << "2D, field-map file format from OPERA with X,Y components (Kamil)" << G4endl;
    G4cout << " ---> Assumed order (6 col.): r, z, dummy, "<<fldType<<"r, "<<fldType<<"z, Dummy"<<G4endl;
    file >> nx >> nz >> nDummy;
    do {
      file.getline(buffer,256);
    } while ( buffer[1]!='0');
  }
  else if ((strcmp(fieldTableType,"2D")==0)||(strcmp(fieldTableType,"2DBOpera")==0)) {
    int nDummy;
    lenUnit = 1*cm;
    fieldNormalisation = 0.00001;
    G4cout << "2D, field-map file format from OPERA with X,Z components (Kamil)" << G4endl;
    G4cout << " ---> Assumed order (6 col.): r, dummy, z, "<<fldType<<"r, "<<fldType<<"z, Dummy"<<G4endl;
    file >> nx >> nDummy >> nz;
    //    G4cout << nx <<" "<< nDummy <<"  "<< nz<<G4endl;
    do {
      file.getline(buffer,256);
    } while ( buffer[1]!='0');
  }
  else {
    G4cout << "      musrTabulatedElementField::musrTabulatedElementField: Unknown field required!"
	   << "  ("<<fieldTableType<<")" << G4endl;
    G4cout << "                 =====>   S T O P " << G4endl;
    musrErrorMessage::GetInstance()->musrError(FATAL,"musrTabulatedElementField: Unknown field required!",false);
  }



  // SET UP STORAGE SPACE FOR THE TABLE
  int ix, iy, iz;
  if (fldDim==3) {
    G4cout << "      The grid consists of [" << nx << " x " << ny << " x " << nz << "] x, y, z values" << G4endl;
    G4cout << "      Field map length unit          = " << lenUnit/mm<<" mm"<< G4endl;
    G4cout << "      Field map normalisation factor = " << fieldNormalisation << G4endl;
    // Set up storage space for the table
    xField.resize( nx );
    yField.resize( nx );
    zField.resize( nx );    
    for (ix=0; ix<nx; ix++) {
      xField[ix].resize(ny);
      yField[ix].resize(ny);
      zField[ix].resize(ny);
      for (iy=0; iy<ny; iy++) {
	xField[ix][iy].resize(nz);
	yField[ix][iy].resize(nz);
	zField[ix][iy].resize(nz);
      }
    }
  }
  else if (fldDim==2) {
    G4cout << "      The grid consists of [" << nx << " x " << nz << "] R, z values" << G4endl;
    G4cout << "      Field map normalisation factor = " << fieldNormalisation << G4endl;
    G4cout << "      Field map length unit          = " << lenUnit << G4endl;
    if ((nx<2)||(nz<2)) {
      char eMessage[200];
      sprintf(eMessage,"musrTabulatedElementField(): Strange Field table!  nx=%i, nz=%i",nx,nz);
      musrErrorMessage::GetInstance()->musrError(WARNING,eMessage,false);
    }
    // Set up storage space for the table
    xField2D.resize( nx );
    zField2D.resize( nx );
    for (ix=0; ix<nx; ix++) {
      xField2D[ix].resize(nz);
      zField2D[ix].resize(nz);
    }
  }


  

  
  // Read in the data
  double xval,yval,zval,bx,by,bz;
  double permeability; // Not used in this example.
  for (ix=0; ix<nx; ix++) {
    for (iy=0; iy<ny; iy++) {
      for (iz=0; iz<nz; iz++) {
	if ((strcmp(fieldTableType,"3DE")==0)||(strcmp(fieldTableType,"3DB")==0)) {
	  if (boolMinimaAndMaximaDefinedInTheFile) {
	    file >> bx >> by >> bz; // Read ONLY field values
	  }
	  else {
	    file >> xval >> yval >> zval >> bx >> by >> bz;
	  }
	}
	else if ((strcmp(fieldTableType,"3D")==0)||(strcmp(fieldTableType,"3DBOpera")==0)) { 
	  file >> xval >> yval >> zval >> bx >> by >> bz >> permeability;
	}
	else if ((strcmp(fieldTableType,"2DE")==0)||(strcmp(fieldTableType,"2DB")==0)||(strcmp(fieldTableType,"2DEf")==0)) { 
	  file >> xval >> zval >> bx >> bz;
	}
	else if ((strcmp(fieldTableType,"2D")==0)||(strcmp(fieldTableType,"2DBOpera")==0)) { 
	  file >> xval >> yval >> zval >> bx >> bz >> permeability;
	  //	  G4cout<< xval <<" "<< yval <<" "<< zval <<" "<< bx <<" "<< bz <<G4endl;
	}
	else if ((strcmp(fieldTableType,"2D_OperaXY")==0)||(strcmp(fieldTableType,"2DBOperaXY")==0)) {
	  file >> xval >> zval >> yval >> bx >> bz >> permeability;
	}
	else {
	  G4cout << "      musrTabulatedElementField::musrTabulatedElementField: Undefined field required!"
		 << "  ("<<fieldTableType<<")" << G4endl;
	  G4cout << "                 =====>   S T O P " << G4endl;
	  musrErrorMessage::GetInstance()->musrError(FATAL,"musrTabulatedElementField: Undefined field required!",false);
	}


	if (fldDim==3) {  // 3D field
	  if ((!boolMinimaAndMaximaDefinedInTheFile) && ( ix==0 && iy==0 && iz==0 ) ) {
	    minimumx = xval * lenUnit;
	    minimumy = yval * lenUnit;
	    minimumz = zval * lenUnit;
	  }
	  xField[ix][iy][iz] = bx*fieldNormalisation;
	  yField[ix][iy][iz] = by*fieldNormalisation;
	  zField[ix][iy][iz] = bz*fieldNormalisation;
	}
	else {  // 2D field
	  if ((!boolMinimaAndMaximaDefinedInTheFile) && ( ix==0 && iz==0 ) ) {
	    minimumx = xval * lenUnit;
	    minimumz = zval * lenUnit;
	  }
	  xField2D[ix][iz] = bx*fieldNormalisation;
	  zField2D[ix][iz] = bz*fieldNormalisation;
	}
      }
    }
  }
  file.close();


  if (!boolMinimaAndMaximaDefinedInTheFile) {
    maximumx = xval * lenUnit;
    maximumz = zval * lenUnit;
    if (fldDim==3) maximumy = yval * lenUnit;
  }

  G4cout << " ---> ... reading of the field map finished." << G4endl;  

  if (fldDim==3) G4cout<<" ---> Min values of x,y,z: ";
  else           G4cout<<" ---> Min values of R,z: ";
  G4cout << minimumx/cm << ", ";
  if (fldDim==3) G4cout<< minimumy/cm << ", " ;
  G4cout << minimumz/cm << " cm "<<G4endl;
  if (fldDim==3) G4cout<<" ---> Max values of x,y,z: ";
  else           G4cout<<" ---> Max values of R,z: ";
  G4cout << maximumx/cm << ", ";
  if (fldDim==3) G4cout<< maximumy/cm << ", " ;
  G4cout << maximumz/cm << " cm " << G4endl;


  // Should really check that the limits are not the wrong way around.
  G4bool reorderingDone = false;
  if (maximumx < minimumx) {Invert("x"); reorderingDone=true;} 
  if (fldDim==3) {if (maximumy < minimumy) {Invert("y"); reorderingDone=true;} }
  if (maximumz < minimumz) {Invert("z"); reorderingDone=true;} 
  
  if (reorderingDone) {
    G4cout << "\n Reordering of the field grid was neccessary - after reordering:"<<G4endl;  
    if (fldDim==3) G4cout<<" ---> Min values of x,y,z: ";
    else           G4cout<<" ---> Min values of R,z: ";
    G4cout << minimumx/cm << ", ";
    if (fldDim==3) G4cout<< minimumy/cm << ", " ;
    G4cout << minimumz/cm << " cm "<<G4endl;
    if (fldDim==3) G4cout<<" ---> Max values of x,y,z: ";
    else           G4cout<<" ---> Max values of R,z: ";
    G4cout << maximumx/cm << ", ";
    if (fldDim==3) G4cout<< maximumy/cm << ", " ;
    G4cout << maximumz/cm << " cm " << G4endl;
  }

  dx = maximumx - minimumx;
  if (fldDim==3)   dy = maximumy - minimumy;
  dz = maximumz - minimumz;
  if (fldDim==3)  G4cout << "\n ---> Dif values x,y,z (range): ";
  else            G4cout << "\n ---> Dif values R,z (range): ";
  G4cout << dx/cm << ", ";
  if (fldDim==3)  G4cout << dy/cm << ", ";
  G4cout << dz/cm << " cm."<<G4endl;
  G4cout << "-----------------------------------------------------------" << G4endl;
  

  // Set maximum width, height and lenght of the field - very important!  This
  // dimensions are used to decide whether the "addFieldValue" method will be
  // called at all for a given elementField.
  if (fldDim==2) {
    maximumWidth = 2*dx;
    maximumHeight = 2*dx;
    if ( (strcmp(fieldTableType,"2D")==0)||(strcmp(fieldTableType,"2DBOpera")==0)||(strcmp(fieldTableType,"2D_OperaXY")) ) {
      maximumLength = 2*dz;
    }
    else maximumLength = dz;
  }
  else {
    maximumWidth = dx;
    maximumHeight = dy;
    maximumLength = dz;
  }
}

void musrTabulatedElementField::addFieldValue(const G4double point[4],
				      G4double *field ) const
{
  //  G4cout<<"musrTabulatedElementField::addFieldValue"<<G4endl;
  if (fldDim==2) addFieldValue2D(point,field);
  else           addFieldValue3D(point,field);
}

void musrTabulatedElementField::addFieldValue3D(const G4double point[4],
				      G4double *field ) const
{
  G4double B[3];  // Field value obtained from the field table

  G4ThreeVector global(point[0],point[1],point[2]);
  G4ThreeVector local;

  local = global2local.TransformPoint(global);
  
  double x = local.x();
  double y = local.y();
  double z = local.z();

  //  G4cout<<"Global points= "<<point[0]<<", "<<point[1]<<", "<<point[2]<<",  Local point= "<<x<<", "<<y<<", "<<z<<G4endl;


  // Check that the point is within the defined region 
  if ( x>minimumx && x<maximumx &&
       y>minimumy && y<maximumy && 
       z>minimumz && z<maximumz ) {
    
    // Position of given point within region, normalized to the range
    // [0,1]
    double xfraction = (x - minimumx) / dx;
    double yfraction = (y - minimumy) / dy; 
    double zfraction = (z - minimumz) / dz;

    // Need addresses of these to pass to modf below.
    // modf uses its second argument as an OUTPUT argument.
    double xdindex, ydindex, zdindex;
    
    // Position of the point within the cuboid defined by the
    // nearest surrounding tabulated points
    double xlocal = ( modf(xfraction*(nx-1), &xdindex));
    double ylocal = ( modf(yfraction*(ny-1), &ydindex));
    double zlocal = ( modf(zfraction*(nz-1), &zdindex));
    
    // The indices of the nearest tabulated point whose coordinates
    // are all less than those of the given point
    int xindex = static_cast<int>(xdindex);
    int yindex = static_cast<int>(ydindex);
    int zindex = static_cast<int>(zdindex);
    
    //cks  The following check is necessary - even though xindex and zindex should never be out of range,
    //     it may happen (due to some rounding error ?).  It is better to leave the check here.
    if ((xindex<0)||(xindex>(nx-2))) {
      std::cout<<"SERIOUS PROBLEM:  xindex out of range!  xindex="<<xindex<<"   x="<<x<<"  xfraction="<<xfraction<<std::endl;
      if (xindex<0) xindex=0;
      else xindex=nx-2;
    }
    if ((yindex<0)||(yindex>(ny-2))) {
      std::cout<<"SERIOUS PROBLEM:  yindex out of range!  yindex="<<yindex<<"   y="<<y<<"  yfraction="<<yfraction<<std::endl;
      if (yindex<0) yindex=0;
      else yindex=ny-2;
    }
    if ((zindex<0)||(zindex>(nz-2))) {
      std::cout<<"SERIOUS PROBLEM:  zindex out of range!  zindex="<<zindex<<"   z="<<z<<"  zfraction="<<zfraction<<std::endl;
      if (zindex<0) zindex=0;
      else zindex=nz-2;
    }

        // Full 3-dimensional version
    B[0] =
      xField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      xField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      xField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      xField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      xField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      xField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      xField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      xField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
    B[1] =
      yField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      yField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      yField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      yField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      yField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      yField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      yField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      yField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;
    B[2] =
      zField[xindex  ][yindex  ][zindex  ] * (1-xlocal) * (1-ylocal) * (1-zlocal) +
      zField[xindex  ][yindex  ][zindex+1] * (1-xlocal) * (1-ylocal) *    zlocal  +
      zField[xindex  ][yindex+1][zindex  ] * (1-xlocal) *    ylocal  * (1-zlocal) +
      zField[xindex  ][yindex+1][zindex+1] * (1-xlocal) *    ylocal  *    zlocal  +
      zField[xindex+1][yindex  ][zindex  ] *    xlocal  * (1-ylocal) * (1-zlocal) +
      zField[xindex+1][yindex  ][zindex+1] *    xlocal  * (1-ylocal) *    zlocal  +
      zField[xindex+1][yindex+1][zindex  ] *    xlocal  *    ylocal  * (1-zlocal) +
      zField[xindex+1][yindex+1][zindex+1] *    xlocal  *    ylocal  *    zlocal ;

    B[0] *= ffieldValue;
    B[1] *= ffieldValue;
    B[2] *= ffieldValue;

    G4ThreeVector finalField(B[0],B[1],B[2]);
    finalField = global2local.Inverse().TransformAxis(finalField);

    if (fldType == 'E') {
      field[3] += finalField.x();
      field[4] += finalField.y();
      field[5] += finalField.z();
    }
    else {
      field[0] += finalField.x();
      field[1] += finalField.y();
      field[2] += finalField.z();
    }
  }
  //  G4cout<<"Kamil: Field: ("<<field[0]/tesla<<","<<field[1]/tesla<<","<<field[2]/tesla<<")"<<G4endl;

}

void musrTabulatedElementField::addFieldValue2D(const G4double point[4],
				      G4double *field ) const
{
  G4double B[3];  // Field value obtained from the field table

  G4ThreeVector global(point[0],point[1],point[2]);
  G4ThreeVector local;

  local = global2local.TransformPoint(global);
  
  double x, z, z_sign;
  if ((strcmp(fieldTableType,"2D")==0)||(strcmp(fieldTableType,"2DBOpera")==0)||
      (strcmp(fieldTableType,"2D_OperaXY"))||(strcmp(fieldTableType,"2DEf")==0)) {
    // Field is defined in just positive range of z;  i.e. it is expected to be "symmetric"
    // and the field for negative z is calculated from the positive z half.
    x = sqrt(local.x()*local.x()+local.y()*local.y());
    z = fabs(local.z());
    z_sign = (local.z()>0) ? 1.:-1.;
  }
  else {
    // Field is defined along the whole range of the z axis (i.e. asymmetric field is expected)
    x = sqrt(local.x()*local.x()+local.y()*local.y());
    z = local.z();
    z_sign = 1;    
  }
  // Check that the point is within the defined region 
  if ( x<maximumx && z<maximumz ) {
    // if (evNr>evNrKriz) std::cout<<"bol som tu"<<std::endl;
    
    // Position of given point within region, normalized to the range
    // [0,1]
    double xfraction = (x - minimumx) / dx;
    double zfraction = (z - minimumz) / dz;
    
    // Need addresses of these to pass to modf below.
    // modf uses its second argument as an OUTPUT argument.
    double xdindex, zdindex;
    
    // Position of the point within the cuboid defined by the
    // nearest surrounding tabulated points
    double xlocal = ( modf(xfraction*(nx-1), &xdindex));
    double zlocal = ( modf(zfraction*(nz-1), &zdindex));
    
    // The indices of the nearest tabulated point whose coordinates
    // are all less than those of the given point
    int xindex = static_cast<int>(xdindex);
    int zindex = static_cast<int>(zdindex);
    
    //cks  The following check is necessary - even though xindex and zindex should never be out of range,
    //     it may happen (due to some rounding error ?).  It is better to leave the check here.
    if ((xindex<0)||(xindex>(nx-2))) {
      std::cout<<"SERIOUS PROBLEM:  xindex out of range!  xindex="<<xindex<<"   x="<<x<<"  xfraction="<<xfraction<<std::endl;
      if (xindex<0) xindex=0;
      else xindex=nx-2;
    }
    if ((zindex<0)||(zindex>(nz-2))) {
      std::cout<<"SERIOUS PROBLEM:  zindex out of range!  zindex="<<zindex<<"   z="<<z<<"  zfraction="<<zfraction<<std::endl;
      if (zindex<0) zindex=0;
      else zindex=nz-2;
    }

    //    G4cout<<"xField2D["<<xindex<<"]["<<zindex<<"]="<<xField2D[xindex  ][zindex  ]<<G4endl;
    //    G4cout<<"zField2D["<<xindex<<"]["<<zindex<<"]="<<zField2D[xindex  ][zindex  ]<<G4endl;
   
    // Interpolate between the neighbouring points
    double Bfield_R =
      xField2D[xindex  ][zindex  ] * (1-xlocal) * (1-zlocal) +
      xField2D[xindex  ][zindex+1] * (1-xlocal) *    zlocal  +
      xField2D[xindex+1][zindex  ] *    xlocal  * (1-zlocal) +
      xField2D[xindex+1][zindex+1] *    xlocal  *    zlocal  ;
    B[0] = (x>0) ? Bfield_R * (local.x() /x) : 0.;
    B[1] = (x>0) ? Bfield_R * (local.y() /x) : 0.;
    B[2] =
      zField2D[xindex  ][zindex  ] * (1-xlocal) * (1-zlocal) +
      zField2D[xindex  ][zindex+1] * (1-xlocal) *    zlocal  +
      zField2D[xindex+1][zindex  ] *    xlocal  * (1-zlocal) +
      zField2D[xindex+1][zindex+1] *    xlocal  *    zlocal  ;

    if (fldType == 'E') {  // Electric field
      B[0] *= ffieldValue;
      B[1] *= ffieldValue;
      B[2] *= ffieldValue * z_sign;
    }
    else {                 // Magnetic field 
      B[0] *= ffieldValue * z_sign;
      B[1] *= ffieldValue * z_sign;
      B[2] *= ffieldValue;
    }

    G4ThreeVector finalField(B[0],B[1],B[2]);
    finalField = global2local.Inverse().TransformAxis(finalField);

    if (fldType == 'E') {
      field[3] += finalField.x();
      field[4] += finalField.y();
      field[5] += finalField.z();
    }
    else {
      field[0] += finalField.x();
      field[1] += finalField.y();
      field[2] += finalField.z();
    }
    //    G4cout<<"F= "<<field[0]<<" "<<field[1]<<" "<<field[2]<<" "<<field[3]<<" "<<field[4]<<" "<<field[5]<<G4endl;
  }
}



G4double musrTabulatedElementField::GetNominalFieldValue() {
  return ffieldValue;
}

void musrTabulatedElementField::SetNominalFieldValue(G4double newFieldValue) {
  //  // Rescale the magnetic field for a new value of the magnetic field
  ffieldValue=newFieldValue;
  G4cout<<"musrTabulatedElementField.cc:   ffieldValue changed to="<< ffieldValue/fieUnit<<" "<<fUnit<<G4endl;
}

void musrTabulatedElementField::Invert(const char* indexToInvert) {
  // This function inverts the indexes of the field table for a given axis (x or z).
  // It should be called in the case when the x or z coordinate in the initial 
  // field table is ordered in the decreasing order. 
  std::vector< std::vector< std::vector< double > > > xFieldTemp(xField);
  std::vector< std::vector< std::vector< double > > > yFieldTemp(yField);
  std::vector< std::vector< std::vector< double > > > zFieldTemp(zField);
  G4bool invertX=false;
  G4bool invertY=false;
  G4bool invertZ=false;

  G4cout<<"Check that the musrTabulatedElementField::Invert() function works properly!"<<G4endl;
  G4cout<<"It has not been tested yet!"<<G4endl;

  if (strcmp(indexToInvert,"x")==0) {invertX=true; std::swap(maximumx,minimumx);}
  if (strcmp(indexToInvert,"y")==0) {invertY=true; std::swap(maximumx,minimumx);}
  if (strcmp(indexToInvert,"z")==0) {invertZ=true; std::swap(maximumz,minimumz);}

  for (int ix=0; ix<nx; ix++) {
    for (int iy=0; iy<ny; iy++) {
      for (int iz=0; iz<nz; iz++) {
	if (invertX) {
	  xField[ix][iy][iz] = xFieldTemp[nx-1-ix][iy][iz];
	  yField[ix][iy][iz] = yFieldTemp[nx-1-ix][iy][iz];
	  zField[ix][iy][iz] = zFieldTemp[nx-1-ix][iy][iz];
	}
	else if(invertY) {
	  xField[ix][iy][iz] = xFieldTemp[ix][ny-1-iy][iz];
	  yField[ix][iy][iz] = yFieldTemp[ix][ny-1-iy][iz];
	  zField[ix][iy][iz] = zFieldTemp[ix][ny-1-iy][iz];
	}
	else if(invertZ) {
	  xField[ix][iy][iz] = xFieldTemp[ix][iy][nz-1-iz];
	  yField[ix][iy][iz] = yFieldTemp[ix][iy][nz-1-iz];
	  zField[ix][iy][iz] = zFieldTemp[ix][iy][nz-1-iz];
	}
      }
    }
  }

  
}
