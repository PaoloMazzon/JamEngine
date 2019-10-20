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
#include <Sprite.h>
#include "BehaviourMap.h"

// The base key comparison function
bool keysEqual(JamAssetKey key1, JamAssetKey key2) {
	return strcmp(key1, key2) == 0;
}

///////////////////////////////////////////////////////////////
void jamLoadAssetIntoHandler(JamAssetHandler *handler, JamAsset *asset, JamAssetKey id) {
	int i;
	int exists = -1;
	JamAssetKey* newInts = NULL;
	JamAsset** newAssets = NULL;
	if (handler != NULL && asset != NULL) {
		// Look and check if the ID already exists
		for (i = 0; i < handler->size; i++)
			if (keysEqual(handler->ids[i], id))
				exists = i;

		// Either throw it in its spot or make a new spot
		if (exists != -1) {
			// First we must free the value there
			if (handler->vals[exists]->type == at_Texture)
				jamFreeTexture(handler->vals[exists]->tex);
			else if (handler->vals[exists]->type == at_Hitbox)
				jamFreeHitbox(handler->vals[exists]->hitbox);
			else if (handler->vals[exists]->type == at_Sprite)
				jamFreeSprite(handler->vals[exists]->spr, true, false);
			else if (handler->vals[exists]->type == at_Entity)
				jamFreeEntity(handler->vals[exists]->entity, false, false, false);
			else
				jamFreeTileMap(handler->vals[exists]->tileMap);

			// Finally, we set the new value
			handler->vals[exists] = asset;
		} else {
			// We must make room for a new piece
			newInts = (JamAssetKey*)realloc((void*)handler->ids, sizeof(JamAssetKey) * (handler->size + 1));
			newAssets = (JamAsset**)realloc((void*)handler->vals, sizeof(JamAsset*) * (handler->size + 1));

			if (newAssets != NULL && newInts != NULL) {
				// Set the new values and increment the size
				newInts[handler->size] = id;
				newAssets[handler->size] = asset;
				handler->ids = newInts;
				handler->vals = newAssets;
				handler->size++;
			} else {
				// Oh heck go back
				handler->ids = (JamAssetKey*)realloc((void*)handler->ids, sizeof(JamAssetKey) * (handler->size));
				handler->vals = (JamAsset**)realloc((void*)handler->vals, sizeof(JamAsset*) * (handler->size));
				jSetError(ERROR_REALLOC_FAILED, "Failed to increment handler size (jamLoadAssetIntoHandler with ID %s)\n", id);
			}
		}
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler does not exist (jamLoadAssetIntoHandler with ID %s)\n", id);
		}
		if (asset == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamAsset passed was null (jamLoadAssetIntoHandler with ID %s)\n", id);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAsset* createAsset(void* pointer, enum JamAssetType type) {
	JamAsset* asset = (JamAsset*)malloc(sizeof(JamAsset));

	if (asset != NULL && pointer != NULL) {
		asset->type = type;

		if (type == at_Texture)
			asset->tex = pointer;
		if (type == at_Sprite)
			asset->spr = pointer;
		if (type == at_Hitbox)
			asset->hitbox = pointer;
		if (type == at_Entity)
			asset->entity = pointer;
		if (type == at_TileMap)
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
JamAssetHandler* jamCreateAssetHandler() {
	JamAssetHandler* handler = (JamAssetHandler*)calloc(1, sizeof(JamAssetHandler));

	if (handler != NULL) {
		// no need to do anything here for now
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create an asset handler (jamCreateAssetHandler)\n");
	}

	return handler;
}
///////////////////////////////////////////////////////////////

//////////////////////// Functions that load individual pieces ////////////////////////
void assetLoadTileMap(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	jamLoadAssetIntoHandler(
			assetHandler,
			createAsset(jamLoadTileMap(
					jamGetKeyINI(ini, headerName, "file", ""),
					(uint32) atof(jamGetKeyINI(ini, headerName, "grid_width", "0")),
					(uint32) atof(jamGetKeyINI(ini, headerName, "grid_height", "0")),
					(uint32) atof(jamGetKeyINI(ini, headerName, "cell_width", "0")),
					(uint32) atof(jamGetKeyINI(ini, headerName, "cell_height", "0"))), at_TileMap),
			(headerName + 1)
	);
}

void assetLoadSprite(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	JamSprite* spr = jamLoadSpriteFromSheet(
			jamGetTextureFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "texture_id", "0"))),
			(uint32) atof(jamGetKeyINI(ini, headerName, "animation_length", "1")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "x_in_texture", "0")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "y_in_texture", "0")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "frame_width", "16")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "frame_height", "16")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "padding_width", "0")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "padding_height", "0")),
			(uint32) atof(jamGetKeyINI(ini, headerName, "x_align", "0")),
			(uint16) atof(jamGetKeyINI(ini, headerName, "frame_delay", "0")),
			(bool) atof(jamGetKeyINI(ini, headerName, "looping", "0")));
	if (spr != NULL) {
		spr->originX = (sint32) atof(jamGetKeyINI(ini, headerName, "x_origin", "0"));
		spr->originY = (sint32) atof(jamGetKeyINI(ini, headerName, "y_origin", "0"));
	}
	if (jamGetAssetFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "texture_id", "0"))) != NULL) {
		jamLoadAssetIntoHandler(assetHandler, createAsset(spr, at_Sprite), (headerName + 1)
		);
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load sprite of id %s, tex not found (jamAssetLoadINI)\\n", headerName + 1);
	}
}

