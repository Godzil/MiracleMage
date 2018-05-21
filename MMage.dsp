# Microsoft Developer Studio Project File - Name="MMage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MMage - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "MMage.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "MMage.mak" CFG="MMage - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "MMage - Win32 Release" ("Win32 (x86) Application" 用)
!MESSAGE "MMage - Win32 Debug" ("Win32 (x86) Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MMage - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib comctl32.lib ole32.lib winmm.lib dxguid.lib dinput.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "MMage - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib comctl32.lib ole32.lib winmm.lib dxguid.lib dinput.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MMage - Win32 Release"
# Name "MMage - Win32 Debug"
# Begin Group "wonx"

# PROP Default_Filter ""
# Begin Group "wonx_include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wonx\wonx_include\bank.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\bios.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\comm.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\disp.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\key.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\service.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\sound.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\system.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\system_configure.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\text.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\timer.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_include\types.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\wonx\bank.c
# End Source File
# Begin Source File

SOURCE=.\wonx\comm.c
# End Source File
# Begin Source File

SOURCE=.\wonx\disp.c
# End Source File
# Begin Source File

SOURCE=.\wonx\etc.c
# End Source File
# Begin Source File

SOURCE=.\wonx\etc.h
# End Source File
# Begin Source File

SOURCE=.\wonx\key.c
# End Source File
# Begin Source File

SOURCE=.\wonx\sound.c
# End Source File
# Begin Source File

SOURCE=.\wonx\system.c
# End Source File
# Begin Source File

SOURCE=.\wonx\text.c
# End Source File
# Begin Source File

SOURCE=.\wonx\timer.c
# End Source File
# Begin Source File

SOURCE=.\wonx\UNIXTimer.c
# End Source File
# Begin Source File

SOURCE=.\wonx\UNIXTimer.h
# End Source File
# Begin Source File

SOURCE=.\wonx\UNIXTimerP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonX.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WonX.h
# End Source File
# Begin Source File

SOURCE=.\wonx\wonx_configure.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXDisplay.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXDisplay.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXDisplayP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSerialPort.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSerialPort.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSerialPortP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSystem.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSystem.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXSystemP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXText.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXText.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WonXTextP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWCharacter.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWCharacter.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWCharacterP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWColorMap.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWColorMap.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWColorMapP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWDisplay.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWDisplay.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWDisplayP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWInterrupt.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWInterrupt.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWInterruptP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWLCDPanel.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWLCDPanel.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWLCDPanelP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWPalette.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWPalette.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWPaletteP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWScreen.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWScreen.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWScreenP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSerialPort.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSerialPort.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSerialPortP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSprite.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSprite.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWSpriteP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWText.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWText.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWTextFonts.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWTextP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWTimer.c
# End Source File
# Begin Source File

SOURCE=.\wonx\WWTimer.h
# End Source File
# Begin Source File

SOURCE=.\wonx\WWTimerP.h
# End Source File
# Begin Source File

SOURCE=.\wonx\XDisplay.c
# End Source File
# Begin Source File

SOURCE=.\wonx\XDisplay.h
# End Source File
# Begin Source File

SOURCE=.\wonx\XDisplayP.h
# End Source File
# End Group
# Begin Group "cpu"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cpu\cpu.c
# End Source File
# Begin Source File

SOURCE=.\cpu\cpu.h
# End Source File
# Begin Source File

SOURCE=.\cpu\global.h
# End Source File
# Begin Source File

SOURCE=.\cpu\hardware.c
# End Source File
# Begin Source File

SOURCE=.\cpu\hardware.h
# End Source File
# Begin Source File

SOURCE=.\cpu\i186.h
# End Source File
# Begin Source File

SOURCE=.\cpu\instr.h
# End Source File
# Begin Source File

SOURCE=.\cpu\mytypes.h
# End Source File
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\resource\aux1.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux1off.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux2.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux2off.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux3.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux3off.ico
# End Source File
# Begin Source File

SOURCE=.\resource\aux4.ico
# End Source File
# Begin Source File

SOURCE=.\resource\horizontal.ico
# End Source File
# Begin Source File

SOURCE=.\resource\horizontaloff.ico
# End Source File
# Begin Source File

SOURCE=.\resource\license.bin
# End Source File
# Begin Source File

SOURCE=.\resource\MMAGE.ICO
# End Source File
# Begin Source File

SOURCE=.\resource\MMage.rc
# End Source File
# Begin Source File

SOURCE=.\resource\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource\sleep.ico
# End Source File
# Begin Source File

SOURCE=.\resource\sleepoff.ico
# End Source File
# Begin Source File

SOURCE=.\resource\SMALL.ICO
# End Source File
# Begin Source File

SOURCE=.\resource\title.bmp
# End Source File
# Begin Source File

SOURCE=.\resource\vertical.ico
# End Source File
# Begin Source File

SOURCE=.\resource\verticaloff.ico
# End Source File
# End Group
# Begin Group "mmage"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mmage\mmage.cpp
# End Source File
# Begin Source File

SOURCE=.\mmage\mmage.h
# End Source File
# Begin Source File

SOURCE=.\mmage\preference.cpp
# End Source File
# Begin Source File

SOURCE=.\mmage\preference.h
# End Source File
# Begin Source File

SOURCE=.\mmage\UNIXcompat.cpp
# End Source File
# End Group
# Begin Group "mmvm"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mmvm\dispbios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\filesys.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\filesys.h
# End Source File
# Begin Source File

SOURCE=.\mmvm\kanjifont.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\kanjifont.h
# End Source File
# Begin Source File

SOURCE=.\mmvm\keybios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\mmvm.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\mmvm.h
# End Source File
# Begin Source File

SOURCE=.\mmvm\systembios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\textbios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\timerbios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\wwbios.cpp
# End Source File
# Begin Source File

SOURCE=.\mmvm\wwbios.h
# End Source File
# Begin Source File

SOURCE=.\mmvm\wwstruct.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\HISTORY.txt
# End Source File
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
