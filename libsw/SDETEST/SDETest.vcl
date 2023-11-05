<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: SDELib - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAA5.tmp" with contents
[
/nologo /W3 /Zi /Od /I "./MemoryMngLib" /I "..\\" /I ".\\" /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=420 /D _WIN32_WCE=420 /D "WCE_PLATFORM_V4IEDK9101" /D "UNICODE" /D "_UNICODE" /D "_AFXDLL" /D "_USRDLL" /Fp"ARMV4IDbg/SDELib.pch" /YX /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /Gz /QRarch4T /QRinterwork-return /MC /c 
"E:\WorkStation-3.5\WorkStation-Asstitant\SDE\GIS_MapBreadthNetwork.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAA5.tmp" 
Creating temporary file "C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAA6.tmp" with contents
[
/nologo /base:"0x00100000" /stack:0x10000,0x1000 /entry:"_DllMainCRTStartup" /dll /incremental:yes /pdb:"ARMV4IDbg/SDELib.pdb" /debug /def:".\SDELib.def" /out:"ARMV4IDbg/SDELib.dll" /implib:"ARMV4IDbg/SDELib.lib" /subsystem:windowsce,4.20 /MACHINE:THUMB 
".\ARMV4IDbg\GeoGlobe.obj"
".\ARMV4IDbg\GeoView.obj"
".\ARMV4IDbg\GIS_Address.obj"
".\ARMV4IDbg\GIS_Attribute.obj"
".\ARMV4IDbg\GIS_BreadthMap.obj"
".\ARMV4IDbg\GIS_BufStorage.obj"
".\ARMV4IDbg\GIS_BufStorageNetwork.obj"
".\ARMV4IDbg\GIS_Feature.obj"
".\ARMV4IDbg\GIS_FeatureEditer.obj"
".\ARMV4IDbg\GIS_FeatureLine.obj"
".\ARMV4IDbg\GIS_FeaturePoint.obj"
".\ARMV4IDbg\GIS_FeaturePoly.obj"
".\ARMV4IDbg\GIS_FeatureRoute.obj"
".\ARMV4IDbg\GIS_FeatureRut.obj"
".\ARMV4IDbg\GIS_FeatureTrack.obj"
".\ARMV4IDbg\GIS_Field.obj"
".\ARMV4IDbg\GIS_GeoMap.obj"
".\ARMV4IDbg\GIS_LayerAttInfo.obj"
".\ARMV4IDbg\GIS_LayerConfig.obj"
".\ARMV4IDbg\GIS_LayerInfo.obj"
".\ARMV4IDbg\GIS_LayerManager.obj"
".\ARMV4IDbg\GIS_LayerTool.obj"
".\ARMV4IDbg\GIS_LFBand.obj"
".\ARMV4IDbg\GIS_MapBreadthNetwork.obj"
".\ARMV4IDbg\GIS_MapConfig.obj"
".\ARMV4IDbg\GIS_MapDataManager.obj"
".\ARMV4IDbg\GIS_MapInfo.obj"
".\ARMV4IDbg\GIS_MapLayer.obj"
".\ARMV4IDbg\GIS_Message.obj"
".\ARMV4IDbg\GIS_MessageValue.obj"
".\ARMV4IDbg\GIS_MFBand.obj"
".\ARMV4IDbg\GIS_Middle.obj"
".\ARMV4IDbg\GIS_NetWork.obj"
".\ARMV4IDbg\GIS_NetworkException.obj"
".\ARMV4IDbg\GIS_NetworkNode.obj"
".\ARMV4IDbg\GIS_NetworkNodeRut.obj"
".\ARMV4IDbg\GIS_NetworkPath.obj"
".\ARMV4IDbg\GIS_NetworkPathInfo.obj"
".\ARMV4IDbg\GIS_NetworkRoute.obj"
".\ARMV4IDbg\GIS_NetworkRut.obj"
".\ARMV4IDbg\GIS_NetworkVtx.obj"
".\ARMV4IDbg\GIS_PathPoint.obj"
".\ARMV4IDbg\GIS_PathpointMark.obj"
".\ARMV4IDbg\GIS_PathpointNode.obj"
".\ARMV4IDbg\GIS_POIQuery.obj"
".\ARMV4IDbg\GIS_QueryByAtt.obj"
".\ARMV4IDbg\GIS_QueryByShape.obj"
".\ARMV4IDbg\GIS_TrackPoint.obj"
".\ARMV4IDbg\GIS_UFNavi.obj"
".\ARMV4IDbg\GIS_UserLayer.obj"
".\ARMV4IDbg\GIS_UserMap.obj"
".\ARMV4IDbg\GIS_ValueManager.obj"
".\ARMV4IDbg\GIS_WayPoint.obj"
".\ARMV4IDbg\Globe.obj"
".\ARMV4IDbg\IArchive.obj"
".\ARMV4IDbg\IGIS_Config.obj"
".\ARMV4IDbg\IGIS_Editer.obj"
".\ARMV4IDbg\IGIS_GeoFile.obj"
".\ARMV4IDbg\IGIS_Layer.obj"
".\ARMV4IDbg\IGIS_LayerFile.obj"
".\ARMV4IDbg\IGIS_LayerTool.obj"
".\ARMV4IDbg\IGIS_Map.obj"
".\ARMV4IDbg\IGIS_MapFile.obj"
".\ARMV4IDbg\IGIS_MapManager.obj"
".\ARMV4IDbg\IGIS_MapNetwork.obj"
".\ARMV4IDbg\IGIS_MapView.obj"
".\ARMV4IDbg\IGIS_Message.obj"
".\ARMV4IDbg\IGIS_NetWork.obj"
".\ARMV4IDbg\IGIS_NetworkRoute.obj"
".\ARMV4IDbg\IGIS_Query.obj"
".\ARMV4IDbg\IGIS_SDETool.obj"
".\ARMV4IDbg\IGIS_TOOL.obj"
".\ARMV4IDbg\IGIS_UtilityTool.obj"
".\ARMV4IDbg\IMemoryMngSet.obj"
".\ARMV4IDbg\MemoryDispatchForNetworkV2.obj"
".\ARMV4IDbg\MemoryDispatchForQueryData.obj"
".\ARMV4IDbg\MemoryDispatchForSpaceDataV2.obj"
".\ARMV4IDbg\MemoryMngSetV2.obj"
".\ARMV4IDbg\SDEControler.obj"
".\ARMV4IDbg\SDELib.obj"
".\ARMV4IDbg\StdAfx.obj"
".\ARMV4IDbg\Winarchive.obj"
".\ARMV4IDbg\adler32.obj"
".\ARMV4IDbg\compress.obj"
".\ARMV4IDbg\crc32.obj"
".\ARMV4IDbg\deflate.obj"
".\ARMV4IDbg\gzio.obj"
".\ARMV4IDbg\infback.obj"
".\ARMV4IDbg\inffast.obj"
".\ARMV4IDbg\inflate.obj"
".\ARMV4IDbg\inftrees.obj"
".\ARMV4IDbg\trees.obj"
".\ARMV4IDbg\uncompr.obj"
".\ARMV4IDbg\zutil.obj"
".\ARMV4IDbg\GIS_MBBNetwork.obj"
".\ARMV4IDbg\SDELib.res"
"\WorkStation-3.5\WorkStation-Asstitant\filedatastruct\ARMV4IDbg\FileDataStruct.lib"
"\WorkStation-3.5\WorkStation-Asstitant\DebugFunctionLib\ARMV4IDbg\DebugFunctionLib.lib"
"\WorkStation-3.5\WorkStation-Asstitant\MemoryMngLib\ARMV4IDbg\MemoryMngLib.lib"
]
Creating command line "link.exe @C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAA6.tmp"
<h3>Output Window</h3>
Compiling...
Command line warning D4002 : ignoring unknown option '/Gz'
GIS_MapBreadthNetwork.cpp
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(21) : warning C4005: 'DBG2SCR0' : macro redefinition
        e:\workstation-3.5\workstation-asstitant\debugfunctionlib\debugfun.h(14) : see previous definition of 'DBG2SCR0'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(22) : warning C4005: 'DBG2SCR1' : macro redefinition
        e:\workstation-3.5\workstation-asstitant\debugfunctionlib\debugfun.h(15) : see previous definition of 'DBG2SCR1'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(23) : warning C4005: 'DBG2SCR2' : macro redefinition
        e:\workstation-3.5\workstation-asstitant\debugfunctionlib\debugfun.h(16) : see previous definition of 'DBG2SCR2'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(24) : warning C4005: 'DBG2SCR3' : macro redefinition
        e:\workstation-3.5\workstation-asstitant\debugfunctionlib\debugfun.h(17) : see previous definition of 'DBG2SCR3'
¶þ¸ö¼¦µ° _AFX_OLD_EXCEPTIONS
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6024) : warning C4081: expected 'newline'; found ';'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(5934) : warning C4102: 'step1' : unreferenced label
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6166) : warning C4002: too many actual parameters for macro 'THROW_LAST'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6060) : warning C4102: 'step1' : unreferenced label
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6223) : warning C4102: 'step1' : unreferenced label
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6344) : warning C4102: 'step1' : unreferenced label
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6552) : warning C4002: too many actual parameters for macro 'THROW_LAST'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6467) : warning C4102: 'step1' : unreferenced label
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(6965) : warning C4002: too many actual parameters for macro 'THROW_LAST'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(7442) : warning C4102: 'step1' : unreferenced label
d:\program files\windows ce tools\wce420\v4iedk9101\include\armv4i\xstring(724) : warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
        d:\program files\windows ce tools\wce420\v4iedk9101\include\armv4i\xstring(720) : while compiling class-template member function 'void __cdecl std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::_Copy(unsigned int)'
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(7021) : warning C4715: 'CGIS_MapBreadthNetwork::LeaveResidentialArea' : not all control paths return a value
e:\workstation-3.5\workstation-asstitant\sde\gis_mapbreadthnetwork.cpp(7418) : warning C4715: 'CGIS_MapBreadthNetwork::CalculateNavigatePath_DoubleSourceSearch' : not all control paths return a value
Linking...
   Creating library ARMV4IDbg/SDELib.lib and object ARMV4IDbg/SDELib.exp
