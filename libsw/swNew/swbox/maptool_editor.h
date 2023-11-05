#ifndef _MAPTOOL_EDITOR_H
#define _MAPTOOL_EDITOR_H

#include "map_tools.h"
//GeoMapTool_ObjectEditor
//地图对象编辑工具
//对于poi对象和barrier对象支持拖放和修改边界与位置
class GeoMapTool_ObjectEditor:public GeoMapToolBase{
public:
	GeoMapTool_ObjectEditor();
	void	set(GeoMapCanvas*);
	void  onMouseEvent(wxMouseEvent & evt);
	void onMapRedraw(const ViewRectT& rect=ViewRectT());
	void	setSelectTypes(unsigned int );			//设置可选择内容,多类型组合
	unsigned int getSelectTypes();
private:
	shared_ptr<ViewPointT>	_lastpt;
	shared_ptr<GeoMapSymbolBase> _cur_symbol;
	unsigned int selectTypes;
};

#endif
