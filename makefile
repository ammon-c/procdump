# NMAKE script to build the procdump program from the C source code.

.SUFFIXES:  .c

.c.obj:
    cl -c -W4 -DWIN32 $<

all:  procdump.exe

procdump.exe:  procdump.obj winproclist.obj
    link /NOLOGO /DEBUG /OUT:$@ $** ntdll.lib

procdump.obj:     procdump.c     winproclist.h
winproclist.obj:  winproclist.c  winproclist.h

clean:
    if exist *.obj del *.obj
    if exist *.pdb del *.pdb
    if exist *.ilk del *.ilk
    if exist *.exe del *.exe
    if exist *.bak del *.bak
    if exist .vs rd /s /q .vs
