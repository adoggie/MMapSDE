//  file:BlockID.h

#ifndef _BLOCKID_H_
#define _BLOCKID_H_

#include "Geo.h"
//#include "SpaceTree.h"

//////////////////////////////////////////////////////////////////////////
/*
	_________________
	|		|		|
	|	10	|   11	|
	|		|		|
	|_______|_______|
	|   	|		|
	|	00	|	01	|
	|		|		|
	|_______|_______|
*/
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
//
#ifndef MAX_DEPTH
#define MAX_DEPTH			14
#endif

//////////////////////////////////////////////////////////////////////
//BlockPos
#ifndef _BLOCKPOS_
#define _BLOCKPOS_
enum BlockPos
{
    LEFT_TOP,
    RIGHT_TOP,
    LEFT_BOTTOM,
    RIGHT_BOTTOM
};
#endif

//////////////////////////////////////////////////////////////////////
//ID
BOOL IsValidDepth(long nDepth);
BOOL IsValidID(long nID);

int GetDepth(long nID);

BOOL GetParentID(long nID, long & nParentID);
BOOL GetChildID(long nID, long & nChildID00);
BOOL GetChildID(long nID, long * pChildID);
BOOL GetChildID(long nID, 
                long &pChildID00, 
                long &pChildID01, 
                long &pChildID10, 
                long &pChildID11);

BOOL GetBottomID(long nID, long & nBottomID);
BOOL GetTopID(long nID, long & nTopID);
BOOL GetRightID(long nID, long & nRightID);
BOOL GetLeftID(long nID, long & nLeftID);

BOOL GetRect00(LLRECT & llParentRect/*in*/,
				   LLRECT & llRect/*out*/);
BOOL GetRect01(LLRECT & llParentRect/*in*/,
				   LLRECT & llRect/*out*/);
BOOL GetRect10(LLRECT & llParentRect/*in*/,
				   LLRECT & llRect/*out*/);
BOOL GetRect11(LLRECT & llParentRect/*in*/,
				   LLRECT & llRect/*out*/);

BOOL GetRectByID(long nID/*in*/, 
                 LLRECT & llRootRect/*in*/,
                 LLRECT & llRect/*out*/);


BOOL GetIDByPos(LONLAT & llPos/*in*/, 
                LLRECT & llRootRect/*in*/,
                int nDepth/*in*/,
                unsigned long & nID/*out*/);


#endif  //_BLOCKID_H_