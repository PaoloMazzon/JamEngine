//
// Created by lugi1 on 2018-07-18.
//

#include "Vector.h"
#include <math.h>
#include "JamError.h"
#include <malloc.h>
#include <Vector.h>

/*typedef struct {
	double* xVerts; ///< X component of the vertices
	double* yVerts; ///< Y component of the vertices
	unsigned int vertices; ///< The total number of vertices in this polygon
} Polygon;*/

//////////////////////////////////////////////////
Polygon* createPolygon(unsigned int vertices) {
	double* xVerts = (double*)malloc(sizeof(double) * vertices);
	double* yVerts = (double*)malloc(sizeof(double) * vertices);
	Polygon* poly = (Polygon*)malloc(sizeof(Polygon));

	if (poly != NULL && xVerts != NULL && yVerts != NULL) {
		poly->vertices = vertices;
		poly->xVerts = xVerts;
		poly->yVerts = yVerts;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Memory could not be allocated.");
		free(poly);
		free(xVerts);
		free(yVerts);
	}

	return poly;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void addVertexToPolygon(Polygon* poly, double x, double y) {
	double* xVerts;
	double* yVerts;

	if (poly != NULL) {
		xVerts = (double*)realloc(poly->xVerts, sizeof(double) * (poly->vertices + 1));
		yVerts = (double*)realloc(poly->yVerts, sizeof(double) * (poly->vertices + 1));

		if (xVerts != NULL && yVerts != NULL) {
			xVerts[poly->vertices++] = x;
			yVerts[poly->vertices] = y;
			poly->xVerts = xVerts;
			poly->yVerts = yVerts;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate vertex arrays.");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Polygon does not exist.");
	}
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void freePolygon(Polygon* poly) {
	if (poly != NULL) {
		free(poly->xVerts);
		free(poly->yVerts);
		free(poly);
	}
}
//////////////////////////////////////////////////

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

//////////////////////////////////////////////////
bool pointInCircle(double px, double py, double cx, double cy, double radius) {
	return pointDistance(px, py, cx, cy) <= radius;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
double clamp(double val, double min, double max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}
//////////////////////////////////////////////////