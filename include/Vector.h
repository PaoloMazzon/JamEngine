/// \file Vector.h
/// \author lugi1
/// \brief Some simple functions to make game development easier

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A polygon struct that is very to-the-point
typedef struct {
	double* xVerts; ///< X component of the vertices
	double* yVerts; ///< Y component of the vertices
	unsigned int vertices; ///< The total number of vertices in this polygon
} JamPolygon;

/// \brief Creates a polygon
///
/// \throws ERROR_ALLOC_FAILED
JamPolygon* jamCreatePolygon(unsigned int vertices);

/// \brief Adds a vertex to a polygon (appending it)
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_REALLOC_FAILED
void jamAddVertexToPolygon(JamPolygon *poly, double x, double y);

/// \brief Frees a polygon from memory
void jamFreePolygon(JamPolygon *poly);

/// \brief Checks the distance between two points
double pointDistance(double x1, double y1, double x2, double y2);

/// \brief Gets the angle between two points in radians
double pointAngle(double x1, double y1, double x2, double y2);

/// \brief Checks if a point is inside a rectangle
bool pointInRectangle(double px, double py, double rx, double ry, double rw, double rh);

/// \brief Checks if a point is in a circle
bool pointInCircle(double px, double py, double cx, double cy, double radius);

/// \brief Returns val but clamped between min and max (eg, clamp(10, 4, 8) would return 8)
double clamp(double val, double min, double max);

/// \brief Returns -1, 0, or 1 depending on the sign of the value
double sign(double val);
	
#ifdef __cplusplus
}
#endif