void assetLoadEntity(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName, JamBehaviourMap* map) {
	JamEntity* ent;
	const char* typeString;
	// Make sure we have all necessary assets
	if (jamGetAssetFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "sprite_id", "0"))) != NULL
		&& jamGetAssetFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "hitbox_id", "0"))) != NULL) {
		ent = jamCreateEntity(
				jamGetAssetFromHandler(assetHandler,
									   (jamGetKeyINI(ini, headerName, "sprite_id", "0")))->spr,
				jamGetHitboxFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "hitbox_id", "0"))),
				(int) atof(jamGetKeyINI(ini, headerName, "x", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "y", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "hitbox_offset_x", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "hitbox_offset_y", "0")),
				jamGetBehaviourFromMap(map, jamGetKeyINI(ini, headerName, "behaviour", "default")));

		// Figure out the type
		typeString = jamGetKeyINI(ini, headerName, "type", "none");
		if (strcmp(typeString, "Logic") == 0)
			ent->type = et_Logic;
		if (strcmp(typeString, "Solid") == 0)
			ent->type = et_Solid;
		if (strcmp(typeString, "NPC") == 0)
			ent->type = et_NPC;
		if (strcmp(typeString, "Object") == 0)
			ent->type = et_Object;
		if (strcmp(typeString, "Item") == 0)
			ent->type = et_Item;
		if (strcmp(typeString, "Player") == 0)
			ent->type = et_Player;
		ent->rot = atof(jamGetKeyINI(ini, headerName, "rotation", "0"));
		ent->alpha = (uint8)atof(jamGetKeyINI(ini, headerName, "alpha", "255"));
		ent->updateOnDraw = (bool)atof(jamGetKeyINI(ini, headerName, "update_on_draw", "1"));
		jamLoadAssetIntoHandler(assetHandler, createAsset(ent, at_Entity), (headerName + 1));
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load entity of id %s (jamAssetLoadINI)\n", headerName + 1);
	}
}

void assetLoadHitbox(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	JamHitboxType hType = ht_Rectangle;
	const char* key = jamGetKeyINI(ini, headerName, "type", "rectangle");
	if (key == "rectangle") hType = ht_Rectangle;
	else if (key == "cirlce") hType = ht_Circle;
	else if (key == "polygon") hType = ht_ConvexPolygon;
	jamLoadAssetIntoHandler(
			assetHandler,
			createAsset(jamCreateHitbox(
					hType,
					atof(jamGetKeyINI(ini, headerName, "radius", "0")),
					atof(jamGetKeyINI(ini, headerName, "width", "0")),
					atof(jamGetKeyINI(ini, headerName, "height", "0")), NULL
			), at_Hitbox),
			(headerName + 1)
	);
}
//////////////////////// End of assetLoadINI support functions ////////////////////////

