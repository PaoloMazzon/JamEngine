/// \file Hitbox.h
/// \author lugi1
/// \brief Fairly bare-bones at the moment, but has some basic functionality.

#pragma once
#include "Constants.h"
#include "Vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A simple enum to making checking collisions easier
typedef enum {
	ht_Circle,
	ht_Rectangle,
	ht_ConvexPolygon
} JamHitboxType;

/// \brief An all-encompassing hitbox
typedef struct {
	JamHitboxType type; ///< What type of hitbox this thing is
	union {
		double radius; ///< For circle collisions
		JamPolygon* polygon;
		struct { ;
			double width; ///< For rectangle collisions
			double height; ///< For rectangle collisions
		};
	};
} JamHitbox;

/// \brief Creates a hitbox
///
/// If you pass a polygon to a hitbox, the polygon now belongs to the
/// hitbox (It will be freed by the hitbox).
///
/// \throws ERROR_ALLOC_FAILED
JamHitbox* jamCreateHitbox(JamHitboxType type, double radius, double width, double height, JamPolygon *polygon);

/// \brief Checks for a collision between any two convex polygons
/// \throws ERROR_NULL_POINTER
bool jamCheckConvexPolygonCollision(JamPolygon *poly1, JamPolygon *poly2, double x1, double y1, double x2, double y2);

/// \brief Checks for a collision between two hitboxes
/// \throws ERROR_NULL_POINTER
bool jamCheckHitboxCollision(JamHitbox *hitbox1, double x1, double y1, JamHitbox *hitbox2, double x2, double y2);

/// \brief Clears a hitbox from memory
void jamFreeHitbox(JamHitbox *hitbox);

#ifdef __cplusplus
}
#endif