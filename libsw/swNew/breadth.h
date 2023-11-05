#ifndef _SW_BREADTH_H
#define _SW_BREADTH_H
#include "base.h"
/*
��ͼ�������
*/

struct swMapBreadthProps{
	 
	swMapBreadthIdentT	id;	
	swUInt32	offset;
	swUInt32	size;
	//swMapPersistContainerIdentT		contId;					//�洢����ı��	swMapPersistContainerIdentT
	//std::vector<swMapPersistContainerIdentT>  relconts;	//��صĴ洢����
};



class swMapBreadth{
public:
	swGeoRect					getGeoRect();
	swMapBreadthProps&	getProps();
	swMapBreadthProps		_props;
	std::list<swMapLayer*>	_layers;
};


#endif