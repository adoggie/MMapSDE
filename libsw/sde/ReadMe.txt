========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY for Windows CE: SDELib
========================================================================


AppWizard has created this SDELib DLL for you.  This DLL not only
demonstrates the basics of using the Microsoft Foundation classes but
is also a starting point for writing your DLL.

This file contains a summary of what you will find in each of the files that
make up your SDELib DLL.

SDELib.h
	This is the main header file for the DLL.  It declares the
	CSDELibApp class.

SDELib.cpp
	This is the main DLL source file.  It contains the class CSDELibApp.


SDELib.rc
    This is a listing of all of the Microsoft Windows CE resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	eMbedded Visual C++.

res\SDELib.rc2
    This file contains resources that are not edited by Microsoft 
	eMbedded Visual C++.  You should place all resources not
	editable by the resource editor in this file.

SDELib.def
    This file contains information about the DLL that must be
    provided to run with Microsoft Windows CE.  It defines parameters
    such as the name and description of the DLL.  It also exports
	functions from the DLL.

SDELib.vcc
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named SDELib.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft eMbedded Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
预编译宏说明:
_ANALYSE_DATA_FILE
_ANALYSE_CALCULATE_PATH_PROCESS		分析拓扑计算过程.
_ANALYSE_PATH_TURN					分析机动计算过程.
_ANALYSE_FULLPATHDATA				分析填充过程
_ANALYSE_TURN_VOICE					分析语音提取过程

_DEBUG_VIEW_TURNLIST				显示出机动列表.
_DEBUG_VIEW_CALCULATE_PATH_PROCESS	显示出路径计算过程
_ENCRYPT_CHECK_ENABLE				是否需要加密校验