#ifndef _SW_MAP_H
#define _SW_MAP_H

#include "base.h"
#include "layer.h"
#include "view.h"
#include "persist.h"



/*
��ͼ�����������:

�����Ų���32λ���޷���������������ֻʹ��24λ
0xaabbbccc
bbb - ͼ���x��� 4k����λ
ccc - ͼ���y��� 4k����λ
aa  -
��ͼ�����ԭ���������½ǣ�������Y������������X������
һ��������ڶ����ͼ�����ļ��г���
*/

typedef swUInt16 swMapIdentT;	//��ͼ��ʶ 

struct sw_breadthsize_desc_t{
	swUInt8		index;
	swGeoSize	size;
	swUInt8		layernum;	//������
};
//��ͼ������Ϣ
struct swMapProps{
	swMapProps(){}
	swMapIdentT		id;				//��ͼ���
	swString		ver;
	swString		name;	
	//std::map<swMapPersistContainerIdentT,swMapPersistContainerProps>	containers;	//��������
	std::vector<swMapLayerProps>	layerProps; //n�㶨������
	swGeoRect		mbr;				//��ͼ������Χ
	std::vector<sw_breadthsize_desc_t> breadthsizes; //����������
	//swMapBreadthIdentT	orgBreadth;		//��׼�ο������� ���½��Ǹ�
	swGeoPoint		orgPoint;												//�ο��������ĵ�����
	swUInt8		compress;																//ѹ����ʽ
	std::vector<swMapLayerDrawPropT>	layerDrawProps;	//����ƿ�������  
	swFloat		 scaleUnits[SWMAP_SCALELEVEL_NUM];		//���ű���������
};

class swMapLayerDataSet;
class swMapView;

class swMap{
public:
	swMap();
	~swMap();
	bool	create(const swString& mapfile);			//������ͼ���󣬶�ȡ��ͼ��Ϣ
	swMapBreadthIdentT	getBreadthMap(const swGeoPoint& pt,swUInt8 sizeIdx=0);						//���ݾ�γ�Ȳ�ѯ�����ĸ�ͼ��
	
	bool getBreadthes(const swGeoRect& rc,swUInt8 sizeIdx,OUT std::vector<swMapBreadthIdentT>& ids); //��ȡͼ���б�,ids�ڵ����߶������˽������������𳬳�
	void render(swMapView* view,const swGeoRect grc,const swMapLayerIdentT& layerId); //��Ⱦ�������

	swMapProps&		getProps();
	bool	getBreadthRect(const swMapBreadthIdentT& bid,OUT swGeoRect&);		//�����ĵ�������
	swUInt8 getLayerNum();	//��ͼͼ������
	swMapLayerProps* getLayerProps(const swMapLayerIdentT& lid);  //��ȡͼ������
	swGeoSize	getBreadthSize(swUInt8 idx); //��ȡ�����С���
	swUInt8		getBreadthLayerNum(swUInt8 sizeIdx); //��ȡ���ֹ�������ͼ������
	swString getMapDirectory();				//��ȡ��ͼ���ڵ��ļ�Ŀ¼

private:
	swMapProps	_props;
	//swMapPersistContainerListT	_conts;  //�ļ��洢�����б�
	swMutex		_mtx_layers;
	std::map<swMapLayerStoreIdentT,swMapLayer*>		_layers_cached;	//�����ͼ������
	/*�����ͼ�����ݽ���ָ��Щ����ȫ�����ڵ�ǰ���ӵ���Χ�ڵ�����ͼ������,�뵱ǰ���������ཻ������
	������Ϊ�������,��Ϊ��Щ�ཻ������ͼ����صĶ����ǲ�������
	�����������Ϊ���ڲ����������������Ӧ�÷�����view�����У������������ص�layer����������Ӧ�õĳ���ʱ���Կ���
	������map�������Ա���view����

	*/
	std::list< shared_ptr<swRenderOutput> > _tiles_cached;				//�����������ݶ���(ͼ���ǿ��Խ��ж��ݵĻ���)
	swMutex		_mutex;
	swMapPersistContainerListT			_persists;	//�洢����
	swFile		_file;
	swString	_mapDir;
	
};


#endif