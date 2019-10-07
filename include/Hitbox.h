/// \file Hitbox.h
/// \author lugi1
/// \brief Fairly bare-bones at the moment, but has some basic functionality.

#pragma once
#include "Constants.h"
#include "Vector.h"

/// \brief A simple enum to making checking collisions easier
typedef enum {
	hitCircle,
	hitRectangle,
	hitConvexPolygon
} hitboxType;

/// \brief An all-encompassing hitbox
typedef struct {
	hitboxType type; ///< What type of hitbox this thing is
	union {
		double radius; ///< For circle collisions
		Polygon* polygon;
		struct { ;
			double width; ///< For rectangle collisions
			double height; ///< For rectangle collisions
		};
	};
} Hitbox;

/// \brief Creates a hitbox
///
/// If you pass a polygon to a hitbox, the polygon now belongs to the
/// hitbox (It will be freed by the hitbox).
///
/// \throws ERROR_ALLOC_FAILED
Hitbox* createHitbox(hitboxType type, double radius, double width, double height, Polygon* polygon);

/// \brief Checks for a collision between any two convex polygons
/// \throws ERROR_NULL_POINTER
bool checkConvexPolygonCollision(Polygon* poly1, Polygon* poly2, double x1, double y1, double x2, double y2);

/// \brief Checks for a collision between two hitboxes
/// \throws ERROR_NULL_POINTER
bool checkHitboxCollision(Hitbox* hitbox1, double x1, double y1, Hitbox* hitbox2, double x2, double y2);

/// \brief Clears a hitbox from memory
void freeHitbox(Hitbox* hitbox);