//
// Created by lugi1 on 2018-07-05.
//

#include "Hitbox.h"
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <Vector.h>
#include <Hitbox.h>
#include "Hitbox.h"
#include "Vector.h"
#include "JamError.h"

/* The following functions mostly break the "don't use a return
 * statement anywhere but the last line rule" for the sake of
 * performance since collisions are generally quite heavy. Just
 * know that anytime a return happens in the middle of a function
 * it is strictly because the function knows for certain that either
 * a collision is or isn't taking place.
 */

// This is made automatically when you try to check a poly-to-rectangle
// collision. It stores a 4-sides polygon whos values are updated instead
// of making a new polygon every time a poly-rect collision is checked.
// It is freed whenever a hitbox is freed.
// tl;dr micro-optimizations
static JamPolygon* gRectPolyCache;

//////////////////////////////////////////////////
static bool _circRectColl(double cX, double cY, double cR, double rX, double rY, double rW, double rH) {
	// Is the circle's centre in the rectangle
	if (pointInRectangle(cX, cY, rX, rY, rW, rH))
		return true;

	// Check if the circle is touching a vertex of the rectangle
	if (pointDistance(cX, cY, rX, rY) < cR) return true;
	if (pointDistance(cX, cY, rX + rW, rY) < cR) return true;
	if (pointDistance(cX, cY, rX, rY + rH) < cR) return true;
	if (pointDistance(cX, cY, rX + rW, rY + rH) < cR) return true;

	// Now if the circle is along the edge
	if (pointInRectangle(cX, cY, rX - cR + 1, rY + 1, rH + cR * 2 - 2, rH - 2)) return true;
	if (pointInRectangle(cX, cY, rX + 1, rY - cR + 1, rH - 2, rH + cR * 2 - 2)) return true;
	return false;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
static inline double _cast1DShadow(double x, double y, double m) {
	return y - m * x;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
static bool _satCheckGap(JamPolygon* p1, JamPolygon* p2, double x1, double y1, double x2, double y2) {
	unsigned int i, j;
	double slope;
	double min1, min2, max1, max2;
	register bool done1;
	register bool done2;
	register double currentVal;

	// For each edge on polygon 1
	for (i = 0; i < p1->vertices; i++) {
		done1 = false;
		done2 = false;

		if (i == 0)
			slope = (p1->yVerts[0] - p1->yVerts[p1->vertices - 1]) / (p1->xVerts[0] - p1->xVerts[p1->vertices - 1]);
		else
			slope = (p1->yVerts[i] - p1->yVerts[i - 1]) / (p1->xVerts[i] - p1->xVerts[i - 1]);

		// Record min/max of polygon 1
		for (j = 0; j < p1->vertices; j++) {
			currentVal = _cast1DShadow(p1->xVerts[j] + x1, p1->yVerts[j] + y1, slope);
			if (currentVal > max1 || !done1)
				max1 = currentVal;
			if (currentVal < min1 || !done1)
				min1 = currentVal;
			done1 = true;
		}

		// Record min/max of polygon 2
		for (j = 0; j < p2->vertices; j++) {
			currentVal = _cast1DShadow(p2->xVerts[j] + x2, p2->yVerts[j] + y2, slope);
			if (currentVal > max2 || !done2)
				max2 = currentVal;
			if (currentVal < min2 || !done2)
				min2 = currentVal;
			done2 = true;
		}

		// Check if the two ranges don't intersect
		if (max1 < min2 || max2 < min1)
			return true;
	}

	return false;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool jamHitboxPolygonCollision(JamPolygon *poly1, JamPolygon *poly2, double x1, double y1, double x2, double y2) {
	// Make sure the polygons exist and they are at least a triangle
	if (poly1 != NULL && poly2 != NULL) {
		if (poly1->vertices >= 3 && poly2->vertices >= 3) {
			if (_satCheckGap(poly1, poly2, x1, y1, x2, y2) || _satCheckGap(poly2, poly1, x2, y2, x1, y1))
				return false;
			else
				return true;
		} else {
			if (poly1->vertices < 3)
				jSetError(ERROR_INCORRECT_FORMAT, "JamPolygon 1 needs at least 3 vertices.");
			if (poly2->vertices < 3)
				jSetError(ERROR_INCORRECT_FORMAT, "JamPolygon 2 needs at least 3 vertices.");

		}
	} else {
		if (poly1 == NULL)
			jSetError(ERROR_NULL_POINTER, "JamPolygon 1 does not exist.");
		if (poly2 == NULL)
			jSetError(ERROR_NULL_POINTER, "JamPolygon 2 does not exist.");
	}

	return false;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
static bool _satToCircleCollisions(JamPolygon* p1, double r, double x1, double y1, double x2, double y2) {
	return false;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
static bool _satToRectangleCollisions(JamPolygon* p, double w, double h, double x1, double y1, double x2, double y2) {
	if (gRectPolyCache == NULL) {
		gRectPolyCache = jamPolygonCreate(4);
		if (gRectPolyCache == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create polygon cache.");
			return false;
		}
	}

	// We now know for certain the polygon cache exists
	gRectPolyCache->xVerts[0] = 0;
	gRectPolyCache->yVerts[0] = 0;
	gRectPolyCache->xVerts[1] = w;
	gRectPolyCache->yVerts[1] = 0;
	gRectPolyCache->xVerts[2] = w;
	gRectPolyCache->yVerts[2] = h;
	gRectPolyCache->xVerts[3] = 0;
	gRectPolyCache->yVerts[3] = h;
	return jamHitboxPolygonCollision(gRectPolyCache, p, x2, y2, x1, y1);
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
JamHitbox* jamHitboxCreate(JamHitboxType type, double radius, double width, double height, JamPolygon *polygon) {
	JamHitbox* hitbox = (JamHitbox*)malloc(sizeof(JamHitbox));

	// Check if it worked of course
	if (hitbox != NULL) {
		hitbox->type = type;

		if (type == ht_Circle) {
			hitbox->radius = radius;
		} else if (type == ht_Rectangle) {
			hitbox->width = width;
			hitbox->height = height;
		} else if (type == ht_ConvexPolygon) {
			hitbox->polygon = polygon;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate hitbox. (jamHitboxCreate)");
	}

	return hitbox;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool jamHitboxCollision(JamHitbox *hitbox1, double x1, double y1, JamHitbox *hitbox2, double x2, double y2) {
	bool hit = false;

	// Double check it's there
	if (hitbox1 != NULL && hitbox2 != NULL) {
		if (hitbox1->type == ht_Circle && hitbox2->type == ht_Circle) {
			// Circle-to-circle
			hit = (pointDistance(x1, y1, x2, y2) < hitbox1->radius + hitbox2->radius);
		} else if (hitbox1->type == ht_Rectangle && hitbox2->type == ht_Rectangle) {
			// Rectangle-to-rectangle
			hit = (y1 + hitbox1->height > y2 && y1 < y2 + hitbox2->height && x1 + hitbox1->width > x2 && x1 < x2 + hitbox2->width);
		} else if (hitbox1->type == ht_Rectangle && hitbox2->type == ht_Circle) {
			// Rectangle-to-circle
			hit = _circRectColl(x2, y2, hitbox2->radius, x1, y1, hitbox1->width, hitbox1->height);
		} else if (hitbox1->type == ht_Circle && hitbox2->type == ht_Rectangle) {
			// Circle-to-rectangle
			hit = _circRectColl(x1, y1, hitbox1->radius, x2, y2, hitbox2->width, hitbox2->height);
		} else if (hitbox1->type == ht_ConvexPolygon && hitbox2->type == ht_ConvexPolygon) {
			// Poly-to-poly
			hit = jamHitboxPolygonCollision(hitbox1->polygon, hitbox2->polygon, x1, y1, x2, y2);
		} else if (hitbox1->type == ht_ConvexPolygon && hitbox2->type == ht_Rectangle) {
			// Poly-to-rectangle
			hit = _satToRectangleCollisions(hitbox1->polygon, hitbox2->width, hitbox2->height, x1, y1, x2, y2);
		} else if (hitbox1->type == ht_Rectangle && hitbox2->type == ht_ConvexPolygon) {
			// Rectangle-to-poly
			hit = _satToRectangleCollisions(hitbox2->polygon, hitbox1->width, hitbox2->height, x2, y2, x1, y1);
		} else if (hitbox1->type == ht_ConvexPolygon && hitbox2->type == ht_Circle) {
			// Poly-to-circle
			hit = _satToCircleCollisions(hitbox1->polygon, hitbox2->radius, x1, y1, x2, y2);
		} else if (hitbox1->type == ht_Circle && hitbox2->type == ht_ConvexPolygon) {
			// Circle-to-poly
			hit = _satToCircleCollisions(hitbox2->polygon, hitbox2->radius, x2, y2, x1, y1);
		} // TODO: Implement cirlce-to-poly collisions
	} else {
		if (hitbox1 == NULL)
			jSetError(ERROR_NULL_POINTER, "JamHitbox 1 does not exist. (jamHitboxCollision)");
		if (hitbox2 == NULL)
			jSetError(ERROR_NULL_POINTER, "JamHitbox 2 does not exist. (jamHitboxCollision)");
	}

	return hit;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void jamHitboxFree(JamHitbox *hitbox) {
	if (hitbox != NULL) {
		if (hitbox->type == ht_ConvexPolygon)
			jamPolygonFree(hitbox->polygon);
		free(hitbox);
	}

	if (gRectPolyCache != NULL) {
		jamPolygonFree(gRectPolyCache);
		gRectPolyCache = NULL;
	}
}
//////////////////////////////////////////////////
