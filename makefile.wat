# Othello for OS/2 - Open Watcom makefile
# Usage: wmake -f makefile.wat

CC      = wcc386
LINK    = wlink
RC      = wrc
WIPFC   = wipfc

CFLAGS  = -bt=os2 -mf -5 -fpi -Oaxt -W3 -ze -d0 -i=src
LFLAGS  = system os2v2 pm option stack=65536 option heap=4096
RCFLAGS = -i=src

TARGET  = bin\othello.exe
RES     = bin\othello.res
DEF     = src\othello.def

OBJS    = bin\othello.obj &
          bin\alg.obj &
          bin\lang.obj

HLPS    = bin\help\Othello_en.hlp bin\help\Othello_es.hlp bin\help\Othello_nl.hlp bin\help\Othello_de.hlp bin\help\Othello_fr.hlp bin\help\Othello_it.hlp

all: $(TARGET) $(HLPS) .SYMBOLIC

$(TARGET): $(OBJS) $(DEF) $(RES)
    $(LINK) $(LFLAGS) name $(TARGET) file {$(OBJS)}
    $(RC) $(RES) $(TARGET)

$(RES): src\othello.rc src\about.dlg src\sound.dlg &
        src\othello.h src\lang.h &
        src\othello.ico src\play.ico src\cross.ptr &
        src\color0.bmp src\color1.bmp src\color2.bmp src\color3.bmp &
        src\color4.bmp src\color5.bmp src\color6.bmp src\color7.bmp &
        src\color8.bmp src\color9.bmp src\color10.bmp src\color11.bmp &
        src\color12.bmp src\color13.bmp src\color14.bmp src\color15.bmp
    $(RC) $(RCFLAGS) -r -fo=$(RES) src\othello.rc

bin\othello.obj: src\othello.c src\othello.h src\lang.h
    $(CC) $(CFLAGS) -fo=bin\othello.obj src\othello.c

bin\alg.obj: src\alg.c src\othello.h src\lang.h
    $(CC) $(CFLAGS) -fo=bin\alg.obj src\alg.c

bin\lang.obj: src\lang.c src\lang.h
    $(CC) $(CFLAGS) -fo=bin\lang.obj src\lang.c

bin\help:
    @if not exist bin\help mkdir bin\help

bin\help\Othello_en.hlp: help\Othello_en.ipf bin\help
    $(WIPFC) -o $@ help\Othello_en.ipf

bin\help\Othello_es.hlp: help\Othello_es.ipf bin\help
    $(WIPFC) -o $@ help\Othello_es.ipf

bin\help\Othello_nl.hlp: help\Othello_nl.ipf bin\help
    $(WIPFC) -o $@ help\Othello_nl.ipf

bin\help\Othello_de.hlp: help\Othello_de.ipf bin\help
    $(WIPFC) -l de_DE -o $@ help\Othello_de.ipf

bin\help\Othello_fr.hlp: help\Othello_fr.ipf bin\help
    $(WIPFC) -l fr_FR -o $@ help\Othello_fr.ipf

bin\help\Othello_it.hlp: help\Othello_it.ipf bin\help
    $(WIPFC) -o $@ help\Othello_it.ipf

clean: .SYMBOLIC
    @if exist bin\help\*.hlp  del bin\help\*.hlp
    @if exist bin\othello.obj del bin\othello.obj
    @if exist bin\alg.obj     del bin\alg.obj
    @if exist bin\lang.obj    del bin\lang.obj
    @if exist bin\othello.res del bin\othello.res
    @if exist bin\othello.exe del bin\othello.exe
    @if exist bin\othello.map del bin\othello.map
