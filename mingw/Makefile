.SUFFIXES: .rc .res .ro .cpp .obj
CC=e:/mingw/gcc-2.95.2/bin/gcc.exe
CPP=e:/mingw/gcc-2.95.2/bin/g++.exe
CFLAGS=-c -mno-cygwin -mwindows -mpentiumpro -Ie:/mingw/directx/include \
	-DNDEBUG -DINT32=int \
	-DUINT32=DWORD -D_WIN32 -D_WINDOWS -D_WIN32_WINNT=0x400 \
	-DINLINE_FUNCTIONS -O9 -fvtable-thunks # -DSHOW_RENDERTIME 
LDFLAGS=-O9 -mno-cygwin -mwindows 
LIBS=-ldinput -lcomctl32 -lole32 -lwinmm
RES=rc.exe
GORC=e:/mingw/Gorcjorg/GORC.EXE

WONX_OBJS=bank.o comm.o disp.o etc.o key.o sound.o \
	system.o text.o timer.o UNIXTimer.o WonX.o \
	WWDisplay.o WonXSerialPort.o WonXSystem.o \
	WWText.o WWCharacter.o WWColorMap.o WWDisplay.o \
	WWInterrupt.o WWLCDPanel.o WWPalette.o WWScreen.o \
	WWSerialPort.o WWSprite.o WWText.o WWTextFonts.o \
	WWTimer.o XDisplay.o

CPU_OBJS=cpu.o hardware.o

RESOURCES=MMage.obj

MMAGE_OBJS=mmage.o preference.o UNIXcompat.o

MMVM_OBJS=dispbios.o filesys.o kanjifont.o keybios.o \
	 mmvm.o systembios.o textbios.o timerbios.o wwbios.o

.c.o:
	$(CC) $(CFLAGS) -o $@ $<
.cpp.o:
	$(CPP) $(CFLAGS) -o $@ $<
.rc.res:
	$(RES) -l0x411 -d"NDEBUG" -fo$@ -r $<
.res.obj:
	$(GORC) -o -fo$@ $<

all: MMage.exe

clean:
	rm -f *.o *.obj *.res MMage.exe

MMage.exe: $(WONX_OBJS) $(CPU_OBJS) $(RESOURCES) $(MMAGE_OBJS) $(MMVM_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(WONX_OBJS) $(CPU_OBJS) $(MMAGE_OBJS) $(MMVM_OBJS) $(RESOURCES) $(LIBS)

bank.o: ../wonx/bank.c
comm.o: ../wonx/comm.c
disp.o: ../wonx/disp.c
etc.o: ../wonx/etc.c
key.o: ../wonx/key.c
sound.o: ../wonx/sound.c
system.o: ../wonx/system.c
text.o: ../wonx/text.c
timer.o: ../wonx/timer.c
UNIXTimer.o: ../wonx/UNIXTimer.c
WonX.o: ../wonx/WonX.c
WonXDisplay.o: ../wonx/WonXDisplay.c
WonXSerialPort.o: ../wonx/WonXSerialPort.c
WonXSystem.o: ../wonx/WonXSystem.c
WonXText.o: ../wonx/WonXText.c
WWCharacter.o: ../wonx/WWCharacter.c
WWColorMap.o: ../wonx/WWColorMap.c
WWDisplay.o: ../wonx/WWDisplay.c
WWInterrupt.o: ../wonx/WWInterrupt.c
WWLCDPanel.o: ../wonx/WWLCDPanel.c
WWPalette.o: ../wonx/WWPalette.c
WWScreen.o: ../wonx/WWScreen.c
WWSerialPort.o: ../wonx/WWSerialPort.c
WWSprite.o: ../wonx/WWSprite.c
WWText.o: ../wonx/WWText.c
WWTextFonts.o: ../wonx/WWTextFonts.c
WWTimer.o: ../wonx/WWTimer.c
XDisplay.o: ../wonx/XDisplay.c

cpu.o: ../cpu/cpu.c
hardware.o: ../cpu/hardware.c

MMage.obj: MMage.res

MMage.res: ../resource/MMage.rc

mmage.o: ../mmage/mmage.cpp
preference.o: ../mmage/preference.cpp
UNIXcompat.o: ../mmage/UNIXcompat.cpp

dispbios.o: ../mmvm/dispbios.cpp
filesys.o: ../mmvm/filesys.cpp
kanjifont.o: ../mmvm/kanjifont.cpp
keybios.o: ../mmvm/keybios.cpp
mmvm.o: ../mmvm/mmvm.cpp
systembios.o: ../mmvm/systembios.cpp
textbios.o: ../mmvm/textbios.cpp
timerbios.o: ../mmvm/timerbios.cpp
wwbios.o: ../mmvm/wwbios.cpp
