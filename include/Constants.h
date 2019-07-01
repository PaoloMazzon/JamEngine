/// \file Constants.h
/// \author lugi1
/// \brief Just some constants

#pragma once
#include <stdbool.h>

// Unsigned numbers
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Signed numbers
typedef signed char sint8;
typedef signed short sint16;
typedef signed int sint32;
typedef signed long long sint64;

// Entity things
#define ENTITY_LIST_ALLOCATION_AMOUNT 5
#define ENTITIES_IN_RANGE 1
#define ENTITIES_OUT_OF_RANGE 0
#define MAX_ENTITY_TYPES 5

///< \brief Contains every type of thing an entity can be
typedef enum {logic = 0, solid = 1, npc = 2, object = 3, item = 4} EntityType;

/// \brief Used exclusive in worlds, sets up how to filter entities
typedef enum {fRectangle, fProximity} FilterType;

// This is used to give unique IDs to whatever
uint64 globalIDCounter;

// Optimization options - comment  out anything here to turn it off
#define ENTITY_ENABLE_Z
#define ENTITY_ENABLE_SPEED
#define ENTITY_ENABLE_FRICTION

// The names of INI headers for assets
#define INI_SPRITE_PREFIX 's'
#define INI_ENTITY_PREFIX 'e'
#define INI_HITBOX_PREFIX 'h'
#define INI_TEXTURE_PREFIX 't'
#define INI_TILEMAP_PREFIX 'm'