//
// Created by lugi1 on 2018-07-05.
//

#include "Hitbox.h"
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <Hitbox.h>
#include "JamError.h"

// A function used only in this file, checks if a circle and rectangle are colliding
bool _circRectColl(double cX, double cY, double cR, double rX, double rY, double rW, double rH) {
	return false; // TODO: This
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
bool checkConvexPolygonCollision(Polygon* poly1, Polygon* poly2, double x1, double y1, double x2, double y2) {
	bool coll = false;
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