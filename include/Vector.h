//
// Created by lugi1 on 2018-07-18.
//

#pragma once
#include <stdbool.h>

/// \brief Checks the distance between two points
double pointDistance(double x1, double y1, double x2, double y2);

/// \brief Gets the angle between two points in radians
double pointAngle(double x1, double y1, double x2, double y2);

/// \brief Checks if a point is inside a rectangle
bool pointInRectangle(double px, double py, double rx, double ry, double rw, double rh);
