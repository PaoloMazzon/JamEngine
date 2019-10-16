//
// Created by lugi1 on 2018-11-11.
//

#include "AssetHandler.h"
#include "INI.h"
#include <stdio.h>
#include <dirent.h>
#include <INI.h>
#include <StringMap.h>
#include <AssetHandler.h>
#include <StringUtil.h>
#include <JamError.h>
#include "BehaviourMap.h"

// The base key comparison function
bool keysEqual(AssetKey key1, AssetKey key2) {
	return strcmp(key1, key2) == 0;
}

///////////////////////////////////////////////////////////////
void loadAssetIntoHandler(AssetHandler* handler, Asset* asset, AssetKey id) {
	int i;
	int exists = -1;
	AssetKey* newInts = NULL;
	Asset** newAssets = NULL;
	if (handler != NULL && asset != NULL) {
		// Look and check if the ID already exists
		for (i = 0; i < handler->size; i++)
			if (keysEqual(handler->ids[i], id))
				exists = i;

		// Either throw it in its spot or make a new spot
		if (exists != -1) {
			// First we must free the value there
			if (handler->vals[exists]->type == texAsset)
				freeTexture(handler->vals[exists]->tex);
			else if (handler->vals[exists]->type == hitAsset)
				freeHitbox(handler->vals[exists]->hitbox);
			else if (handler->vals[exists]->type == sprAsset)
				freeSprite(handler->vals[exists]->spr, true, false);
			else if (handler->vals[exists]->type == entAsset)
				freeEntity(handler->vals[exists]->entity, false, false, false);
			else
				freeTileMap(handler->vals[exists]->tileMap);

			// Finally, we set the new value
			handler->vals[exists] = asset;
		} else {
			// We must make room for a new piece
			newInts = (AssetKey*)realloc((void*)handler->ids, sizeof(AssetKey) * (handler->size + 1));
			newAssets = (Asset**)realloc((void*)handler->vals, sizeof(Asset*) * (handler->size + 1));

			if (newAssets != NULL && newInts != NULL) {
				// Set the new values and increment the size
				newInts[handler->size] = id;
				newAssets[handler->size] = asset;
				handler->ids = newInts;
				handler->vals = newAssets;
				handler->size++;
			} else {
				// Oh heck go back
				handler->ids = (AssetKey*)realloc((void*)handler->ids, sizeof(AssetKey) * (handler->size));
				handler->vals = (Asset**)realloc((void*)handler->vals, sizeof(Asset*) * (handler->size));
				jSetError(ERROR_REALLOC_FAILED, "Failed to increment handler size (loadAssetIntoHandler with ID %s)\n", id);
			}
		}
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler does not exist (loadAssetIntoHandler with ID %s)\n", id);
		}
		if (asset == NULL) {
			jSetError(ERROR_NULL_POINTER, "Asset passed was null (loadAssetIntoHandler with ID %s)\n", id);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Asset* createAsset(void* pointer, enum AssetType type) {
	Asset* asset = (Asset*)malloc(sizeof(Asset));

	if (asset != NULL && pointer != NULL) {
		asset->type = type;

		if (type == texAsset)
			asset->tex = pointer;
		if (type == sprAsset)
			asset->spr = pointer;
		if (type == hitAsset)
			asset->hitbox = pointer;
		if (type == entAsset)
			asset->entity = pointer;
		if (type == tileAsset)
			asset->tileMap = pointer;
	} else {
		if (asset == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create asset (createAsset)\n");
		}
		if (pointer == NULL) {
			jSetError(ERROR_NULL_POINTER, "Pointer does not exist (createAsset)\n");
		}
		free(asset);
		asset = NULL;
	}

	return asset;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
AssetHandler* createAssetHandler() {
	AssetHandler* handler = (AssetHandler*)calloc(1, sizeof(AssetHandler));

	if (handler != NULL) {
		// no need to do anything here for now
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create an asset handler (createAssetHandler)\n");
	}

	return handler;
}
///////////////////////////////////////////////////////////////

//////////////////////// Functions that load individual pieces ////////////////////////
void assetLoadTileMap(AssetHandler* assetHandler, INI* ini, const char* headerName) {
	loadAssetIntoHandler(
			assetHandler,
			createAsset(loadTileMap(
					getKeyINI(ini, headerName, "file", ""),
					(uint32)atof(getKeyINI(ini, headerName, "grid_width", "0")),
					(uint32)atof(getKeyINI(ini, headerName, "grid_height", "0")),
					(uint32)atof(getKeyINI(ini, headerName, "cell_width", "0")),
					(uint32)atof(getKeyINI(ini, headerName, "cell_height", "0"))), tileAsset),
			(headerName + 1)
	);
}

void assetLoadSprite(AssetHandler* assetHandler, INI* ini, const char* headerName) {
	if (assetGet(assetHandler, (getKeyINI(ini, headerName, "texture_id", "0"))) != NULL) {
		loadAssetIntoHandler(
				assetHandler,
				createAsset(loadSpriteFromSheet(
						assetGet(assetHandler, (getKeyINI(ini, headerName, "texture_id", "0")))->tex,
						(uint32) atof(getKeyINI(ini, headerName, "animation_length", "1")),
						(uint32) atof(getKeyINI(ini, headerName, "x_in_texture", "0")),
						(uint32) atof(getKeyINI(ini, headerName, "y_in_texture", "0")),
						(uint32) atof(getKeyINI(ini, headerName, "frame_width", "16")),
						(uint32) atof(getKeyINI(ini, headerName, "frame_height", "16")),
						(uint32) atof(getKeyINI(ini, headerName, "padding_width", "0")),
						(uint32) atof(getKeyINI(ini, headerName, "padding_height", "0")),
						(uint32) atof(getKeyINI(ini, headerName, "x_align", "0")),
						(uint16) atof(getKeyINI(ini, headerName, "frame_delay", "0")),
						(bool) atof(getKeyINI(ini, headerName, "looping", "0"))), sprAsset),
				(headerName + 1)
		);
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load sprite of id %s, tex not found (assetLoadINI)\\n", headerName + 1);
	}
}

void assetLoadEntity(AssetHandler* assetHandler, INI* ini, const char* headerName, BehaviourMap* map) {
	Entity* ent;
	const char* typeString;
	// Make sure we have all necessary assets
	if (assetGet(assetHandler, (getKeyINI(ini, headerName, "sprite_id", "0"))) != NULL
		&& assetGet(assetHandler, (getKeyINI(ini, headerName, "hitbox_id", "0"))) != NULL) {
		ent = createEntity(
							assetGet(assetHandler,
							(getKeyINI(ini, headerName, "sprite_id", "0")))->spr,
							assetGetHitbox(assetHandler, (getKeyINI(ini, headerName, "hitbox_id", "0"))),
							(int) atof(getKeyINI(ini, headerName, "x", "0")),
							(int) atof(getKeyINI(ini, headerName, "y", "0")),
							(int) atof(getKeyINI(ini, headerName, "hitbox_offset_x", "0")),
							(int) atof(getKeyINI(ini, headerName, "hitbox_offset_y", "0")),
							getBehaviourFromMap(map, getKeyINI(ini, headerName, "behaviour", "default")));

		// Figure out the type
		typeString = getKeyINI(ini, headerName, "type", "none");
		if (strcmp(typeString, "Logic") == 0)
			ent->type = logic;
		if (strcmp(typeString, "Solid") == 0)
			ent->type = solid;
		if (strcmp(typeString, "NPC") == 0)
			ent->type = npc;
		if (strcmp(typeString, "Object") == 0)
			ent->type = object;
		if (strcmp(typeString, "Item") == 0)
			ent->type = item;
		if (strcmp(typeString, "Player") == 0)
			ent->type = player;
		ent->rot = atof(getKeyINI(ini, headerName, "rotation", "0"));
		ent->alpha = (uint8)atof(getKeyINI(ini, headerName, "alpha", "255"));
		ent->updateOnDraw = (bool)atof(getKeyINI(ini, headerName, "update_on_draw", "1"));
		loadAssetIntoHandler(assetHandler, createAsset(ent, entAsset), (headerName + 1));
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load entity of id %s (assetLoadINI)\n", headerName + 1);
	}
}

void assetLoadHitbox(AssetHandler* assetHandler, INI* ini, const char* headerName) {
	hitboxType hType = hitRectangle;
	const char* key = getKeyINI(ini, headerName, "type", "rectangle");
	if (key == "rectangle") hType = hitRectangle;
	else if (key == "cirlce") hType = hitCircle;
	else if (key == "polygon") hType = hitConvexPolygon;
	loadAssetIntoHandler(
			assetHandler,
			createAsset(createHitbox(
					hType,
					atof(getKeyINI(ini, headerName, "radius", "0")),
					atof(getKeyINI(ini, headerName, "width", "0")),
					atof(getKeyINI(ini, headerName, "height", "0")), NULL
			), hitAsset),
			(headerName + 1)
	);
}
//////////////////////// End of assetLoadINI support functions ////////////////////////

///////////////////////////////////////////////////////////////
void assetLoadINI(AssetHandler* assetHandler, Renderer* renderer, const char* filename, BehaviourMap* map) {
	INI* ini = loadINI(filename);
	uint32 i, j;

	if (assetHandler != NULL && renderer != NULL && ini != NULL) {
		// Keep track of the ini (it holds the strings) and destroy it later
		assetHandler->localINI = ini;

		// Load the texture ids first
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") == 0) {
				for (j = 0; j < ini->headers[i]->size; j++)
					loadAssetIntoHandler(
							assetHandler,
							createAsset(loadTexture(renderer, ini->headers[i]->vals[j]), texAsset),
							(ini->headers[i]->keys[j])
					);
			}
		}

		// Phase 1: Sprites and hitboxes
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strlen(ini->headerNames[i]) > 0) {
				if (ini->headerNames[i][0] == INI_SPRITE_PREFIX) {
					assetLoadSprite(assetHandler, ini, ini->headerNames[i]);
				} else if (ini->headerNames[i][0] == INI_HITBOX_PREFIX) {
					assetLoadHitbox(assetHandler, ini, ini->headerNames[i]);
				}
			}
		}

		// Phase 2: Everything else
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_TILEMAP_PREFIX) {
					assetLoadTileMap(assetHandler, ini, ini->headerNames[i]);
				} else if (ini->headerNames[i][0] == INI_ENTITY_PREFIX) {
					assetLoadEntity(assetHandler, ini, ini->headerNames[i], map);
				}
			}
		}
	} else {
		if (assetHandler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Asset loader does not exist for file %s (assetLoadINI)\n", filename);
		}
		if (renderer == NULL) {
			jSetError(ERROR_NULL_POINTER, "Renderer does not exist for file %s (assetLoadINI)\n", filename);
		}
		if (ini == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to load INI for file %s (assetLoadINI)\n", filename);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Asset* assetGet(AssetHandler* assetHandler, AssetKey key) {
	int i;
	Asset* asset = NULL;
	if (assetHandler != NULL) {
		for (i = 0; i < assetHandler->size; i++)
			if (keysEqual(assetHandler->ids[i], key))
				asset = assetHandler->vals[i];
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist (assetGet)\n");
	}

	return asset;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Sprite* assetGetSprite(AssetHandler* handler, AssetKey key) {
	Asset* asset = assetGet(handler, key);
	Sprite* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == sprAsset) {
			returnVal = asset->spr;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected sprite (assetGetSprite)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find sprite for key %s (assetGetSprite)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist (assetGetSprite)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Entity* assetGetEntity(AssetHandler* handler, AssetKey key) {
	Asset* asset = assetGet(handler, key);
	Entity* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == entAsset) {
			returnVal = asset->entity;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected entity (assetGetEntity)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find entity for key %s (assetGetEntity)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist (assetGetEntity)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Hitbox* assetGetHitbox(AssetHandler* handler, AssetKey key) {
	Asset* asset = assetGet(handler, key);
	Hitbox* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == hitAsset) {
			returnVal = asset->hitbox;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected hitbox (assetGetHitbox)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find hitbox for key %s (assetGetHitbox)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist (assetGetHitbox)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Texture* assetGetTexture(AssetHandler* handler, AssetKey key) {
	Asset* asset = assetGet(handler, key);
	Texture* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == texAsset) {
			returnVal = asset->tex;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected texture (assetGetTexture)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find texture for key %s (assetGetTexture)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist(assetGetTexture)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
TileMap* assetGetTileMap(AssetHandler* handler, AssetKey key) {
	Asset* asset = assetGet(handler, key);
	TileMap* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == tileAsset) {
			returnVal = asset->tileMap;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected tileMap (assetGetTileMap)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find tileMap for key %s (assetGetTileMap)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "AssetHandler does not exist(assetGetTileMap)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
AssetHandler* freeAssetHandler(AssetHandler* handler) {
	int i;
	if (handler != NULL) {
		for (i = 0; i < handler->size; i++) {
			// Now dump whatever asset is here
			if (handler->vals[i]->type == texAsset)
				freeTexture(handler->vals[i]->tex);
			else if (handler->vals[i]->type == sprAsset)
				freeSprite(handler->vals[i]->spr, true, false);
			else if (handler->vals[i]->type == hitAsset)
				freeHitbox(handler->vals[i]->hitbox);
			else if (handler->vals[i]->type == entAsset)
				freeEntity(handler->vals[i]->entity, false, false, false);
			else
				freeTileMap(handler->vals[i]->tileMap);
			free(handler->vals[i]);
		}
		free(handler->vals);
		free(handler->ids);
		freeINI(handler->localINI);
		free(handler);
	}
}
///////////////////////////////////////////////////////////////