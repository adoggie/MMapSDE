#include "stdafx.h"
#include "Watch.h"

CWatch::CWatch(LPCTSTR strText)
{
	m_strText = strText;
	m_dwTickStart = GetTickCount();
}

CWatch::~CWatch()
{
	TRACE2("%s Use %d ms.\n",m_strText,GetTickCount() - m_dwTickStart);
}
