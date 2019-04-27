# IBM Developer's Workframe/2 Make File Creation run at 03:12:00 on 01/30/94

# Make File Creation run in directory:
#   C:\SOURCE\OTHELLO;

.SUFFIXES:

.SUFFIXES: .c .cpp .cxx .rc .ipf

ALL: OTHELLO.EXE \
     othello.HLP \
     othello.RES

OTHELLO.EXE:  \
  alg.OBJ \
  othello.OBJ \
  othello.RES \
  MAKEFILE
   ICC.EXE @<<
 /B" /nod"
 /Fe"OTHELLO.EXE" DDE4MBS.LIB OS2386.LIB OTHELLO.DEF 
alg.OBJ 
othello.OBJ
<<
   RC othello.RES OTHELLO.EXE

{.}.rc.res:
   RC -r .\$*.RC

{.}.ipf.hlp:
   IPFC .\$*.IPF

{.}.c.obj:
   ICC.EXE /Tdc /Ss /Wdcleffextgenparprorearetuse /O /Gm /G4 /Ms /C   .\$*.c

{.}.cpp.obj:
   ICC.EXE /Tdc /Ss /Wdcleffextgenparprorearetuse /O /Gm /G4 /Ms /C   .\$*.cpp

{.}.cxx.obj:
   ICC.EXE /Tdc /Ss /Wdcleffextgenparprorearetuse /O /Gm /G4 /Ms /C   .\$*.cxx

!include MAKEFILE.DEP
