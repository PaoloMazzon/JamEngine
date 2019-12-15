/// \file Constants.h
/// \author lugi1
/// \brief Just some constants

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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

// JamWorld things
#define ENTITY_LIST_ALLOCATION_AMOUNT 5
#define ENTITIES_IN_RANGE 1
#define ENTITIES_OUT_OF_RANGE 0
#define MAX_ENTITY_TYPES 7
#define MAX_TILEMAPS 5

///< \brief Contains every type of thing an entity can be
///
/// Don't use none in an actual entity, its to denote the lack thereof
typedef enum {et_None = -1, et_Logic = 0, et_Solid = 1, et_NPC = 2, et_Object = 3, et_Item = 4, et_Player = 5, et_Unassigned = 6} JamEntityType;

///< Used exclusive in worlds, sets up how to filter entities
typedef enum {ft_Rectangle, ft_Proximity} JamFilterType;

///< The file that error messages will be output to
#define LOG_FILENAME "jamerrorlog.txt"

///< This is the maximum number vertices jamCheckMapCollision is allowed to check
#define MAX_GRID_VERTICES 16

///< Maximum number of cells to check when calling jamSnapEntityToTileMap* as to prevent an infinite loop
#define MAX_GRID_CHECKS 10
	
// The names of JamINI headers for assets
#define INI_SPRITE_PREFIX 's'
#define INI_ENTITY_PREFIX 'e'
#define INI_HITBOX_PREFIX 'h'
#define INI_TILEMAP_PREFIX 'm'
#define INI_AUDIO_PREFIX 'a'
#define INI_WORLD_PREFIX 'w'

#ifdef __cplusplus
}
#endif
