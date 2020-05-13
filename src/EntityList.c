//
// Created by plo on 1/31/19.
//

#include "EntityList.h"
#include "JamError.h"
#include <stdio.h>
#include <EntityList.h>
#include <malloc.h>
#include <memory.h>
#include <JamEngine.h>

///////////////////////////////////////////////////////////////
JamEntityList* jamEntityListCreate() {
	JamEntityList* list = (JamEntityList*)calloc(1, sizeof(JamEntityList));

	if (list == NULL) {
		jSetError(ERROR_NULL_POINTER, "Failed to allocate entity list (jamEntityListCreate)");
	}

	return list;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// This function is in most cases the most called function in all of JamEngine in real-world
// scenarios. This will be optimized again later, it must be as fast as humanly possibly.
int jamEntityListAdd(JamEntityList *list, JamEntity *entity) {
	register int i = 0;
	JamEntity** newList;
	register bool foundSpot = false;
	if (list != NULL && entity != NULL) {
		// First we look for a free spot in the list
		while (i < list->size && !foundSpot) {
			if (list->entities[i] == NULL) {
				foundSpot = true;
				list->entities[i] = entity;
			}
			i++;
		}

		// If not, we check for empty space at the end of list between size and capacity
		if (!foundSpot && list->size < list->capacity) {
			list->entities[list->size] = entity;
			list->size++;
		} else if (!foundSpot) {
			// When all else fails, allocate more memory
			newList = (JamEntity**)realloc(list->entities, (list->size + ENTITY_LIST_ALLOCATION_AMOUNT) * sizeof(JamEntity*));

			if (newList != NULL) {
				list->entities = newList;
				list->size++;
				list->capacity += ENTITY_LIST_ALLOCATION_AMOUNT;
				newList[list->size - 1] = entity;

				// Null all new cells
				for (i = list->size; i < list->capacity; i++)
					list->entities[i] = NULL;

				i = list->size - 1; // Record entity position in list
			} else {
				jSetError(ERROR_REALLOC_FAILED, "Could not reallocate entity list to accommodate for new entity (jamEntityListAdd)");
			}
		}
	} else if (list == NULL) {
		jSetError(ERROR_NULL_POINTER, "List does not exist (jamEntityListAdd)");
	}

	return (foundSpot) ? i - 1 : i;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamEntity* jamEntityListPop(JamEntityList *list, JamEntity *entity) {
	int i = 0;
	bool found = false;
	if (list != NULL) {
		while (i < list->size && found == false) {
			if (list->entities[i] == entity) {
				found = true;
				list->entities[i] = NULL;
			}
			i++;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "List does not exist");
	}

	return entity;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamEntity* jamEntityListCollision(int x, int y, JamEntity* entity, JamEntityList* list) {
	int i;
	JamEntity* output = NULL;
	if (list != NULL && entity != NULL) {
		for (i = 0; i < list->size && output == NULL; i++)
			if (list->entities[i] != NULL && jamEntityCheckCollision(x, y, entity, list->entities[i]))
				output = list->entities[i];
	} else {
		if (list == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity list does not exist");
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}

	return output;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
/*    Implementation
 * 1. Loop through the size of the list (not capacity)
 * 2. For each NULL pointer (free slot), perform another loop starting
 *    at that index + 1 that will find the last entity in the list and record it
 *  2a. If no entity is found after that index, record nothing and skip 3
 * 3. Move the recorded index's entity to the outer loop's empty index
 * 4. One the loop is done, loop until you hit a NULL, and resize the list to that size
 * 5. Adjust size and capacity accordingly
*/
void jamEntityListShrink(JamEntityList *list) {
	int i, j, posInList;
	JamEntity** newList;
	if (list != NULL) {
		// Loop every element in size, we don't need to run through empty capacity
		for (i = 0; i < list->size; i++) {
			// We found an empty spot in the list
			if (list->entities[i] == NULL && i < list->size - 1) {
				// Find the last et_Item in the list and put it at the current spot
				posInList = 0;
				for (j = i + 1; j < list->size; j++)
					if (list->entities[j] != NULL)
						posInList = j;
				list->entities[i] = list->entities[posInList];
				list->entities[posInList] = NULL;
			}
		}

		// Now that the list is all together, find the first NULL occurrence and that's the new size
		i = 0;
		posInList = -1;
		while (i < list->size && posInList == -1) {
			if (list->entities[i] == NULL)
				posInList = i;
			i++;
		}

		// Reset list size to that amount and make sure it works
		newList = realloc(list->entities, posInList * sizeof(JamEntity*));
		if (newList != NULL) {
			list->entities = newList;
			list->size = (uint32)posInList;
			list->capacity = (uint32)posInList;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate entity list (jamEntityListShrink)");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "List does not exist (jamEntityListShrink)");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamEntityListEmpty(JamEntityList *list, bool destroyEntities) {
	int i;

	if (list != NULL) {
		if (destroyEntities)
			for (i = 0; i < list->size; i++)
				jamEntityFree(list->entities[i], false, false, false);
		free(list->entities);
		list->entities = NULL;
		list->size = 0;
		list->capacity = 0;
	} else {
		jSetError(ERROR_NULL_POINTER, "List does not exist (jamEntityListEmpty)");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamEntityListFree(JamEntityList *list, bool destroyEntities) {
	if (list != NULL) {
		jamEntityListEmpty(list, destroyEntities);
		free(list);
	}
}
///////////////////////////////////////////////////////////////
