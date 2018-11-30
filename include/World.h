/// \file World.h
/// \author lugi1
/// \brief Declares the world struct and functions surrounding it

#pragma once
#include "TileMap.h"
#include "Entity.h"

/// \brief A thing that holds lots of info for convenience
typedef struct {
	TileMap* collisionGrid; ///< This is the struct that represents the collisions in this world
	Entity** entityList; ///< A list of entities in the world
} World;

/// \brief Creates a world to work with
World* createWorld();

/// \brief Frees a world
///
/// This free function does not handle
/// any data inside the world, you must
/// free that yourself.
void freeWorld(World* world);