/**
 * Instruction: Implemention of Plane Point  
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */
 
#include "DPoint.h"

DPoint::DPoint(): x(0.0), y(0.0), id(-1){}

DPoint::~DPoint(){}

DPoint::DPoint(double posx, double posy): x(posx), y(posy), id(-1){}

DPoint::DPoint(double posx, double posy, int pid): x(posx), y(posy), id(pid){}


DPoint::DPoint(const DPoint& point)
{
    x = point.x;
    y = point.y;
    id = point.id;
}


DPoint & DPoint::operator = (DPoint & point)
{
    x = point.x;
    y = point.y;
    id = point.id;
    return point;
}

DPoint DPoint::operator - (DPoint & point)
{
    return DPoint(this->x - point.x, this->y - point.y);
}

// compute distance, ref: http://tech.meituan.com/lucene-distance.html
double DPoint::Distance(DPoint& point)
{
    // return sqrt((point.x - x) * (point.x - x) + (point.y - y) * (point.y - y));
    double lng_lc = ToRadians((point.y + y) / 2.0);
    // longitude distance
    // double lng = ToRadians(point.x - x) * 6367000.0 * cos(lng_lc);
    double lng = ToRadians(point.x - x) * cos(lng_lc);
    // latitude distance
    double lat = ToRadians(point.y - y);
    return sqrt(lng * lng + lat * lat);
}
