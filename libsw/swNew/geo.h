
#ifndef _SW_GEO_H
#define _SW_GEO_H

#define sw_PI	3.1415926

#define sw_METERS_PER_MINUTE	(1860.0)
#define sw_METERS_PER_DEGREE	(sw_METERS_PER_MINUTE*60*1.0)
#define sw_METERS_PER_SECOND	(sw_METERS_PER_MINUTE/60*1.0)

#define SWMAP_GEOMTYPE_POINT 0
#define SWMAP_GEOMTYPE_LINE 1
#define SWMAP_GEOMTYPE_POLYGON 2

enum swGeometryTypeT{
	GEOM_POINT = 0,
	GEOM_MULTIPOINT,
	GEOM_LINESTRING,
	GEOM_MULTILINESTRING,
	GEOM_POLYGON,
	GEOM_MULTIPOLYGON
};

enum swFeatureRoadTypeT{
};

enum swFeaturePoiTypeT{

};

enum swFeaturePolygonTypeT{
	
};

#endif