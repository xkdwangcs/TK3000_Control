# Microsoft Developer Studio Project File - Name="SWITCH" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=SWITCH - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SWITCH.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SWITCH.mak" CFG="SWITCH - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SWITCH - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "SWITCH - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SWITCH - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "STM32F10X_HD" /D "USE_STDPERIPH_DRIVER"  /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\STM32F10x_FWLib\inc" /I "..\STM32F10x_FWLib\src" /I "..\USER" /I "..\SYSTEM\delay" /I "..\SYSTEM\sys" /I "..\SYSTEM\usart" /I "..\FreeModbus\modbus" /I "..\FreeModbus\port" /I "..\CORE" /I "d:\OtherProgram\Keil_mdk_v474\ARM\RV31\INC"  /D "STM32F10X_HD" /D "USE_STDPERIPH_DRIVER"  /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 user32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "SWITCH - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "STM32F10X_HD" /D "USE_STDPERIPH_DRIVER"  /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\STM32F10x_FWLib\inc" /I "..\STM32F10x_FWLib\src" /I "..\USER" /I "..\SYSTEM\delay" /I "..\SYSTEM\sys" /I "..\SYSTEM\usart" /I "..\FreeModbus\modbus" /I "..\FreeModbus\port" /I "..\CORE" /I "d:\OtherProgram\Keil_mdk_v474\ARM\RV31\INC"  /D "STM32F10X_HD" /D "USE_STDPERIPH_DRIVER"  /YX /FD /GZ  /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 user32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SWITCH - Win32 Release"
# Name "SWITCH - Win32 Debug"
# Begin Group "CORE"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\CORE\core_cm3.c"
# End Source File
# Begin Source File

SOURCE="..\CORE\startup_stm32f10x_hd.s"
# End Source File
# End Group
# Begin Group "FWLIB"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\misc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_adc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_bkp.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_can.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_cec.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_crc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_dac.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_dbgmcu.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_dma.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_exti.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_flash.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_fsmc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_gpio.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_i2c.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_iwdg.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_pwr.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_rcc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_rtc.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_sdio.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_spi.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_tim.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_usart.c"
# End Source File
# Begin Source File

SOURCE="..\STM32F10x_FWLib\src\stm32f10x_wwdg.c"
# End Source File
# End Group
# Begin Group "USER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\main.c"
# End Source File
# Begin Source File

SOURCE=".\stm32f10x_it.c"
# End Source File
# Begin Source File

SOURCE=".\system_stm32f10x.c"
# End Source File
# Begin Source File

SOURCE=".\nvic.c"
# End Source File
# Begin Source File

SOURCE=".\exti.c"
# End Source File
# Begin Source File

SOURCE=".\gpio.c"
# End Source File
# Begin Source File

SOURCE=".\time.c"
# End Source File
# Begin Source File

SOURCE=".\Public.c"
# End Source File
# Begin Source File

SOURCE=".\Delay.c"
# End Source File
# End Group
# Begin Group "DAL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\DAL\File.c"
# End Source File
# Begin Source File

SOURCE=".\DAL\Parameter.c"
# End Source File
# Begin Source File

SOURCE=".\DAL\SST25V.c"
# End Source File
# End Group
# Begin Group "UI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\UI\Lcddrive.c"
# End Source File
# Begin Source File

SOURCE=".\UI\lcdRcvCmd.c"
# End Source File
# Begin Source File

SOURCE=".\UI\MainForm.c"
# End Source File
# Begin Source File

SOURCE=".\UI\FileManager.c"
# End Source File
# Begin Source File

SOURCE=".\UI\TextBox.c"
# End Source File
# Begin Source File

SOURCE=".\UI\Button.c"
# End Source File
# Begin Source File

SOURCE=".\UI\QLCoord.c"
# End Source File
# Begin Source File

SOURCE=".\UI\CoordEdit.c"
# End Source File
# Begin Source File

SOURCE=".\UI\WorkParameter.c"
# End Source File
# Begin Source File

SOURCE=".\UI\FileSelect.c"
# End Source File
# Begin Source File

SOURCE=".\UI\CoordList.c"
# End Source File
# Begin Source File

SOURCE=".\UI\MotorParameter.c"
# End Source File
# Begin Source File

SOURCE=".\UI\CylinderParameter.c"
# End Source File
# Begin Source File

SOURCE=".\UI\IOTest.c"
# End Source File
# Begin Source File

SOURCE=".\UI\TextBoxEx.c"
# End Source File
# Begin Source File

SOURCE=".\UI\ControlBase.c"
# End Source File
# Begin Source File

SOURCE=".\UI\ParaSelectForm.c"
# End Source File
# Begin Source File

SOURCE=".\UI\SystemParaSet.c"
# End Source File
# Begin Source File

SOURCE=".\UI\SystemInfo.c"
# End Source File
# End Group
# Begin Group "COM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\COM\CMD.c"
# End Source File
# Begin Source File

SOURCE=".\COM\COM.c"
# End Source File
# Begin Source File

SOURCE=".\COM\Protocol.c"
# End Source File
# End Group
# Begin Group "BLL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\BLL\Coordition.c"
# End Source File
# Begin Source File

SOURCE=".\BLL\StepMotor.c"
# End Source File
# Begin Source File

SOURCE=".\BLL\WorkControl.c"
# End Source File
# End Group
# End Target
# End Project
