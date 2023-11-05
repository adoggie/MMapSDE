/** 
 * Copyright (c) 2007
 * @file    E:\WORKSTATION-3.5\WORKSTATION-ASSTITANT\BANCONVERTTOOLS\FILEDATASTRUCT\V1\SpaceFileStructV1.h
 * @brief
 * @version
 * @author
 * @date    2007/01/09
 * 
 * This file is under the source control of VSS
 * $History: $
 * $Revision: $
 * $Date: $
 * $NoKeywords: $ 
 */

#ifndef _SPACEFILESTRUCT_V1_H_
#define _SPACEFILESTRUCT_V1_H_

	
struct StuSpaceFileStructV1
{	


	typedef struct
	{
		char chSign[12];
		unsigned long nMapCount;
	}StuHeaderInfo;
	
	typedef struct
	{
		unsigned long nID;
		unsigned long nOffset;//地图在文件内的偏移
		unsigned long nBreadthCount;

        void DumpState(){};
        void AssertValid()
        {
            ASSERT( nID < 10);
            ASSERT( nOffset < 1024*1024*512);
            ASSERT( nBreadthCount < 99999);
        };

    }StuIndexInfoOfMap;
	
	
	typedef struct
	{
		unsigned long nID;
		unsigned long nOffset;//图幅类地图内的偏移
        
		unsigned char nLayerCount;
		unsigned char unuse0;
		unsigned char unuse1;
		unsigned char unuse2;

        void DumpState()
        {
            TRACE1("Breadth ID:%d\n",nID);
        };
        void AssertValid()
        {
            ASSERT( nID < 999999);
            ASSERT( nOffset < 1024*1024*512);
            ASSERT( nLayerCount < 25);
        };
		
	}StuIndexInfoOfBreadth;
	
	typedef struct
	{
		unsigned short nID;
		unsigned char nType;
		unsigned char nIconID;
		
		unsigned long nOffset;//图层在图幅内的偏移
		unsigned long nBeforeCompressSize;
		unsigned long nAfterCompressSize;
 
        void DumpState()
        {
            TRACE1("Layer ID:%d\n",nID);
        };	
        void AssertValid()
        {
            ASSERT( nID < 25);
            ASSERT( nType < 10);
            //ASSERT( nIconID < 0);
            ASSERT( nOffset < 1024*1024*512);
            ASSERT( nBeforeCompressSize < 1024*1024);
            ASSERT( nAfterCompressSize < 1024*1024);
        };
        
	}StuIndexInfoOfLayerInBreadthData;
	
	typedef struct
	{
		unsigned long nFeatureCount;
		unsigned long nCharDataBuddyLength;
		unsigned long nShortDataBuddyLength;
		unsigned long nDwordDataBuddyLength;
         
        void DumpState()
        {
            
        };	
        void AssertValid()
        {
            if(nFeatureCount)
            {
                ASSERT(nCharDataBuddyLength+nShortDataBuddyLength+nDwordDataBuddyLength);
            }
        };
	}StuHeaderInfoOfLayerInBreadthData;
	
	typedef struct
	{
		unsigned char nType;
		unsigned char nAnnoNum;
		unsigned char nAddressNum;
		unsigned char nPhoneNum;
		
		float fLog;
		float fLat;		

	}StuGisFeaturePoint;
	
	typedef struct
	{
		unsigned long nPointCount;
		unsigned short nID;
		unsigned char nDirection;
		unsigned char nAnnoNum;

	}StuGisFeatureRoadSection;
	
	typedef struct
	{
		unsigned long nPointCount;
		unsigned long nPartCount;
		
		unsigned char nAnnoNum;
		unsigned char unuse0;
		unsigned char unuse1;
		unsigned char unuse2;
        
	}StuGisFeatureLine;
	typedef struct
	{
		unsigned long nPointCount;
		unsigned long nPartCount;
		
		unsigned char nAnnoNum;
		unsigned char nShowClass;
		unsigned char unuse1;
		unsigned char unuse2;
        
	}StuGisFeaturePoly;
};

#endif //_SPACEFILESTRUCT_V1_H_