/// \file EntityList.h
/// \author plo
/// \date 1/31/19
/// \brief A struct for managing variable number of entities efficiently
#pragma once
#include "Entity.h"

/// \brief A fairly simple entity list
///
/// There are a couple of features in this struct to keep
/// it more efficient.
///
///  + Every time you add something to the struct, it checks
/// if there is already a free spot, and if not, it allocates
/// ENTITY_LIST_ALLOCATION_AMOUNT number of spots instead of
/// just 1. This is so memory isn't constantly getting bounced
/// around ram
///  + shrinkEntityList can be used to get rid of all free spots
/// in the list and deallocate any excess spots. This is useful
/// if you have a ton of entities coming an going and need the
/// list to come back to reality a bit.
///
/// It should also be noted that the entity list is not organized
/// and this struct's functions will not allow you to play around
/// with the organization. As such, you cannot pop, append, or
/// get specific positions from the list. If you need to find a
/// specific entity, use its internal ID, not array index since
/// this struct's internal list is incredibly volatile.
/// Furthermore, even though there is not an append function, you
/// can still add things by using addEntityToList.
///
/// If you want to loop this struct, use size; not capacity, and
/// ignore all NULL entities.
typedef struct {
	Entity** entities; ///< All the entities in the list
	uint32 size; ///< The size of the entity list
	uint32 capacity; ///< Total memory slots allocated
} EntityList;

/// \brief Creates an entity list
EntityList* createEntityList();

/// \brief Puts an entity into the list, making it bigger or possibly using a free spot
///
/// This function does not necessarily append things to the internal
/// list, it will always put the entity at the first free spot in the
/// list. If there is no free spots left, even on the end, it will
/// allocate ENTITY_LIST_ALLOCATION_AMOUNT new spots then put the entity
/// in one of those spots.
void addEntityToList(EntityList* list, Entity* entity);

/// \brief Removes an entity from the list and returns it
/// \return Returns NULL if it was not found
Entity* popEntityFromList(EntityList* list, int entityID);

/// \brief Looks for and returns an entity from the list
/// \return Returns NULL if it was not found
Entity* findEntityInList(EntityList* list, int entityID);

/// \brief Shrinks an entity list down to no bigger than needed
void shrinkEntityList(EntityList* list);

/// \brief Removes all entities from the list
void emptyEntityList(EntityList* list, bool destroyEntities);

/// \brief Destroys an entity list
void freeEntityList(EntityList* list, bool destroyEntities);