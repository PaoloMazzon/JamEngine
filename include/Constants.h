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
#define MAX_TILEMAPS 5

///< The file that error messages will be output to
#define LOG_FILENAME "jamerrorlog.txt"

///< This is the maximum number vertices jamCheckMapCollision is allowed to check
#define MAX_GRID_VERTICES 16

///< Maximum number of cells to check when calling jamSnapEntityToTileMap* as to prevent an infinite loop
#define MAX_GRID_CHECKS 5

///< The ID of an entity not within a world
#define ID_NOT_ASSIGNED (-1)

///< Default bucket size of a control map for hashing
#define INPUT_BUCKET_SIZE 50
	
// The names of JamINI headers for assets
#define INI_SPRITE_PREFIX 's'
#define INI_ENTITY_PREFIX 'e'
#define INI_HITBOX_PREFIX 'h'
#define INI_TILEMAP_PREFIX 'm'
#define INI_AUDIO_PREFIX 'a'
#define INI_WORLD_PREFIX 'w'
#define INI_FONT_PREFIX 'f'
#define INI_BFONT_PREFIX 'b'

#ifdef __cplusplus
}
#endif
