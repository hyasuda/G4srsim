# Running file of Geant4 Simulation based on musrSim

## Run file list

#### 00000_test.mac
#### 0045_RAL.mac
#### 0100_SRsim.mac
#### 0110_SRsim.mac
#### 0200_SRsim.mac
#### 0210_SRsim.mac
     - I include the uniform map.
#### 0300_SRsim.mac
     - This file is updated in 14, June, 2018. I include the field map to this runnign code.
#### 0310_SRsim.mac
     - Applying field : Wien_180620_ver1_3D?Opera_2
#### 0320_SRsim.mac
     - Applying field :Wien_180621_ver4
#### 0330_SRsim.mac
     - Applying field : Wien_180621_ver5
     - Events : 30000 (Long Run)
#### 0340_SRsim.mac
     - Applying modified normalized field : Wien_180621_ver5_3D?Opera_2.map
     - Events : 30000 (Long Run)
#### 0350_SRsim.mac
     - Field : Same as 0340_SRsim.mac
     - Events : 30000 (Long Run)
     - Set save detector
#### 0360_SRsim.mac
     - Field : Same as 0340_SRsim.mac
     - Events : 30000 (Long Run) | test 100
     - Set save detector * 10
#### 0370_SRsim.mac

#### 0400_SRsim.mac
     - Using Pencil Beam (NOT using RFQ output)
     - for test

#### 0410_SRsim.mac
     - or tuning the field and beam position

#### 0420_SRsim.mac
     - using Real field : normalized factor is as same as calculation

#### 0430_SRsim.mac
     - optimize normalized factors by hand

#### 0450_SRsim.mac
     - uniform EB-field
     - RFQ output 
     - For study of beam profile and emittance
     - Event 16,116

#### 0500_SRsim.mac
     - For check the initial beam profile 
     - Modify RFQ output unit
     - Apply Real field
     - Event 16,116

#### 0501_SRsim.mac
     - For Rough check of trajectory
     - Setting is same as above
     - Event 100

#### 0510_SRsim.mac
     - For beam center tuning before Study of uniform field profile
     - Applying uniform field before tuning
     - Event 100

#### 0511_SRsim.mac
     - For Study beam profile in uniform field
     - Applying uniform field ( same as before tuning )
     - Event full 16,116

#### 0512_SRsim.mac
     - For Study beam profile in uniform field : Using pencil beam ( not include px , py)
     - Applying uniform field ( same as before tuning )
     - Event full 15,000

#### 0520_SRsim.mac
     - For beam center tuning before study of real field
     - Applying real filed
     - Event 1000
     - Using the Normalized factor as below : NB = 12.50, NE= 0.181911	

#### 0521_SRsim.mac
     - For study of beam parameter in real field
     - Applying real filed
     - Event full 16,116
     - Using the Normalized factor as below : NB = 12.50, NE= 0.181911	

#### 0530_SRsim.mac
     - For study of beam quality
     - Applying uniform field
     - Using rfqll_xp0.dat : xp = 0
     - Event full 16,116

#### 0531_SRsim.mac
     - For study of beam quality
     - Applying uniform field
     - Using rfqll_yp0.dat : yp = 0
     - Event full 16,116

#### 0532_SRsim.mac
     - For study of beam quality
     - Applying uniform field
     - Using rfqll_p0.dat : p0 = 8.466 * 1.e-03 GeV
     - Event full 16,116

#### 0533_SRsim.mac
     - For study of beam quality
     - Applying uniform field
     - Using rfqll_ttf_test.dat : 3 points 
     - Event 3

#### 0540_SRsim.mac
     - For test
     - Using transferd intial beam "rfqll_trans_output.dat"
     - Applying uniform field
     - Event full 1000
	  

## map field
   - Using RFQ output given from Otani-san

#### Wien_180621_ver5_3D?Opera_4.map
     - mean x = 1.062 , mean y = -3.602
     - mean PolZ = 0.9994

#### Wien_180621_ver5_3D?Opera_5.map
     - NB = 12.50, NE= 0.181051
     - mean x = 0.8208, mean y = -3.587
     - mean PolZ = 0.9995

#### Wien_180621_ver5_3D?Opera_5.map
     - NB = 12.50, NE= 0.181551
     - mean x = 0.3601, mean y = -3.529
     - mean PolZ = 0.9996

#### Wien_180621_ver5_3D?Opera_5.map
     - NB = 12.50, NE= 0.181851
     - mean x = 0.06591, mean y = -3.562
     - mean PolZ = 0.9997

#### Wien_180621_ver5_3D?Opera_5.map
     - NB = 12.50, NE= 0.181901
     - mean x = 0.01113, mean y = -3.55
     - mean PolZ = 0.9997

#### Wien_180621_ver5_3D?Opera_5.map
     - NB = 12.50, NE= 0.181921
     - mean x = -0.0109, mean y = -3.546
     - mean PolZ = 0.9997

#### Wien_180621_ver5_3D?Opera_5.map 
     - FINALIZED FIELD
     - NB = 12.50, NE= 0.181911
     - mean x = 0.0001232, mean y = -3.548
     - mean PolZ = 0.9997