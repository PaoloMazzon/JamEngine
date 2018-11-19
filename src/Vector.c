//
// Created by lugi1 on 2018-07-18.
//

#include "Vector.h"
#include <math.h>

//////////////////////////////////////////////////
double pointDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
double pointAngle(double x1, double y1, double x2, double y2) {
	return atan((x2 - x1) / (y2 - y1));
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool pointInRectangle(double px, double py, double rx, double ry, double rw, double rh) {
	return (px >= rx && px <= rx + rw && py >= ry && py <= ry + rh);
}
//////////////////////////////////////////////////