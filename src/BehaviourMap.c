#include "BehaviourMap.h"
#include "JamError.h"

/*
typedef struct {
	void (*onCreation)(BEHAVIOUR_ARGUMENTS); ///< Will be executed when its added to a world using worldAddEntity
	void (*onDestruction)(BEHAVIOUR_ARGUMENTS); ///< Will be executed when this is freed from a world
	void (*onPreFrame)(BEHAVIOUR_ARGUMENTS); ///< Will be executed at the beginning each frame
	void (*onFrame)(BEHAVIOUR_ARGUMENTS); ///< Will be executed during each frame
	void (*onPostFrame)(BEHAVIOUR_ARGUMENTS); ///< Will be executed at the end of each frame
	void (*onDraw)(BEHAVIOUR_ARGUMENTS); ///< Will be executed in place of normal world drawing functionality
} Behaviour;

/// \brief A dictionary of strings to behaviours
typedef struct {
	Behaviour** behaviours; ///< The behaviours
	const char** names; ///< The names of the behaviours
	uint32 size; ///< How many behaviours/names in this struct
} BehaviourMap;
*/

/////////////////////////////////////////////////////////////////
BehaviourMap* createBehaviourMap() {
	BehaviourMap* map = (BehaviourMap*)calloc(1, sizeof(BehaviourMap));

	if (map == NULL) {
		jSetError(ERROR_ALLOC_FAILED);
		fprintf(stderr, "Failed to allocate behaviour map (createBehaviourMap)\n");
	}

	return map;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void addBehaviourToMap(BehaviourMap* map, const char* name, void (*onCreation)(BEHAVIOUR_ARGUMENTS), void (*onDestruction)(BEHAVIOUR_ARGUMENTS), void (*onPreFrame)(BEHAVIOUR_ARGUMENTS), void (*onFrame)(BEHAVIOUR_ARGUMENTS), void (*onPostFrame)(BEHAVIOUR_ARGUMENTS), void (*onDraw)(BEHAVIOUR_ARGUMENTS)) {
	Behaviour** newBehaviours;
	const char** newNames;
	Behaviour* behaviour;
	
	if (map != NULL) {
		newBehaviours = (Behaviour**)realloc(map->behaviours, (map->size + 1) * sizeof(Behaviour*));
		newNames = (const char**)realloc(map->names, (map->size + 1) * sizeof(const char*));
		behaviour = (Behaviour*)calloc(1, sizeof(Behaviour));

		if (behaviour != NULL && newNames != NULL && newBehaviours != NULL) {
			map->size++;
			map->behaviours = newBehaviours;
			map->names = newNames;
			map->behaviours[map->size - 1] = behaviour;

			behaviour->onCreation = onCreation;
			behaviour->onDestruction = onDestruction;
			behaviour->onPreFrame = onPreFrame;
			behaviour->onFrame = onFrame;
			behaviour->onPostFrame = onPostFrame;
			behaviour->onDraw = onDraw;
		} else {
			jSetError(ERROR_REALLOC_FAILED);
			fprintf(stderr, "Failed to reallocate map (addBehaviourToMap)\n");			
		}
	} else {
		jSetError(ERROR_NULL_POINTER);
		fprintf(stderr, "Map doesn't exist (addBehaviourToMap)\n");
	}
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void freeBehaviourMap(BehaviourMap* map) {
	int i;
	
	if (map != NULL) {
		for (i = 0; i < map->size; i++)
			free(map->behaviours[i]);
		free(map->behaviours);
		free(map->names);
		free(map);
	}
}
/////////////////////////////////////////////////////////////////