//
// Created by lugi1 on 2018-07-18.
//

#include "Vector.h"
#include <math.h>
#include "JamError.h"
#include <malloc.h>
#include <Vector.h>
#include <stdlib.h>
#include "StringUtil.h"
#include "File.h"
#include <string.h>

//////////////////////////////////////////////////
JamPolygon* jamCreatePolygon(unsigned int vertices) {
	JamPolygon* poly = (JamPolygon*)malloc(sizeof(JamPolygon));

	if (poly != NULL) {
		poly->xVerts = (double*)malloc(sizeof(double) * vertices);
		poly->yVerts = (double*)malloc(sizeof(double) * vertices);
		poly->vertices = vertices;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Memory could not be allocated.");
		free(poly);
	}

	return poly;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
JamPolygon* jamLoadPolygon(const char* string) {
	int i, j;
	JamStringList* vertices = jamExplodeString(string, '/', false);
	JamPolygon* poly = jamCreatePolygon(0);
	int commaLoc;
	
	if (vertices != NULL && strlen(string) > 0) {
		for (i = 0; i < vertices->size; i++) {
			// First locate the comma and replace it with a \0
			commaLoc = -1;
			for (j = 0; j < strlen(vertices->strList[i]) && commaLoc == -1; j++) {
				if (vertices->strList[i][j] == ',') {
					commaLoc = j;
					vertices->strList[i][j] = '\0';
				}
			}
			
			jamAddVertexToPolygon(poly, atof(vertices->strList[i]), atof(vertices->strList[i] + commaLoc + 1));
		}
	} else {
		if (vertices == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to split string \"%s\"", string);
		jamFreePolygon(poly);
	}

	jamFreeStringList(vertices);

	return poly;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void jamAddVertexToPolygon(JamPolygon *poly, double x, double y) {
	double* xVerts;
	double* yVerts;

	if (poly != NULL) {
		xVerts = (double*)realloc(poly->xVerts, sizeof(double) * (poly->vertices + 1));
		yVerts = (double*)realloc(poly->yVerts, sizeof(double) * (poly->vertices + 1));

		if (xVerts != NULL && yVerts != NULL) {
			xVerts[poly->vertices] = x;
			yVerts[poly->vertices] = y;
			poly->vertices++;
			poly->xVerts = xVerts;
			poly->yVerts = yVerts;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate vertex arrays.");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamPolygon does not exist.");
	}
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void jamFreePolygon(JamPolygon *poly) {
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

//////////////////////////////////////////////////
double sign(double val) {
	if (val < 0)
		return -1;
	else if (val > 0)
		return 1;
	else return 0;
}
//////////////////////////////////////////////////
