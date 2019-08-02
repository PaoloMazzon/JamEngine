//
// Created by lugi1 on 2018-07-05.
//

#include "Hitbox.h"
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include "Vector.h"
#include "JamError.h"

// A function used only in this file, checks if a circle and rectangle are colliding
bool _circRectColl(double cX, double cY, double cR, double rX, double rY, double rW, double rH) {
	bool coll = false;
	double x2 = rX + rW;
	double y2 = rY + rH;
	double pdtl;  // Point distance top left
	double pdtr;  // Point distance top right
	double pdbl;  // Point distance bottom left
	double pdbr;  // Point distance bottom right

	// The first check is if the circle is inside the rectangle
	if (!pointInRectangle(cX, cY, rX, rY, rW, rH)) {
		pdtl = pointDistance(cX, cY, rX, rY);
		pdtr = pointDistance(cX, cY, x2, rY);
		pdbl = pointDistance(cX, cY, rX, y2);
		pdbr = pointDistance(cX, cY, x2, y2);

		// Next check is if the circle is touching a corner of the rectangle
		if (pdtl < cR || pdtr < cR || pdbl < cR	|| pdbr < cR) {
			coll = true;
		} else {
			// Next check is the more advanced trig-based radius touching edge check
		}
	} else {
		coll = true;
	}

	return coll;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
Hitbox* createHitbox(hitboxType type, double radius, double width, double height) {
	Hitbox* hitbox = (Hitbox*)malloc(sizeof(Hitbox));

	// Check if it worked of course
	if (hitbox != NULL) {
		hitbox->type = type;
		hitbox->radius = radius;
		hitbox->width = width;
		hitbox->height = height;
	} else {
		fprintf(stderr, "Failed to allocate hitbox. (createHitbox)\n");
		jSetError(ERROR_ALLOC_FAILED);
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
		}
	} else {
		if (hitbox1 == NULL)
			fprintf(stderr, "Hitbox 1 does not exist. (checkHitboxCollision)\n");
		if (hitbox2 == NULL)
			fprintf(stderr, "Hitbox 2 does not exist. (checkHitboxCollision)\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return hit;
}
//////////////////////////////////////////////////

//////////////////////////////////////////////////
void freeHitbox(Hitbox* hitbox) {
	if (hitbox != NULL)
		free(hitbox);
}
//////////////////////////////////////////////////