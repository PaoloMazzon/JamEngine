/// \file BehaviourMap.h
/// \author plo
/// \brief Makes managing large amounts of entities easier
#pragma once
#include "Renderer.h"

struct _World;
struct _Entity;

///< The arguments that must be present in every behaviour function
#define BEHAVIOUR_ARGUMENTS Renderer* renderer, struct _World* world, struct _Entity*

/// \brief A behaviour that holds a few functions that will be executed at specific times
///
/// For any of these functions you can simply use NULL and nothing will be executed. Do
/// note that if the onDraw function is not NULL, the entity will not be drawn by the world
/// at all and drawing the entity is now entirely on you.
typedef struct {
	void (*onCreation)(BEHAVIOUR_ARGUMENTS); ///< Will be executed when its added to a world using worldAddEntity
	void (*onDestruction)(BEHAVIOUR_ARGUMENTS); ///< Will be executed when this is freed from a world
	void (*onFrame)(BEHAVIOUR_ARGUMENTS); ///< Will be executed during each frame
	void (*onDraw)(BEHAVIOUR_ARGUMENTS); ///< Will be executed in place of normal world drawing functionality
} Behaviour;

/// \brief A dictionary of strings to behaviours
typedef struct {
	Behaviour** behaviours; ///< The behaviours
	const char** names; ///< The names of the behaviours
	uint32 size; ///< How many behaviours/names in this struct
} BehaviourMap;

/// \brief Creates a behaviour map
/// \throws ERROR_ALLOC_FAILED
BehaviourMap* createBehaviourMap();

/// \brief Adds a behaviour to a behaviour map
/// 
/// If the entity doesn't need a behaviour for onCreation, for example,
/// you can just leave it as NULL and nothing will be executed on creation.
/// 
/// \warning Strings passed to this function belong to the caller, not the map (It expects just in-code strings)
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_REALLOC_FAILED
void addBehaviourToMap(BehaviourMap* map, const char* name, void (*onCreation)(BEHAVIOUR_ARGUMENTS), void (*onDestruction)(BEHAVIOUR_ARGUMENTS), void (*onFrame)(BEHAVIOUR_ARGUMENTS), void (*onDraw)(BEHAVIOUR_ARGUMENTS));

/// \brief Grabs a behaviour struct (that still belongs to the map) from a map
/// 
/// Contrary to other functions like this, it will not set ERROR_NULL_POINTER
/// if you don't give it a map.
Behaviour* getBehaviourFromMap(BehaviourMap* map, const char* name);

/// \brief Frees a behaviour map
void freeBehaviourMap(BehaviourMap* map);