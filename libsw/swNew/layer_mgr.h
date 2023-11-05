
#ifndef	_SW_LAYERSET_H
#define _SW_LAYERSET_H
#include "base.h"
#include "layer.h"
#include "breadth.h"
#include "render.h"



class swMapView;

//���ݲ���ʾ�������ԣ�ֻ�㼯�ɼ�ͼ��
typedef std::map< swUInt8,swMapLayerListT> swMapLayerChainT;//��ͬ�㼶�Ĳ��������

class swMapLayerDataSet{
public:
	enum eDataCachedType{
		CACHED = 0x1,
		UNCACHED =0x2,
		LAZY =0x4
	};
	swMapLayerDataSet(){}
	swMapLayerDataSet(swMapView *);	
	~swMapLayerDataSet();
//	shared_ptr<swRenderOutput> 		render();	 //�������;
	bool	assemble();		//����ͼ����Ϣ
//	bool	setDataCachedType(eDataCachedType dct);
//	bool	getLayers(std::vector<swMapLayer*>& layers,const swMapBreadthIdentT& breadId,const swMapLayerIdentT& layerId);

private:
	std::map<swMapBreadthIdentT,swMapLayerChainT>	 _breadthes;	//���������,ͬһ��������Կ����洢�ռ䣬�������ɲ㣬����Щ��Ҳ�Ǹ��������и�洢�ڲ�ͬ�Ĵ洢�ռ�
	swMapView *				_view;
	eDataCachedType		_dct;	//���ݼ��ط�ʽ
	std::vector<swMapLayer*>		_layers_cached;	//�����ͼ������
	/*�����ͼ�����ݽ���ָ��Щ����ȫ�����ڵ�ǰ���ӵ���Χ�ڵ�����ͼ������,�뵱ǰ���������ཻ������
	������Ϊ�������,��Ϊ��Щ�ཻ������ͼ����صĶ����ǲ�������
	�����������Ϊ���ڲ����������������Ӧ�÷�����view�����У������������ص�layer����������Ӧ�õĳ���ʱ���Կ���
	������map�������Ա���view����
	�����layer���������Ϊһ�鵥λ���壬����һ����������Ǵ洢�ڲ�ͬ�洢�ļ��Ķ���������
	*/
};

#endif
/*
1.����Ƕ��ʽ�ڴ���Դ�ѷ��Ŀ��� �ڻ�ͼʱ���Ȼ���ͬһ�㣬Ȼ��������һ������
������������£�map�˾Ͳ��ܻ�������Ĳ����ݣ�����ÿ����һ������������������ڵĴ洢�ļ���
Ȼ���ȡA�����ݻ��ƣ����Ŵ���һ��������A�����ݻ��ƣ����ѭ������B�����ݵĻ��ƣ�����������Ч��ʡ�ڴ�ռ�
�������������ܱȽϲ�����Ļ�_breadthes������layers����û�м���feature����ֻ�Ƕ�layer����Ϣ������ֻ����
������һ���ʱ��Ž��ж�ȡfeature����
2. assemble()
	�ڼ���ͼ������ʱ����3�д���ʽ������(cached)���ǻ���(uncached)��lazy��
	**cached:�ȵ�map�����layer�������ȥѰ���Լ�Ҫ����Ķ����Ƿ�
	���ڣ����ھͿ��ټ��أ���������ھʹ��ļ���ȥ��ȡ����ȡ֮����ݻ����㷨��ӵ�map�Ļ���layer����
	**uncached: ÿ�ζ�ȡlayerʱ���ȡ���е�feature���ݣ�����ǰ�󲻽���map�����ȡ�ʹ洢
	**lazy: ����ģʽ�� assemble����ֻ�Ǽ���layer��Ϣ����������feature����ļ��أ�ֻ�е�render�������ʱ
		��ȥ���ز����ݣ�ʹ�����֮�󼴿��ͷ�
	cached ��ȱʡģʽ
	lazy��ʽ�ʺ�Ƕ��ʽ�豸�ڴ���Դ���Ƶ����

*/

