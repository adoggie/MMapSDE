# Microsoft Developer Studio Project File - Name="FileDataStruct" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=FileDataStruct - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FileDataStruct.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FileDataStruct.mak" CFG="FileDataStruct - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FileDataStruct - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FileDataStruct - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/04-ExecuteLib/04-Code/SDE_070415", DNGAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FileDataStruct - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./" /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "FileDataStruct - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "FileDataStruct - Win32 Release"
# Name "FileDataStruct - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\TypeTranslateTool.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FileDataStruct.h
# End Source File
# Begin Source File

SOURCE=.\GeoGlobe.h
# End Source File
# Begin Source File

SOURCE=.\GIS_BreadthMap.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkRut.h
# End Source File
# Begin Source File

SOURCE=.\GIS_NetworkVtx.h
# End Source File
# Begin Source File

SOURCE=.\NetworkFileStruct.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TypeTranslateTool.h
# End Source File
# End Group
# Begin Group "V5"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\V5\GIS_FeatureRoadSectionV5.cpp
# End Source File
# Begin Source File

SOURCE=.\V5\GIS_FeatureRoadSectionV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\GIS_NetworkRoadSectionV5.cpp
# End Source File
# Begin Source File

SOURCE=.\V5\GIS_NetworkRoadSectionV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\GIS_NetworkVertexV5.cpp
# End Source File
# Begin Source File

SOURCE=.\V5\GIS_NetworkVertexV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\NetworkFileDataV5.cpp
# End Source File
# Begin Source File

SOURCE=.\V5\NetworkFileDataV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\NetworkFileStructV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\SpaceFileDataV5.cpp
# End Source File
# Begin Source File

SOURCE=.\V5\SpaceFileDataV5.h
# End Source File
# Begin Source File

SOURCE=.\V5\SpaceFileStructV5.h
# End Source File
# End Group
# Begin Group "V4"

# PROP Default_Filter ""
# End Group
# Begin Group "V3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\V3\GIS_FeatureRoadSectionV3.cpp
# End Source File
# Begin Source File

SOURCE=.\V3\GIS_FeatureRoadSectionV3.h
# End Source File
# Begin Source File

SOURCE=.\V3\GIS_NetworkRoadSectionV3.cpp
# End Source File
# Begin Source File

SOURCE=.\V3\GIS_NetworkRoadSectionV3.h
# End Source File
# Begin Source File

SOURCE=.\V3\GIS_NetworkVertexV3.cpp
# End Source File
# Begin Source File

SOURCE=.\V3\GIS_NetworkVertexV3.h
# End Source File
# End Group
# Begin Group "V2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\v2\GIS_FeatureLineV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeatureLineV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeaturePointV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeaturePointV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeaturePolyV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeaturePolyV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeatureRoadSectionV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeatureRoadSectionV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeatureV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_FeatureV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_NetworkRoadSectionV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_NetworkRoadSectionV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_NetworkVertexV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\GIS_NetworkVertexV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileDataV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileDataV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileDataV2_LoadAll.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileDataV2_LoadAll.h
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileStruct.h
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileStructV2B.h
# End Source File
# Begin Source File

SOURCE=.\v2\NetworkFileStructV2C.h
# End Source File
# Begin Source File

SOURCE=.\v2\SpaceFileDataV2.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\SpaceFileDataV2.h
# End Source File
# Begin Source File

SOURCE=.\v2\SpaceFileDataV2_RoadSection.cpp
# End Source File
# Begin Source File

SOURCE=.\v2\SpaceFileDataV2_RoadSection.h
# End Source File
# Begin Source File

SOURCE=.\v2\SpaceFileStructV2.h
# End Source File
# End Group
# Begin Group "V1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\v1\GIS_BreadthMap.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_BreadthMap.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_Feature.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_Feature.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeatureLine.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeatureLine.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeaturePoint.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeaturePoint.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeaturePoly.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeaturePoly.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeatureRut.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_FeatureRut.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_NetworkRut.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_NetworkRut.h
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_NetworkVtx.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\GIS_NetworkVtx.h
# End Source File
# Begin Source File

SOURCE=.\v1\IN2FileDataV1.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\IN2FileDataV1.h
# End Source File
# Begin Source File

SOURCE=.\v1\NetworkFileDataV1.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\NetworkFileDataV1.h
# End Source File
# Begin Source File

SOURCE=.\v1\NetworkFileStructV1.h
# End Source File
# Begin Source File

SOURCE=.\v1\SpaceFileDataV1.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\SpaceFileDataV1.h
# End Source File
# Begin Source File

SOURCE=.\v1\SpaceFileStructV1.h
# End Source File
# Begin Source File

SOURCE=.\v1\TempGeoMap.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\TempGeoMap.h
# End Source File
# Begin Source File

SOURCE=.\v1\TempLayerInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\TempLayerInfo.h
# End Source File
# Begin Source File

SOURCE=.\v1\TempLayerManager.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\TempLayerManager.h
# End Source File
# Begin Source File

SOURCE=.\v1\TempMapInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\TempMapInfo.h
# End Source File
# Begin Source File

SOURCE=.\v1\TempMapLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\v1\TempMapLayer.h
# End Source File
# End Group
# Begin Group "ZLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\compress.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\crc32.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\infback.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inflate.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\trees.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\trees.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zconf.in.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=.\Zlib\zutil.c
# End Source File
# Begin Source File

SOURCE=.\Zlib\zutil.h
# End Source File
# End Group
# Begin Group "V6"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\V6\GIS_FeatureRoadSectionV6.cpp
# End Source File
# Begin Source File

SOURCE=.\V6\GIS_FeatureRoadSectionV6.h
# End Source File
# Begin Source File

SOURCE=.\V6\GIS_NetworkRoadSectionV6.cpp
# End Source File
# Begin Source File

SOURCE=.\V6\GIS_NetworkRoadSectionV6.h
# End Source File
# Begin Source File

SOURCE=.\V6\GIS_NetworkVertexV6.cpp
# End Source File
# Begin Source File

SOURCE=.\V6\GIS_NetworkVertexV6.h
# End Source File
# End Group
# Begin Group "V7"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\V7\GIS_FeatureRoadSectionV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_FeatureRoadSectionV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_FeatureRoadV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_FeatureRoadV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_NetworkRoadSectionV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_NetworkRoadSectionV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_NetworkVertexV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\GIS_NetworkVertexV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkFileDataV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkFileDataV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkFileStructV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkIDFileDataV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkIDFileDataV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\NetworkIDFileStructV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\SpaceFileDataV7.cpp
# End Source File
# Begin Source File

SOURCE=.\V7\SpaceFileDataV7.h
# End Source File
# Begin Source File

SOURCE=.\V7\SpaceFileStructV7.h
# End Source File
# End Group
# End Target
# End Project
