//  file:BlockID.cpp
#include "Stdafx.h"
#include "BlockID.h"

//////////////////////////////////////////////////////////////////////
//ID
BOOL IsValidDepth(long nDepth)
{
    return ( nDepth >= 0 && nDepth <= MAX_DEPTH);
}

BOOL IsValidID(long nID)
{
    int nDepth = GetDepth(nID);
    if ( !IsValidDepth(nDepth) )
        return FALSE;
    long nIDNoDepth = nID & 0x0FFFFFFF;
    if (nIDNoDepth >= pow(4.0, nDepth))
        return FALSE;
    return TRUE;
}

int GetDepth(long nID)
{
    return (unsigned)nID >> 28;
}

BOOL GetLeftID(long nID, long & nLeftID)
{
    int nDepth = GetDepth(nID);

    if ( !IsValidDepth(nDepth) )
        return FALSE;
    
    nLeftID = 0;
    int nMove = 0x00000001;
    int nOffset = 0;
    for (int i = 0; i < nDepth; i++)
    {
        if (nID & nMove)
        {
            nLeftID += nID - nMove + nOffset;
            return TRUE;
        }
        else
        {
            nOffset += nMove;
            nMove = nMove << 2;
        }
    }
    
    return FALSE;
}

BOOL GetRightID(long nID, long & nRightID)
{
    int nDepth = GetDepth(nID);

    if ( !IsValidDepth(nDepth) )
        return FALSE;
    
    nRightID = 0;
    long nMove = 0x00000001;
    int nOffset = 0;
    for (int i = 0; i < nDepth; i++)
    {
        if (nID & nMove)
        {
            nOffset += nMove;
            nMove = nMove << 2;
        }
        else
        {
            nRightID += nID + nMove - nOffset;
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL GetTopID(long nID, long & nTopID)
{
    int nDepth = GetDepth(nID);

    if ( !IsValidDepth(nDepth) )
        return FALSE;
    
    nTopID = 0;
    long nMove = 0x00000002;
    int nOffset = 0;
    for (int i = 0; i < nDepth; i++)
    {
        if (nID & nMove)
        {
            nTopID += nID - nMove + nOffset;
            return TRUE;
        }
        else
        {
            nOffset += nMove;
            nMove = nMove << 2;
        }
    }
    
    return FALSE;
}

BOOL GetBottomID(long nID, long & nBottomID)
{
    int nDepth = GetDepth(nID);

    if ( !IsValidDepth(nDepth) )
        return FALSE;
    
    nBottomID = 0;
    long nMove = 0x00000002;
    int nOffset = 0;
    for (int i = 0; i < nDepth; i++)
    {
        if (nID & nMove)
        {
            nOffset += nMove;
            nMove = nMove << 2;
        }
        else
        {
            nBottomID += nID + nMove - nOffset;
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL GetParentID(long nID, long & nParentID)
{
    int nDepth = GetDepth(nID);
    if ( !IsValidDepth(nDepth) )
        return FALSE;

    nDepth--;
    if ( !IsValidDepth(nDepth) )
        return FALSE;

    nID = 0x0FFFFFFF & nID;
    nParentID = nID >> 2;

    nDepth <<= 28;

    nParentID = nParentID | nDepth;

    return TRUE;
}


BOOL GetChildID(long nID, long & nChildID00)
{
    int nDepth = GetDepth(nID);
    if ( !IsValidDepth(nDepth) )
        return FALSE;

    nDepth++;
    if ( !IsValidDepth(nDepth) )
        return FALSE;

    nID = 0x0FFFFFFF & nID;
    
    nDepth <<= 28;

    nChildID00 = nDepth | (nID << 2);
    
    return TRUE;
}


BOOL GetChildID(long nID, long * pChildID)
{
    if (pChildID == NULL)
        return FALSE;

    if (!GetChildID(nID, pChildID[0]))
        return FALSE;

    pChildID[1] = pChildID[0] + 1;
    pChildID[2] = pChildID[0] + 2;
    pChildID[3] = pChildID[0] + 3;

    return TRUE;
}

BOOL GetChildID(long nID, 
                long &pChildID00, 
                long &pChildID01, 
                long &pChildID10, 
                long &pChildID11)
{
    if (!GetChildID(nID, pChildID00))
        return FALSE;

    pChildID01 = pChildID00 + 1;
    pChildID10 = pChildID00 + 2;
    pChildID11 = pChildID00 + 3;

    return TRUE;
}

void GetChildRect(LLRECT & llParentRect/*in*/,
				  BlockPos child/*in*/,
				  LLRECT & llRect/*out*/)
{
    LONLAT llMid;
    llMid.lon = llParentRect.left + llParentRect.Width() / 2;
    llMid.lat = llParentRect.bottom + llParentRect.Height() / 2;
	switch(child)
	{
	case LEFT_TOP:
		llRect.left   = llParentRect.left;
		llRect.top    = llParentRect.top;
		llRect.right  = llMid.lon;
		llRect.bottom = llMid.lat;
		break;
	case RIGHT_TOP:
		llRect.left   = llMid.lon;
		llRect.top    = llParentRect.top;
		llRect.right  = llParentRect.right;
		llRect.bottom = llMid.lat;
		break;
	case LEFT_BOTTOM:
		llRect.left   = llParentRect.left;
		llRect.top    = llMid.lat;
		llRect.right  = llMid.lon;
		llRect.bottom = llParentRect.bottom;
		break;
	case RIGHT_BOTTOM:
		llRect.left   = llMid.lon;
		llRect.top    = llMid.lat;
		llRect.right  = llParentRect.right;
		llRect.bottom = llParentRect.bottom;
		break;
	default:
		break;
	}
}


/*
*	id of llRootRect is 0x00000000
*/
BOOL GetRectByID(long nID/*in*/, 
                 LLRECT & llRootRect/*in*/,
                 LLRECT & llRect/*out*/)
{
	int nDepth = GetDepth(nID);

	LLRECT llParentRect;
	llParentRect = llRootRect;
	int nTmpID = nID & 0x0FFF;
	int nBlockPos, nOffset;
	for(int i = nDepth; i > 0; i--)
	{
		nOffset = (i - 1) << 1;
		nBlockPos = (nTmpID & (0x03 << nOffset)) >> nOffset;
		GetChildRect(llParentRect, (BlockPos)nBlockPos, llRect);
		llParentRect = llRect;
	}

    return TRUE;
}


BOOL GetIDByPos(LONLAT & llPos/*in*/, 
                LLRECT & llRootRect/*in*/,
                int nDepth/*in*/,
                unsigned long & nID/*out*/)
{
	if (!IsValidDepth(nDepth))
		return FALSE;

	unsigned int nPower = (unsigned int)pow(2.0, nDepth);
	double dLonSpan = llPos.lon - llRootRect.left;
	double dLatSpan = llRootRect.top - llPos.lat;
	double dBreadthLonSpan = llRootRect.Width() / nPower;
	double dBreadthLatSpan = llRootRect.Height() / nPower;

	unsigned int nTimesLon = (unsigned int)(dLonSpan / dBreadthLonSpan);
	unsigned int nTimesLat = (unsigned int)(dLatSpan / dBreadthLatSpan);
	if (nTimesLon == nPower)
		nTimesLon --;
	if (nTimesLat == nPower)
		nTimesLat --;

	nID = 0;
	unsigned long nTemp;
	for (int i = 0; i < nDepth; i++)
	{
		nTemp = 1 << (nDepth - i - 1);
		nID =   (nID << 2) + 
				( ((nTimesLat & nTemp) >> (nDepth - 1 - i)) << 1 ) + 
				( (nTimesLon & nTemp) >> (nDepth - 1 - i) );
	}

	nID = nID | (nDepth << 28);
	if (!IsValidID(nID))
	{
		return FALSE;
	}
	
    return TRUE;
}

