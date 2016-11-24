/**
 * Instruction: Definition of Plane Point  
 * 
 * Author: yao62995 <yao_62995@163.com>
 * Last Revise: 2016.10.19
 */

#ifndef TRAVELLER_DPOINT_H_
#define  TRAVELLER_DPOINT_H_

#include <math.h>

#define _PI_     3.14159265358979323846

class DPoint
{
public:
    DPoint();
    ~DPoint();
    DPoint(double posx, double posy);
    DPoint(double posx, double posy, int id);
    // copy construct
    DPoint(const DPoint& point);

    double x;
    double y;
    int id;

public:
    // copy operation
    DPoint & operator = (DPoint & point);
    // reduce operation
    DPoint operator - (DPoint & point);

    // angle to radians
    inline double ToRadians(double angle) {
        return angle * _PI_ / 180.0;
    }

    // compute distance, ref: http://tech.meituan.com/lucene-distance.html
    double Distance(DPoint& point);
};

#endif