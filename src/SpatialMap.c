//
// Created by plo on 1/7/20.
//

#include "SpatialMap.h"

// Grabs the out-of-range list, does NOT check if the map is NULL
static inline JamEntityList* _getExtraneousList(JamSpatialMap* map) {
	return map->entityGrid[map->gridWidth * map->gridHeight];
}

/////////////////////////////////////////////////////
JamSpatialMap* jamSpatialMapCreate(uint32 gridWidth, uint32 gridHeight, uint32 cellWidth, uint32 cellHeight) {

}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void jamSpatialMapAdd(JamSpatialMap* spaceMap, JamEntity* entity) {

}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void jamSpatialMapFree(JamSpatialMap* spaceMap) {

}
/////////////////////////////////////////////////////