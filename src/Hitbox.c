//
// Created by lugi1 on 2018-07-05.
//

#include "Hitbox.h"
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <Hitbox.h>
#include <Vector.h>
#include "JamError.h"

//////////////////////////////////////////////////
static bool _circRectColl(double cX, double cY, double cR, double rX, double rY, double rW, double rH) {
	return false; // TODO: This
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
static bool _satCheckPoly1Edges(Polygon* poly1, Polygon* poly2, double x1, double y1, double x2, double y2) {
	unsigned int i, j;
	double slope;
	double min1, min2, max1, max2;
	register unsigned char done1 = 0;
	register unsigned char done2 = 0;
	register double currentVal;

	// For each edge on polygon 1
	for (i = 0; i < poly1->vertices; i++) {
		if (i == 0)
			slope = (poly1->yVerts[0] - poly1->yVerts[poly1->vertices - 1]) / (poly1->xVerts[0] - poly1->xVerts[poly1->vertices - 1]);
		else
			slope = (poly1->yVerts[i] - poly1->yVerts[i - 1]) / (poly1->xVerts[i] - poly1->xVerts[i - 1]);

		// Record min/max of polygon 1
		for (j = 0; j < poly1->vertices; j++) {
			currentVal = poly1->yVerts[j] - (poly1->xVerts[j] * slope);
			if (currentVal > max1 || !done1)
				max1 = currentVal;
			if (currentVal < min1 || !done1)
				min1 = currentVal;
			done1++;
		}

		// Record min/max of polygon 2
		for (j = 0; j < poly2->vertices; j++) {
			currentVal = poly2->yVerts[j] - (poly2->xVerts[j] * slope);
			if (currentVal > max2 || !done2)
				max2 = currentVal;
			if (currentVal < min2 || !done2)
				min2 = currentVal;
			done2++;
		}

		// Check if the two ranges intersect
		if ((max1 >= min2 && min1 <= min2) || (max2 >= min1 && min2 <= min1))
			return true;
	}

	return false;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool checkConvexPolygonCollision(Polygon* poly1, Polygon* poly2, double x1, double y1, double x2, double y2) {
	bool coll = false;
	unsigned int i, j;
	double slope;

	// Make sure the polygons exist and they are at least a triangle
	if (poly1 != NULL && poly2 != NULL) {
		if (poly1->vertices >= 3 && poly2->vertices >= 3) {
			if (_satCheckPoly1Edges(poly1, poly2, x1, y1, x2, y2)) coll = true;
			if (_satCheckPoly1Edges(poly2, poly1, x1, y1, x2, y2)) coll = true;
		} else {
			if (poly1->vertices < 3)
				jSetError(ERROR_INCORRECT_FORMAT, "Polygon 1 needs at least 3 vertices.");
			if (poly2->vertices < 3)
				jSetError(ERROR_INCORRECT_FORMAT, "Polygon 2 needs at least 3 vertices.");

		}
	} else {
		if (poly1 == NULL)
			jSetError(ERROR_NULL_POINTER, "Polygon 1 does not exist.");
		if (poly2 == NULL)
			jSetError(ERROR_NULL_POINTER, "Polygon 2 does not exist.");
	}
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
Hitbox* createHitbox(hitboxType type, double radius, double width, double height, Polygon* polygon) {
	Hitbox* hitbox = (Hitbox*)malloc(sizeof(Hitbox));

	// Check if it worked of course
	if (hitbox != NULL) {
		hitbox->type = type;

		if (type == hitCircle) {
			hitbox->radius = radius;
		} else if (type == hitRectangle) {
			hitbox->width = width;
			hitbox->height = height;
		} else if (type == hitConvexPolygon) {
			hitbox->polygon = polygon;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate hitbox. (createHitbox)\n");
	}

	return hitbox;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool checkHitboxCollision(Hitbox* hitbox1, double x1, double y1, Hitbox* hitbox2, double x2, double y2) {
	bool hit = false;

	// Double check it's there
	if (hitbox1 != NULL && hitbox2 != NULL) {
		if (hitbox1->type == hitCircle && hitbox2->type == hitCircle) {
			// Circle-to-circle
			hit = (pointDistance(x1, y1, x2, y2) < hitbox1->radius + hitbox2->radius);
		} else if (hitbox1->type == hitRectangle && hitbox2->type == hitRectangle) {
			// Rectangle-to-rectangle
			hit = (y1 + hitbox1->height >= y2 && y1 <= y2 + hitbox2->height && x1 + hitbox1->width >= x2 && x1 <= x2 + hitbox2->width);
		} else if (hitbox1->type == hitRectangle && hitbox2->type == hitCircle) {
			// Rectangle-to-circle
			hit = _circRectColl(x2, y2, hitbox2->radius, x1, y1, hitbox1->width, hitbox1->height);
		} else if (hitbox1->type == hitCircle && hitbox2->type == hitRectangle) {
			// Circle-to-rectangle
			hit = _circRectColl(x1, y1, hitbox1->radius, x2, y2, hitbox2->width, hitbox2->height);
		} else if (hitbox1->type == hitConvexPolygon && hitbox2->type == hitConvexPolygon) {
			hit = checkConvexPolygonCollision(hitbox1->polygon, hitbox2->polygon, x1, y1, x2, y2);
		} // TODO: Add support for circle-poly and rect-poly collisions
	} else {
		if (hitbox1 == NULL)
			jSetError(ERROR_NULL_POINTER, "Hitbox 1 does not exist. (checkHitboxCollision)\n");
		if (hitbox2 == NULL)
			jSetError(ERROR_NULL_POINTER, "Hitbox 2 does not exist. (checkHitboxCollision)\n");
	}

	return hit;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void freeHitbox(Hitbox* hitbox) {
	if (hitbox != NULL) {
		if (hitbox->type == hitConvexPolygon)
			freePolygon(hitbox->polygon);
		free(hitbox);
	}
}
//////////////////////////////////////////////////