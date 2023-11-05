#ifndef _MAPTOOL_EDITOR_H
#define _MAPTOOL_EDITOR_H

#include "map_tools.h"
//GeoMapTool_ObjectEditor
//��ͼ����༭����
//����poi�����barrier����֧���Ϸź��޸ı߽���λ��
class GeoMapTool_ObjectEditor:public GeoMapToolBase{
public:
	GeoMapTool_ObjectEditor();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
	void onMapRedraw(const ViewRectT& rect=ViewRectT());
	void	setSelectTypes(unsigned int );			//���ÿ�ѡ������,���������
	unsigned int getSelectTypes();
private:
	shared_ptr<ViewPointT>	_lastpt;
	shared_ptr<GeoMapSymbolBase> _cur_symbol;
	unsigned int selectTypes;
};

#endif