///////////////////////////////////////////////////////////////
void jamAssetLoadINI(JamAssetHandler *assetHandler, const char *filename, JamBehaviourMap *map) {
	JamINI* ini = jamLoadINI(filename);
	uint32 i, j;

	if (assetHandler != NULL && jamRendererGetInternalRenderer() != NULL && ini != NULL) {
		// Keep track of the ini (it holds the strings) and destroy it later
		assetHandler->localINI = ini;

		// Load the texture ids first
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") == 0) {
				for (j = 0; j < ini->headers[i]->size; j++)
					jamLoadAssetIntoHandler(
							assetHandler,
							createAsset(jamLoadTexture(ini->headers[i]->vals[j]), at_Texture),
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
			jSetError(ERROR_NULL_POINTER, "JamAsset loader does not exist for file %s (jamAssetLoadINI)\n", filename);
		}
		if (jamRendererGetInternalRenderer() == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist for file %s (jamAssetLoadINI)\n", filename);
		}
		if (ini == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to load JamINI for file %s (jamAssetLoadINI)\n", filename);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, JamAssetKey key) {
	int i;
	JamAsset* asset = NULL;
	if (assetHandler != NULL) {
		for (i = 0; i < assetHandler->size; i++)
			if (keysEqual(assetHandler->ids[i], key))
				asset = assetHandler->vals[i];
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetAssetFromHandler)\n");
	}

	return asset;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamSprite* jamGetSpriteFromHandler(JamAssetHandler *handler, JamAssetKey key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamSprite* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Sprite) {
			returnVal = asset->spr;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected sprite (jamGetSpriteFromHandler)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find sprite for key %s (jamGetSpriteFromHandler)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetSpriteFromHandler)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamEntity* jamGetEntityFromHandler(JamAssetHandler *handler, JamAssetKey key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamEntity* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Entity) {
			returnVal = asset->entity;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected entity (jamGetEntityFromHandler)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find entity for key %s (jamGetEntityFromHandler)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetEntityFromHandler)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamHitbox* jamGetHitboxFromHandler(JamAssetHandler *handler, JamAssetKey key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamHitbox* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Hitbox) {
			returnVal = asset->hitbox;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected hitbox (jamGetHitboxFromHandler)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find hitbox for key %s (jamGetHitboxFromHandler)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetHitboxFromHandler)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamTexture* jamGetTextureFromHandler(JamAssetHandler *handler, JamAssetKey key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamTexture* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Texture) {
			returnVal = asset->tex;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected texture (jamGetTextureFromHandler)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find texture for key %s (jamGetTextureFromHandler)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist(jamGetTextureFromHandler)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamTileMap* jamGetTileMapFromHandler(JamAssetHandler *handler, JamAssetKey key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamTileMap* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_TileMap) {
			returnVal = asset->tileMap;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected tileMap (jamGetTileMapFromHandler)\n", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find tileMap for key %s (jamGetTileMapFromHandler)\n", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist(jamGetTileMapFromHandler)\n");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAssetHandler* jamFreeAssetHandler(JamAssetHandler *handler) {
	int i;
	if (handler != NULL) {
		for (i = 0; i < handler->size; i++) {
			// Now dump whatever asset is here
			if (handler->vals[i]->type == at_Texture)
				jamFreeTexture(handler->vals[i]->tex);
			else if (handler->vals[i]->type == at_Sprite)
				jamFreeSprite(handler->vals[i]->spr, true, false);
			else if (handler->vals[i]->type == at_Hitbox)
				jamFreeHitbox(handler->vals[i]->hitbox);
			else if (handler->vals[i]->type == at_Entity)
				jamFreeEntity(handler->vals[i]->entity, false, false, false);
			else
				jamFreeTileMap(handler->vals[i]->tileMap);
			free(handler->vals[i]);
		}
		free(handler->vals);
		free(handler->ids);
		jamFreeINI(handler->localINI);
		free(handler);
	}
}
///////////////////////////////////////////////////////////////