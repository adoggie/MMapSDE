# Microsoft Developer Studio Project File - Name="SDELib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=SDELib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SDELib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SDELib.mak" CFG="SDELib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SDELib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SDELib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/04-ExecuteLib/04-Code/SDE_070415", DNGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SDELib - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I ".\\" /I "..\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_UNICODE" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 version.lib /nologo /subsystem:windows /dll /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /w /W0 /Gm /GX /ZI /Od /I ".\\" /I "..\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_UNICODE" /D "_USRDLL" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=postbuild debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "SDELib - Win32 Release"
# Name "SDELib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\GeoGlobe.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GeoView.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Address.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Attribute.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_BreadthMap.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_BufStorage.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_BufStorageNetwork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Feature.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureEditer.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureLine.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeaturePoint.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeaturePoly.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureRoute.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureRut.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureTrack.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Field.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_GeoMap.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LayerAttInfo.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LayerConfig.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LayerInfo.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LayerManager.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LayerTool.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_LFBand.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MapBreadthNetwork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MapConfig.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MapDataManager.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MapInfo.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MapLayer.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Message.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MessageValue.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MFBand.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_Middle.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetWork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkException.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkNode.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkNodeRut.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkPath.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkPathInfo.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkRoute.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkRut.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkStatus.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkVtx.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_PathPoint.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointMark.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointNode.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointNodeV2.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_POIQuery.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_QueryByAtt.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_QueryByShape.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_TrackPoint.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_UFNavi.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_UserLayer.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_UserMap.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_ValueManager.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_WayPoint.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Globe.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\IArchive.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Config.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Editer.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_GeoFile.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Layer.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_LayerFile.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_LayerTool.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Map.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_MapFile.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_MapManager.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_MapNetwork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_MapView.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Message.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_NetWork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_NetworkRoute.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_Query.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_SDETool.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IGIS_TOOL.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\IMemoryMngSet.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MD5.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForNetworkV2.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForQueryData.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForSpaceDataV2.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\MemoryMngSetV2.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RichPOIView.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RichPOIView.h
# End Source File
# Begin Source File

SOURCE=.\SDEControler.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SDELib.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SDELibMethod.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Winarchive.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Geo.h
# End Source File
# Begin Source File

SOURCE=.\GeoGlobe.h
# End Source File
# Begin Source File

SOURCE=.\GeoView.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Address.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Attribute.h
# End Source File
# Begin Source File

SOURCE=.\GIS_BreadthMap.h
# End Source File
# Begin Source File

SOURCE=.\GIS_BufStorage.h
# End Source File
# Begin Source File

SOURCE=.\GIS_BufStorageNetwork.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Feature.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureEditer.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureLine.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeaturePoint.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeaturePoly.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureRoute.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureRut.h
# End Source File
# Begin Source File

SOURCE=.\GIS_FeatureTrack.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Field.h
# End Source File
# Begin Source File

SOURCE=.\GIS_GeoMap.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LayerAttInfo.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LayerConfig.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LayerInfo.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LayerManager.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LayerTool.h
# End Source File
# Begin Source File

SOURCE=.\GIS_LFBand.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MapBreadthNetwork.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MapConfig.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MapDataManager.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MapInfo.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MapLayer.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Message.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MessageValue.h
# End Source File
# Begin Source File

SOURCE=.\GIS_MFBand.h
# End Source File
# Begin Source File

SOURCE=.\GIS_Middle.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetWork.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkException.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkNode.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkNodeRut.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkPath.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkPathInfo.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkRoute.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkRut.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkStatus.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkVoice.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkVtx.h
# End Source File
# Begin Source File

SOURCE=.\GIS_PathPoint.h
# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointMark.h
# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointNode.h
# End Source File
# Begin Source File

SOURCE=.\GIS_PathpointNodeV2.h
# End Source File
# Begin Source File

SOURCE=.\GIS_POIQuery.h
# End Source File
# Begin Source File

SOURCE=.\GIS_QueryByAtt.h
# End Source File
# Begin Source File

SOURCE=.\GIS_QueryByShape.h
# End Source File
# Begin Source File

SOURCE=.\GIS_TrackPoint.h
# End Source File
# Begin Source File

SOURCE=.\GIS_UFNavi.h
# End Source File
# Begin Source File

SOURCE=.\GIS_UserLayer.h
# End Source File
# Begin Source File

SOURCE=.\GIS_UserMap.h
# End Source File
# Begin Source File

SOURCE=.\GIS_ValueManager.h
# End Source File
# Begin Source File

SOURCE=.\GIS_WayPoint.h
# End Source File
# Begin Source File

SOURCE=.\Globe.h
# End Source File
# Begin Source File

SOURCE=.\IArchive.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Config.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Editer.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_GeoFile.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Layer.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_LayerFile.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_LayerTool.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Map.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_MapFile.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_MapManager.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_MapNetwork.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_MapView.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Message.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_NetWork.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_NetworkRoute.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Query.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_Query2.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_SDETool.h
# End Source File
# Begin Source File

SOURCE=.\IGIS_TOOL.h
# End Source File
# Begin Source File

SOURCE=.\IMemoryMngSet.h
# End Source File
# Begin Source File

SOURCE=.\MD5.h
# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForNetworkV2.h
# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForQueryData.h
# End Source File
# Begin Source File

SOURCE=.\MemoryDispatchForSpaceDataV2.h
# End Source File
# Begin Source File

SOURCE=.\MemoryMngSet.h
# End Source File
# Begin Source File

SOURCE=.\MemoryMngSetV2.h
# End Source File
# Begin Source File

SOURCE=.\newres.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SDEControler.h
# End Source File
# Begin Source File

SOURCE=.\SDELib.h
# End Source File
# Begin Source File

SOURCE=.\SDELibMethod.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\res\Struct.h
# End Source File
# Begin Source File

SOURCE=.\Winarchive.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\80232009.bmp
# End Source File
# Begin Source File

SOURCE=.\res\all.bmp
# End Source File
# Begin Source File

SOURCE=.\res\flag.ico
# End Source File
# Begin Source File

SOURCE=.\res\mapcompass.bmp
# End Source File
# Begin Source File

SOURCE=.\res\MidFlag.ico
# End Source File
# Begin Source File

SOURCE=.\res\obj_icon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SDELib.rc2
# End Source File
# Begin Source File

SOURCE=.\res\UnKnow.bmp
# End Source File
# End Group
# Begin Group "ZLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Zlib\adler32.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\compress.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\crc32.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\deflate.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\gzio.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\infback.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffast.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\inflate.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\inftrees.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\minigzip.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\trees.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\uncompr.c
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\Zlib\zutil.c
# ADD CPP /YX
# End Source File
# End Group
# Begin Group "Trunk"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GIS_MBBNetwork.cpp

!IF  "$(CFG)" == "SDELib - Win32 Release"

# ADD CPP /YX

!ELSEIF  "$(CFG)" == "SDELib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GIS_MBBNetwork.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\SDELib.def
# End Source File
# Begin Source File

SOURCE=.\SDELib.rc
# End Source File
# End Target
# End Project
