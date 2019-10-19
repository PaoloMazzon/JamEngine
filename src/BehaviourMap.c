#include "BehaviourMap.h"
#include "JamError.h"
#include "string.h"

/////////////////////////////////////////////////////////////////
JamBehaviourMap* jamCreateBehaviourMap() {
	JamBehaviourMap* map = (JamBehaviourMap*)calloc(1, sizeof(JamBehaviourMap));

	if (map == NULL)
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate behaviour map (jamCreateBehaviourMap)\n");

	return map;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void jamAddBehaviourToMap(JamBehaviourMap *map, const char *name, void (*onCreation)(BEHAVIOUR_ARGUMENTS),
						  void (*onDestruction)(BEHAVIOUR_ARGUMENTS), void (*onFrame)(BEHAVIOUR_ARGUMENTS),
						  void (*onDraw)(BEHAVIOUR_ARGUMENTS)) {
	JamBehaviour** newBehaviours;
	const char** newNames;
	JamBehaviour* behaviour;
	
	if (map != NULL) {
		newBehaviours = (JamBehaviour**)realloc(map->behaviours, (map->size + 1) * sizeof(JamBehaviour*));
		newNames = (const char**)realloc(map->names, (map->size + 1) * sizeof(const char*));
		behaviour = (JamBehaviour*)malloc(sizeof(JamBehaviour));

		if (behaviour != NULL && newNames != NULL && newBehaviours != NULL) {
			map->behaviours = newBehaviours;
			map->names = newNames;
			map->behaviours[map->size] = behaviour;
			map->names[map->size] = name;
			map->size++;

			behaviour->onCreation = onCreation;
			behaviour->onDestruction = onDestruction;
			behaviour->onFrame = onFrame;
			behaviour->onDraw = onDraw;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate map (jamAddBehaviourToMap)\n");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Map doesn't exist (jamAddBehaviourToMap)\n");
	}
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
JamBehaviour* jamGetBehaviourFromMap(JamBehaviourMap *map, const char *name) {
	JamBehaviour* returnBehaviour = NULL;
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
void jamFreeBehaviourMap(JamBehaviourMap *map) {
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