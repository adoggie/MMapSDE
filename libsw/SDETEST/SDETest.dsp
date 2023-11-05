# Microsoft Developer Studio Project File - Name="SDETest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SDETest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDETest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDETest.mak" CFG="SDETest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDETest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SDETest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/04-ExecuteLib/04-Code/SDE_070415", DNGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SDETest - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../sde" /I "./query" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SDETest - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../sde" /I "./query" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "_AFXDLL" /D "_ANALYSE_DATA_FILE" /D "VERSION_NATIVE" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /profile /debug /machine:I386 /libpath:".\SDE\pc"

!ENDIF 

# Begin Target

# Name "SDETest - Win32 Release"
# Name "SDETest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoMoveByRoute.cpp

!IF  "$(CFG)" == "SDETest - Win32 Release"

!ELSEIF  "$(CFG)" == "SDETest - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\BlockID.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\DlgCommonList.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\DlgTestFrame.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\DlgTestMain.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\DlgTestNavi.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\EditDlg.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\ExpandMap.cpp

!IF  "$(CFG)" == "SDETest - Win32 Release"

!ELSEIF  "$(CFG)" == "SDETest - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Geo.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\MainControl.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\MapBar.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\NaviTestBar.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\QueryDlg.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\RichPOI.cpp

!IF  "$(CFG)" == "SDETest - Win32 Release"

!ELSEIF  "$(CFG)" == "SDETest - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RouteMng.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\SDETest.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\SDETest.rc
# End Source File
# Begin Source File

SOURCE=.\SDETestDoc.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\SDETestView.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\SimulNavInfo.cpp

!IF  "$(CFG)" == "SDETest - Win32 Release"

!ELSEIF  "$(CFG)" == "SDETest - Win32 Debug"

# ADD CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /YX
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoMoveByRoute.h
# End Source File
# Begin Source File

SOURCE=.\BlockID.h
# End Source File
# Begin Source File

SOURCE=.\DlgCommonList.h
# End Source File
# Begin Source File

SOURCE=.\DlgTestMain.h
# End Source File
# Begin Source File

SOURCE=.\ExpandMap.h
# End Source File
# Begin Source File

SOURCE=.\MainControl.h
# End Source File
# Begin Source File

SOURCE=.\MapBar.h
# End Source File
# Begin Source File

SOURCE=.\NaviTestBar.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RichPOI.h
# End Source File
# Begin Source File

SOURCE=.\RouteMng.h
# End Source File
# Begin Source File

SOURCE=.\SDETest.h
# End Source File
# Begin Source File

SOURCE=.\SDETestDoc.h
# End Source File
# Begin Source File

SOURCE=.\SimulNavInfo.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Cmdbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\SDETest.ico
# End Source File
# Begin Source File

SOURCE=.\res\SDETest.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SDETestDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Group "TestFrame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TestFrame\CIniFile.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\TestFrame\CIniFile.h
# End Source File
# Begin Source File

SOURCE=.\TestFrame\CommandAction.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\TestFrame\CommandAction.h
# End Source File
# Begin Source File

SOURCE=.\TestFrame\ScriptAnalyzer.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\TestFrame\ScriptAnalyzer.h
# End Source File
# End Group
# Begin Group "Query"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Query\IGIS_Query2.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Query\IGIS_Query2.h
# End Source File
# Begin Source File

SOURCE=.\Query\QueryProxy.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Query\QueryProxy.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