GeoView.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x0000E6E4
GIS_GeoMap.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x0003DAD4
GIS_MapBreadthNetwork.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x00057BCC
GIS_Middle.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x000823C4
GIS_QueryByShape.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x000A0B1C
GIS_MBBNetwork.obj : warning LNK1166: cannot adjust code at offset=0x00001000, rva=0x000E9114

<h3>
--------------------Configuration: SDETest - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating temporary file "C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAAC.tmp" with contents
[
/nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"wWinMainCRTStartup" /incremental:yes /pdb:"ARMV4IDbg/SDETest.pdb" /debug /out:"ARMV4IDbg/SDETest.exe" /subsystem:windowsce,4.20 /MACHINE:THUMB 
".\ARMV4IDbg\DlgCommonList.obj"
".\ARMV4IDbg\DlgTestFrame.obj"
".\ARMV4IDbg\EditDlg.obj"
".\ARMV4IDbg\MainControl.obj"
".\ARMV4IDbg\MainFrm.obj"
".\ARMV4IDbg\MapBar.obj"
".\ARMV4IDbg\NaviTestBar.obj"
".\ARMV4IDbg\QueryDlg.obj"
".\ARMV4IDbg\RouteMng.obj"
".\ARMV4IDbg\SDETest.obj"
".\ARMV4IDbg\SDETestDoc.obj"
".\ARMV4IDbg\SDETestView.obj"
".\ARMV4IDbg\StdAfx.obj"
".\ARMV4IDbg\CIniFile.obj"
".\ARMV4IDbg\CommandAction.obj"
".\ARMV4IDbg\ScriptAnalyzer.obj"
".\ARMV4IDbg\IGIS_Query2.obj"
".\ARMV4IDbg\QueryProxy.obj"
".\ARMV4IDbg\SDETest.res"
"\WorkStation-3.5\WorkStation-Asstitant\SDE\ARMV4IDbg\SDELib.lib"
]
Creating command line "link.exe @C:\DOCUME~1\lijun\LOCALS~1\Temp\RSPAAC.tmp"
<h3>Output Window</h3>
Linking...
   Creating library ARMV4IDbg/SDETest.lib and object ARMV4IDbg/SDETest.exp




<h3>Results</h3>
SDETest.exe - 0 error(s), 24 warning(s)
</pre>
</body>
</html>
