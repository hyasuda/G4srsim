\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesPackage{dis04}[2004/01/07 DIS'04 document class]
\newif\if@restonecol
\newif\if@titlepage
%
\DeclareOption{draft}{\setlength\overfullrule{5pt}}
\DeclareOption{final}{\setlength\overfullrule{0pt}}
\DeclareOption{twoside}{\@twosidetrue  \@mparswitchtrue}
%
\ExecuteOptions{a4paper,10pt,twoside,final}
\ProcessOptions
%
%\def\publ{\hss \copyright ~Institute of Experimental Physics SAS, Ko\v sice, Slovakia\hss}
\def\publ{\hspace{10cm} }
%
\renewcommand\normalsize{%
   \@setfontsize\normalsize\@xpt\@xiipt
   \abovedisplayskip6\p@\@plus4\p@\@minus2\p@
   \abovedisplayshortskip\abovedisplayskip
   \belowdisplayshortskip\abovedisplayshortskip
   \belowdisplayskip\abovedisplayskip
   \let\@listi\@listI}
\normalsize
\newcommand\small{%
   \@setfontsize\small\@ixpt{11}%
   \abovedisplayskip 8.5\p@ \@plus3\p@ \@minus4\p@
   \abovedisplayshortskip \z@ \@plus2\p@
   \belowdisplayshortskip 4\p@ \@plus2\p@ \@minus2\p@
   \def\@listi{\leftmargin\leftmargini
               \topsep 4\p@ \@plus2\p@ \@minus2\p@
               \parsep 2\p@ \@plus\p@ \@minus\p@
               \itemsep \parsep}%
   \belowdisplayskip \abovedisplayskip
}
\newcommand\footnotesize{%
   \@setfontsize\footnotesize\@viiipt{9.5}%
   \abovedisplayskip 6\p@ \@plus2\p@ \@minus4\p@
   \abovedisplayshortskip \z@ \@plus\p@
   \belowdisplayshortskip 3\p@ \@plus\p@ \@minus2\p@
   \def\@listi{\leftmargin\leftmargini
               \topsep 3\p@ \@plus\p@ \@minus\p@
               \parsep 2\p@ \@plus\p@ \@minus\p@
               \itemsep \parsep}%
   \belowdisplayskip \abovedisplayskip
}
\newcommand\scriptsize{\@setfontsize\scriptsize\@viipt\@viiipt}
\newcommand\tiny{\@setfontsize\tiny\@vpt\@vipt}
\newcommand\large{\@setfontsize\large\@xiipt{14}}
\newcommand\Large{\@setfontsize\Large\@xivpt{18}}
\newcommand\LARGE{\@setfontsize\LARGE\@xviipt{22}}
\newcommand\huge{\@setfontsize\huge\@xxpt{25}}
\newcommand\Huge{\@setfontsize\Huge\@xxvpt{30}}
\setlength\parindent{0.25in}
\setlength\smallskipamount{3\p@ \@plus 1\p@ \@minus 1\p@}
\setlength\medskipamount{6\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\bigskipamount{12\p@ \@plus 4\p@ \@minus 4\p@}
%%%%%%
%
\newdimen\trimheight
\newdimen\trimwidth
\newdimen\typeheight
\newdimen\typewidth
\newdimen\tempdimen
\newdimen\normaltextheight
%

\trimheight9.75in
\addtolength\trimheight{.047in}
\trimwidth6.50in
%cks \typeheight48.10pc
\typeheight60.pc
%
\setlength\parindent{1.5em}%10\p@
\setlength\headheight{6\p@}
\setlength\headsep   {15pt}%11
\setlength\topskip   {7\p@}%{10\p@}
\setlength\footskip{18pt}
\setlength\maxdepth{4\p@}
\setlength\@maxdepth\maxdepth
%cks \setlength\textwidth{30pc}
\setlength\textwidth{38pc}
\setlength\textheight\typeheight
\advance\textheight-\headheight
\advance\textheight-\headsep
%\advance\textheight\topskip%Comment
\advance\textheight-3pt  %comment
\setlength\normaltextheight{\textheight}
%cks \setlength\oddsidemargin{60pt}%.625in%
%cks \setlength\evensidemargin{\trimwidth}
\setlength\oddsidemargin{0pt}
\setlength\evensidemargin{455pt}
\addtolength\evensidemargin{-\textwidth}
\addtolength\evensidemargin{-\oddsidemargin}
\setlength\marginparwidth{.75in}
\setlength\marginparsep{7\p@}
\setlength\marginparpush{5\p@}
%cks \setlength\topmargin{.65926in}%
\setlength\topmargin{0.in}%
\setlength\footnotesep{6\p@}%6pt
\setlength{\skip\footins}{11\p@ \@plus 5\p@ \@minus 2\p@}
\setlength\floatsep    {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\textfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
\setlength\intextsep   {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\dblfloatsep    {18\p@ \@plus 2\p@ \@minus 2\p@}
\setlength\dbltextfloatsep{20\p@ \@plus 2\p@ \@minus 4\p@}
\setlength\@fptop{0\p@}
\setlength\@fpsep{8\p@ \@plus 2fil}
\setlength\@fpbot{0\p@ \@plus 1fil}
\setlength\@dblfptop{0\p@ \@plus 1fil}
\setlength\@dblfpsep{8\p@ \@plus 2fil}
\setlength\@dblfpbot{0\p@ \@plus 1fil}
\setlength\partopsep{0\p@}
%
\setlength\lineskip{1\p@}
\setlength\normallineskip{1\p@}%
\renewcommand\baselinestretch{}%{}
\setlength\parskip{0\p@}
\@lowpenalty   51
\@medpenalty  151
\@highpenalty 301
\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
%
\def\@listi{\leftmargin\leftmargini
            \parsep 4\p@ \@plus2\p@ \@minus\p@
            \topsep 8\p@ \@plus2\p@ \@minus4\p@
            \itemsep4\p@ \@plus2\p@ \@minus\p@}
\let\@listI\@listi
\@listi
\def\@listii {\leftmargin\leftmarginii
              \labelwidth\leftmarginii
              \advance\labelwidth-\labelsep
              \topsep    4\p@ \@plus2\p@ \@minus\p@
              \parsep    2\p@ \@plus\p@  \@minus\p@
              \itemsep   \parsep}
\def\@listiii{\leftmargin\leftmarginiii
              \labelwidth\leftmarginiii
              \advance\labelwidth-\labelsep
              \topsep    2\p@ \@plus\p@\@minus\p@
              \parsep    \z@
              \partopsep \p@ \@plus\z@ \@minus\p@
              \itemsep   \topsep}
\def\@listiv {\leftmargin\leftmarginiv
              \labelwidth\leftmarginiv
              \advance\labelwidth-\labelsep}
\def\@listv  {\leftmargin\leftmarginv
              \labelwidth\leftmarginv
              \advance\labelwidth-\labelsep}
\def\@listvi {\leftmargin\leftmarginvi
              \labelwidth\leftmarginvi
              \advance\labelwidth-\labelsep}
%
\setcounter{topnumber}{3}
\def\topfraction{.98}
\setcounter{bottomnumber}{3}
\def\bottomfraction{.98}
\setcounter{totalnumber}{6}%4
\def\textfraction{.01}
\def\floatpagefraction{.98100}
\setcounter{dbltopnumber}{2}
\def\dbltopfraction{.981}
\def\dblfloatpagefraction{.99}
%
\newcounter {prvastrana}
%
\def\ps@myheadings{%
\def\@oddfoot{\ifnum\c@page=\c@prvastrana{
\noindent{\small \publ}\rm\thepage}
\else{}\fi}

\def\@evenfoot{\ifnum\c@page=\c@prvastrana{
\noindent\rm\thepage{\small \publ}}
\else{}\fi}

\def\@evenhead{%
\ifnum\c@page=\c@prvastrana{%
%Proceedings of the DIS'2004,~\v Strbsk\'e Pleso,~Slovakia~p.~\number\c@prvastrana -- \hfill
%Proceedings of the DIS'2004,~\v Strbsk\'e Pleso,~Slovakia\hfill
Internal PSI draft\hfill
}
\else{\thepage\hfill{\sl\runauthor}\qquad}\fi}

\def\@oddhead{%
\ifnum\c@page=\c@prvastrana{%
%Proceedings of the DIS'2004,~\v Strbsk\'e Pleso,~Slovakia~p.~\number\c@prvastrana -- \hfill
%Proceedings of the DIS'2004,~\v Strbsk\'e Pleso,~Slovakia\hfill
Internal PSI draft\hfill
}
\else \qquad{\sl\shorttitle}\hfill\thepage\fi}
}
%
\newcommand\maketitle{\par
  \begingroup
    \renewcommand\thefootnote{\@fnsymbol\c@footnote}%
    \def\@makefnmark{\rlap{\@textsuperscript{\normalfont\@thefnmark}}}%
    \long\def\@makefntext##1{\parindent 1em\noindent
            \hb@xt@1.8em{%
                \hss\@textsuperscript{\normalfont\@thefnmark}}##1}%
 \if@twocolumn
      \ifnum \col@number=\@ne
        \@maketitle
      \else
        \twocolumn[\@maketitle]%
      \fi
    \else
      \newpage
      \global\@topnum\z@   % Prevents figures from going at top of page.
      \@maketitle
    \fi
%    \thispagestyle{plain}
  \@thanks
  \endgroup
  \setcounter{footnote}{0}%
  \setcounter{prvastrana}{\thepage}
  \global\let\thanks\relax
  \global\let\maketitle\relax
  \global\let\@maketitle\relax
  \global\let\@thanks\@empty
  \global\let\@author\@empty
  \global\let\@date\@empty
  \global\let\@title\@empty
  \global\let\title\relax
  \global\let\author\relax
  \global\let\date\relax
  \global\let\and\relax
}
%
%cks\def\title#1{\gdef\@title{\uppercase{#1}}}
\def\title#1{\gdef\@title{\Large{#1}}}
\def\@title{\@latex@error{No \expand\title given}\@ehc}
%%
\def\@maketitle{%
  \newpage
  \null
  \vskip 2em%
%\vspace*{-14pt}
  \begin{center}%
  \let \footnote \thanks
    {\bf \@title \par}%
    \vskip 2em%
   \@aabuffer
\end{center}%
 \par}
%  \vskip 1.5em}
%
\def\@aabuffer{}
\def\author #1{\expandafter\def\expandafter\@aabuffer\expandafter
	{\@aabuffer\rm #1\relax\par
%cks	{\@aabuffer\small\rm\uppercase{#1}\relax\par
	\vspace*{2pt}}}%\vspace{0.75em}
\def\address#1{\expandafter\def\expandafter\@aabuffer\expandafter
	{\@aabuffer\small\it #1\relax\par
	\vspace*{10pt}}}
%
\setcounter{secnumdepth}{2}
\newcounter {section}
\newcounter {subsection}[section]
\renewcommand\thesection      {\@arabic\c@section}
\renewcommand\thesubsection   {\thesection.\@arabic\c@subsection}
%
\newcommand\section{\@startsection {section}{1}{\z@}%
                                   {-3.50ex \@plus -1ex \@minus -.2ex}%
                                   {2.3ex \@plus.2ex}%
                                   {\rightskip1pc plus1fil\normalfont\normalsize\bfseries}}
\newcommand\subsection{\@startsection{subsection}{2}{\z@}%
                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
                                     {1.5ex \@plus .2ex}%
                                     {\rightskip1pc plus1fil\normalfont\normalsize\it }}
%
\setlength\leftmargini  {2em}
\leftmargin  \leftmargini
\setlength\leftmarginii  {2.2em}
\setlength\leftmarginiii {1.87em}
\setlength\leftmarginiv  {1.7em}
\setlength\leftmarginv  {.5em}
\setlength\leftmarginvi {.5em}
\setlength  \labelsep  {.5em}
\setlength  \labelwidth{\leftmargini}
\addtolength\labelwidth{-\labelsep}
\@beginparpenalty -\@lowpenalty
\@endparpenalty   -\@lowpenalty
\@itempenalty     -\@lowpenalty
\renewcommand\theenumi{\@arabic\c@enumi}
\renewcommand\theenumii{\@alph\c@enumii}
\renewcommand\theenumiii{\@roman\c@enumiii}
\renewcommand\theenumiv{\@Alph\c@enumiv}
\newcommand\labelenumi{\theenumi.}
\newcommand\labelenumii{(\theenumii)}
\newcommand\labelenumiii{\theenumiii.}
\newcommand\labelenumiv{\theenumiv.}
\renewcommand\p@enumii{\theenumi}
\renewcommand\p@enumiii{\theenumi(\theenumii)}
\renewcommand\p@enumiv{\p@enumiii\theenumiii}
\newcommand\labelitemi{$\m@th\bullet$}
\newcommand\labelitemii{\normalfont\bfseries --}
\newcommand\labelitemiii{$\m@th\ast$}
\newcommand\labelitemiv{$\m@th\cdot$}
\newenvironment{description}
               {\list{}{\labelwidth\z@ \itemindent-\leftmargin
                        \let\makelabel\descriptionlabel}}
               {\endlist}
\newcommand*\descriptionlabel[1]{\hspace\labelsep
                                \normalfont\bfseries #1}
%

\def\abstracts#1{
\begin{center}
{\begin{minipage}{4.2truein}
                 \footnotesize
                 \parindent=0pt #1\par
                 \end{minipage}}\end{center}
                 \vskip 2em \par}

\newenvironment{quotation}
               {\list{}{\listparindent 1.5em%
                        \itemindent    \listparindent
                        \rightmargin   \leftmargin
                        \parsep        \z@ \@plus\p@}%
                \item\relax}
               {\endlist}
%
\newenvironment{quote}
               {\list{}{\rightmargin\leftmargin}%
                \item\relax}
               {\endlist}
%
%FOLLOWING THREE COMMANDS ARE FOR `LIST' COMMAND.
\topsep=0in
\parsep=0in
\itemsep=0in
%
\newcounter{arabiclistc}
\newenvironment{arabiclist}
	{\setcounter{arabiclistc}{0}
	 \begin{list}{\arabic{arabiclistc}}
	{\usecounter{arabiclistc}
	 \setlength{\parsep}{0pt}
	 \setlength{\itemsep}{0pt}}}{\end{list}}
%
%ACKNOWLEDGEMENT: this portion is from John Hershberger 
%
\def\@citex[#1]#2{\if@filesw\immediate\write\@auxout
	{\string\citation{#2}}\fi
\def\@citea{}\@cite{\@for\@citeb:=#2\do
	{\@citea\def\@citea{,}\@ifundefined
	{b@\@citeb}{{\bf ?}\@warning
	{Citation `\@citeb' on page \thepage \space undefined}}
	{\csname b@\@citeb\endcsname}}}{#1}}
%
\newif\if@cghi
\def\cite{\@cghitrue\@ifnextchar [{\@tempswatrue
	\@citex}{\@tempswafalse\@citex[]}}
\def\citelow{\@cghifalse\@ifnextchar [{\@tempswatrue
	\@citex}{\@tempswafalse\@citex[]}}
%\def\@cite#1#2{{$^{#1}$\if@tempswa\typeout
\def\@cite#1#2{{[#1]\if@tempswa\typeout
	{Warning: optional citation argument ignored: `#2'} \fi}}
\newcommand{\citeup}{\cite}
%
\newcommand\appendix{\par
  \setcounter{section}{0}%
  \setcounter{subsection}{0}%
  \renewcommand\thesection{\@Alph\c@section}
	\section*{\appendixname}}
%
\setlength\arraycolsep{1.5\p@}%5pt
\setlength\tabcolsep{6\p@}
\setlength\arrayrulewidth{.4\p@}
\setlength\doublerulesep{2\p@}
\setlength\tabbingsep{\labelsep}
\skip\@mpfootins = \skip\footins
\setlength\fboxsep{3\p@}
\setlength\fboxrule{.4\p@}
\renewcommand\theequation{\@arabic\c@equation}
%%
\InputIfFileExists{psfig.sty}{}{}
\InputIfFileExists{epsf.sty}{}{}
\InputIfFileExists{epsfsafe.tex}{}{}
\InputIfFileExists{epsfig.sty}{}{}
%
\newcounter{figure}
\renewcommand\thefigure{\@arabic\c@figure}
\def\fps@figure{tbp}
\def\ftype@figure{1}
\def\ext@figure{lof}
\def\fnum@figure{\figurename~\thefigure}
\newenvironment{figure}
               {\@float{figure}}
               {\end@float}
\newenvironment{figure*}
               {\@dblfloat{figure}}
               {\end@dblfloat}
%
\newcounter{table}
\renewcommand\thetable{\@arabic\c@table}
\def\fps@table{tbp}
\def\ftype@table{2}
\def\ext@table{lot}
\def\fnum@table{\tablename~\thetable}
\newenvironment{table}
               {\@float{table}}
               {\end@float}
\newenvironment{table*}
               {\@dblfloat{table}}
               {\end@dblfloat}
%
\newlength\abovecaptionskip
\newlength\belowcaptionskip
\setlength\abovecaptionskip{10\p@}
\setlength\belowcaptionskip{6\p@}
\def\captionfont{\normalfont\footnotesize{}}
\long\def\@makecaption#1#2{%
	\captionfont
 \vskip\abovecaptionskip
  \sbox\@tempboxa{#1.\hskip.5em#2}%
  \ifdim \wd\@tempboxa >\hsize
    \noindent #1.\hskip.5em#2\par
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}
%
\def\figurebox#1#2#3{%
  	\def\arg{#3}%
  	\ifx\arg\empty
  	{\hfill\vbox{\hsize#2\hrule\hbox to #2{\vrule\hfill\vbox to #1{\hsize#2\vfill}\vrule}\hrule}\hfill}%
  	\else
   	{\hfill\epsfbox{#3}\hfill}% 
  	\fi}
%
\def\@figurecaption#1#2{\unskip\vskip10pt{#1.\hskip.5em#2\par}}
%
\DeclareOldFontCommand{\rm}{\normalfont\rmfamily}{\mathrm}
\DeclareOldFontCommand{\sf}{\normalfont\sffamily}{\mathsf}
\DeclareOldFontCommand{\tt}{\normalfont\ttfamily}{\mathtt}
\DeclareOldFontCommand{\bf}{\normalfont\bfseries}{\mathbf}
\DeclareOldFontCommand{\it}{\normalfont\itshape}{\mathit}
\DeclareOldFontCommand{\sl}{\normalfont\slshape}{\@nomath\sl}
\DeclareOldFontCommand{\sc}{\normalfont\scshape}{\@nomath\sc}
\DeclareRobustCommand*\cal{\@fontswitch\relax\mathcal}
\DeclareRobustCommand*\mit{\@fontswitch\relax\mathnormal}
\newcommand\@pnumwidth{1.55em}
\newcommand\@tocrmarg{2.55em}
\newcommand\@dotsep{4.5}
%
\newenvironment{thebibliography}[1]
	{\section*{\refname}
	\begin{list}{\arabic{enumi}.}
	{\usecounter{enumi}\setlength{\parsep}{0pt}
	 \setlength{\itemsep}{0pt} \settowidth
	{\labelwidth}{#1.}\sloppy}}{\end{list}}
%%
\newenvironment{theindex}
               {\if@twocolumn
                  \@restonecolfalse
                \else
                  \@restonecoltrue
                \fi
                \columnseprule \z@
                \columnsep 35\p@
                \twocolumn[\section*{\indexname}]%
                \@mkboth{\MakeUppercase\indexname}%
                        {\MakeUppercase\indexname}%
                \thispagestyle{plain}\parindent\z@
                \parskip\z@ \@plus .3\p@\relax
                \let\item\@idxitem}
               {\if@restonecol\onecolumn\else\clearpage\fi}
\newcommand\@idxitem{\par\hangindent 40\p@}
\newcommand\subitem{\@idxitem \hspace*{20\p@}}
\newcommand\subsubitem{\@idxitem \hspace*{30\p@}}
\newcommand\indexspace{\par \vskip 10\p@ \@plus5\p@ \@minus3\p@\relax}
%
\renewcommand\footnoterule{%
  \kern-3\p@
  \hrule\@width.4\columnwidth
  \kern2.6\p@}
\newcommand\@makefntext[1]{%
    \parindent 1em%
    \noindent
     \@makefnmark#1}
%
\def\thefootnote{\alph{footnote}}
\def\@makefnmark{{$^{\@thefnmark}$}}
%
\newcommand\contentsname{Contents}
\newcommand\listfigurename{List of Figures}
\newcommand\listtablename{List of Tables}
\newcommand\refname{References}
\newcommand\indexname{Index}
\newcommand\figurename{Figure}
\newcommand\tablename{Table}
\newcommand\partname{Part}
\newcommand\appendixname{Appendix}
\newcommand\abstractname{Abstract}
%
%
\bibliographystyle{unsrt} 
\arraycolsep1.5pt
%
%%%%%%%%%%%%%%%%
%
\setlength\columnsep{0.25in}
\setlength\columnseprule{0\p@}

\pagestyle{myheadings}
\pagenumbering{arabic}
\flushbottom
\def\sloppy{\tolerance=100000\hfuzz=\maxdimen\vfuzz=\maxdimen}
\vbadness=12000
\hbadness=12000
\sloppy
\onecolumn
\endinput
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% End %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



