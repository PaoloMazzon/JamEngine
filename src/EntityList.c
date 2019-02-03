//
// Created by plo on 1/31/19.
//

#include "EntityList.h"

///////////////////////////////////////////////////////////////
EntityList* createEntityList() {
	EntityList* list = (EntityList*)calloc(1, sizeof(EntityList));

	if (list == NULL) {
		fprintf(stderr, "Failed to allocate entity list (createEntityList)\n");
	}

	return list;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void addEntityToList(EntityList* list, Entity* entity) {
	int i = 0;
	Entity** newList;
	bool foundSpot = false;
	if (list != NULL) {
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
			foundSpot = true;
			list->entities[list->size] = entity;
			list->size++;
		} else {
			// When all else fails, allocate more memory
			newList = realloc(list->entities, (list->size + ENTITY_LIST_ALLOCATION_AMOUNT) * sizeof(Entity*));

			if (newList != NULL) {
				list->entities = newList;
				list->size++;
				list->capacity += ENTITY_LIST_ALLOCATION_AMOUNT;
			} else {
				fprintf(stderr, "Could not reallocate entity list to accommodate for new entity (addEntityToList)\n");
			}
		}
	} else {
		fprintf(stderr, "List does not exist (addEntityToList)\n");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Entity* popEntityFromList(EntityList* list, int entityID) {
	Entity* search = NULL;
	int i = 0;
	if (list != NULL) {
		while (i < list->size && search == NULL) {
			if (list->entities[i]->entityID == entityID) {
				search = list->entities[i];
				list->entities[i] = NULL;
			}
			i++;
		}
	} else {
		fprintf(stderr, "List does not exist (popEntityInList)\n");
	}

	return search;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Entity* findEntityInList(EntityList* list, int entityID) {
	Entity* search = NULL;
	int i = 0;
	if (list != NULL) {
		while (i < list->size && search == NULL) {
			if (list->entities[i]->entityID == entityID)
				search = list->entities[i];
			i++;
		}
	} else {
		fprintf(stderr, "List does not exist (findEntityInList)\n");
	}

	return search;
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
void shrinkEntityList(EntityList* list) {
	int i, j, posInList;
	Entity** newList;
	if (list != NULL) {
		// Loop every element in size, we don't need to run through empty capacity
		for (i = 0; i < list->size; i++) {
			// We found an empty spot in the list
			if (list->entities[i] == NULL && i < list->size - 1) {
				// Find the last item in the list and put it at the current spot
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
		newList = realloc(list->entities, posInList * sizeof(Entity*));
		if (newList != NULL) {
			list->entities = newList;
			list->size = (uint32)posInList;
			list->capacity = (uint32)posInList;
		} else {
			fprintf(stderr, "Failed to reallocate entity list (shrinkEntityList)\n");
		}
	} else {
		fprintf(stderr, "List does not exist (shrinkEntityList)\n");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void emptyEntityList(EntityList* list, bool destroyEntities) {
	int i;

	if (list != NULL) {
		if (destroyEntities)
			for (i = 0; i < list->size; i++)
				freeEntity(list->entities[i], false, false, false);
		free(list->entities);
		list->entities = NULL;
		list->size = 0;
		list->capacity = 0;
	} else {
		fprintf(stderr, "List does not exist (emptyEntityList)\n");
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void freeEntityList(EntityList* list, bool destroyEntities) {
	if (list != NULL) {
		emptyEntityList(list, destroyEntities);
		free(list);
	}
}
///////////////////////////////////////////////////////////////