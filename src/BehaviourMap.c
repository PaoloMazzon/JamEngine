#include "BehaviourMap.h"
#include "JamError.h"
#include "string.h"

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
		behaviour = (Behaviour*)malloc(sizeof(Behaviour));

		if (behaviour != NULL && newNames != NULL && newBehaviours != NULL) {
			map->behaviours = newBehaviours;
			map->names = newNames;
			map->behaviours[++map->size] = behaviour;
			map->names[map->size - 1] = name;

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
Behaviour* getBehaviourFromMap(BehaviourMap* map, const char* name) {
	Behaviour* returnBehaviour = NULL;
	int i;

	if (map != NULL) {
		for (i = 0; i < map->size && returnBehaviour == NULL; i++)
			if (strcmp(map->names[i], name) == 0)
				returnBehaviour = map->behaviours[i];
	}

	return returnBehaviour;
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