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
#include <malloc.h>
#include <SDL.h>
#include "TMXWorldLoader.h"

///////////////////////////////////////////////////////////////
void jamLoadAssetIntoHandler(JamAssetHandler *handler, JamAsset *asset, const char* id) {
	int i;
	int hash = jamHashString(id, HANDLER_MAX_ASSETS);
	bool exists = false;
	if (handler != NULL && asset != NULL) {
		// Look and check if the ID already exists
		if (handler->vals[hash] != NULL)
			exists = true;

		// Either throw it in its spot or make a new spot
		if (exists) {
			// Warn the user in the case of a possible hashing collision
			jSetError(ERROR_WARNING, "Warning: Asset already exists for id %s, freeing old asset...");
			
			// First we must free the value there
			if (handler->vals[exists]->type == at_Texture)
				jamFreeTexture(handler->vals[exists]->tex);
			else if (handler->vals[exists]->type == at_Hitbox)
				jamFreeHitbox(handler->vals[exists]->hitbox);
			else if (handler->vals[exists]->type == at_Sprite)
				jamFreeSprite(handler->vals[exists]->spr, true, false);
			else if (handler->vals[exists]->type == at_Entity)
				jamFreeEntity(handler->vals[exists]->entity, false, false, false);
			else if (handler->vals[exists]->type == at_World)
				jamFreeWorld(handler->vals[exists]->world);
			else if (handler->vals[exists]->type == at_AudioBuffer)
				jamFreeAudioBuffer(handler->vals[exists]->buffer);
			jSetError(ERROR_WARNING, "Asset %s already exists, replacing old one.", id);
		}
		// Finally, we set the new value
		handler->vals[hash] = asset;
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler does not exist (jamLoadAssetIntoHandler with ID %s)", id);
		}
		if (asset == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamAsset passed was null (jamLoadAssetIntoHandler with ID %s)", id);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAsset* createAsset(void* pointer, enum JamAssetType type) {
	JamAsset* asset = (JamAsset*)malloc(sizeof(JamAsset));

	if (asset != NULL && pointer != NULL) {
		asset->type = type;

		// I know this isn't necessary, but it would be very confusing for those relatively new to c
		// In fact the union and all that is really just for clarity and could just as well be
		// a void* pointer to anything but that is not clear and being unclear is not what this engine's about
		// (also it would not be as easy to change things in JamAsset later should I decide to)
		if (type == at_Texture)
			asset->tex = pointer;
		if (type == at_Sprite)
			asset->spr = pointer;
		if (type == at_Hitbox)
			asset->hitbox = pointer;
		if (type == at_Entity)
			asset->entity = pointer;
		if (type == at_World)
			asset->world = pointer;
		if (type == at_AudioBuffer)
			asset->buffer = pointer;
	} else {
		if (asset == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create asset (createAsset)");
		}
		if (pointer == NULL) {
			jSetError(ERROR_NULL_POINTER, "Pointer does not exist (createAsset)");
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
		// Allocate the massive list
		handler->vals = (JamAsset**)calloc(HANDLER_MAX_ASSETS, sizeof(JamAsset*));

		if (handler->vals == NULL) {
			free(handler);
			handler = NULL;
			jSetError(ERROR_ALLOC_FAILED, "Could not allocate asset list");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create an asset handler (jamCreateAssetHandler)");
	}

	return handler;
}
///////////////////////////////////////////////////////////////

//////////////////////// Functions that load individual pieces ////////////////////////
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
	const char* behaviourString;
	// Make sure we have all necessary assets
	if (jamGetAssetFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "sprite_id", "0"))) != NULL
		&& jamGetAssetFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "hitbox_id", "0"))) != NULL) {
		behaviourString = jamGetKeyINI(ini, headerName, "behaviour", "default");
		ent = jamCreateEntity(
				jamGetAssetFromHandler(assetHandler,
									   (jamGetKeyINI(ini, headerName, "sprite_id", "0")))->spr,
				jamGetHitboxFromHandler(assetHandler, (jamGetKeyINI(ini, headerName, "hitbox_id", "0"))),
				(int) atof(jamGetKeyINI(ini, headerName, "x", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "y", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "hitbox_offset_x", "0")),
				(int) atof(jamGetKeyINI(ini, headerName, "hitbox_offset_y", "0")),
				jamGetBehaviourFromMap(map, behaviourString));

		// Alert the user if a behaviour was expected but not found
		if (strcmp(behaviourString, "default") != 0 && ent->behaviour == NULL)
			jSetError(ERROR_WARNING, "Expected behaviour '%s' for entity '%s'", behaviourString, headerName);
		
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
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load entity of id %s (jamAssetLoadINI)", headerName + 1);
	}
}

void assetLoadHitbox(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	JamHitboxType hType = ht_Rectangle;
	const char* key = jamGetKeyINI(ini, headerName, "type", "rectangle");
	if (strcmp(key, "rectangle") == 0) hType = ht_Rectangle;
	else if (strcmp(key, "cirlce") == 0) hType = ht_Circle;
	else if (strcmp(key, "polygon") == 0) hType = ht_ConvexPolygon;
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

void assetLoadAudio(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	jamLoadAssetIntoHandler(
			assetHandler,
			createAsset(
					jamLoadAudioBuffer(jamGetKeyINI(ini, headerName, "file", ""))
					, at_AudioBuffer)
			, (headerName + 1));
}

void assetLoadWorld(JamAssetHandler* assetHandler, JamINI* ini, const char* headerName) {
	// Load the world filtering setup and world
	JamWorld* world = jamLoadWorldFromTMX(assetHandler, jamGetKeyINI(ini, headerName, "file", ""));
	uint16 width = (uint16)atof(jamGetKeyINI(ini, headerName, "width", "0"));
	uint16 height = (uint16)atof(jamGetKeyINI(ini, headerName, "height", "0"));
	uint16 radius = (uint16)atof(jamGetKeyINI(ini, headerName, "radius", "0"));

	if (world != NULL) {
		if (width != 0) {
			jamSetWorldFilterTypeRectangle(world, width, height);
		} else if (radius != 0) {
			jamSetWorldFilterTypeCircle(world, radius);
		}
	}

	jamLoadAssetIntoHandler(assetHandler, createAsset(world, at_World), (headerName + 1));
}
//////////////////////// End of assetLoadINI support functions ////////////////////////

///////////////////////////////////////////////////////////////
void jamAssetLoadINI(JamAssetHandler *assetHandler, const char *filename, JamBehaviourMap *map) {
	JamINI* ini = jamLoadINI(filename);
	uint32 i, j;

	if (assetHandler != NULL && jamRendererGetInternalRenderer() != NULL && ini != NULL) {
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
				} else if (ini->headerNames[i][0] == INI_AUDIO_PREFIX) {
					assetLoadAudio(assetHandler, ini, ini->headerNames[i]);
				}
			}
		}

		// Phase 2: Entities
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_ENTITY_PREFIX) {
					assetLoadEntity(assetHandler, ini, ini->headerNames[i], map);
				}
			}
		}

		// Phase 3: Worlds from .tmx files
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_WORLD_PREFIX) {
					assetLoadWorld(assetHandler, ini, ini->headerNames[i]);
				}
			}
		}
	} else {
		if (assetHandler == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamAsset loader does not exist for file %s (jamAssetLoadINI)", filename);
		}
		if (jamRendererGetInternalRenderer() == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist for file %s (jamAssetLoadINI)", filename);
		}
		if (ini == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to load JamINI for file %s (jamAssetLoadINI)", filename);
		}
	}

	jamFreeINI(ini);
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, const char* key) {
	int i;
	JamAsset* asset = NULL;
	if (assetHandler != NULL) {
		asset = assetHandler->vals[jamHashString(key, HANDLER_MAX_ASSETS)];
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetAssetFromHandler)");
	}

	return asset;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamSprite* jamGetSpriteFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamSprite* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Sprite) {
			returnVal = asset->spr;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected sprite (jamGetSpriteFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find sprite for key %s (jamGetSpriteFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetSpriteFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamEntity* jamGetEntityFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamEntity* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Entity) {
			returnVal = asset->entity;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected entity (jamGetEntityFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find entity for key %s (jamGetEntityFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetEntityFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamHitbox* jamGetHitboxFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamHitbox* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Hitbox) {
			returnVal = asset->hitbox;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected hitbox (jamGetHitboxFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find hitbox for key %s (jamGetHitboxFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist (jamGetHitboxFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamTexture* jamGetTextureFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamTexture* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Texture) {
			returnVal = asset->tex;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected texture (jamGetTextureFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find texture for key %s (jamGetTextureFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist(jamGetTextureFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAudioBuffer* jamGetAudioBufferFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamAudioBuffer* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_AudioBuffer) {
			returnVal = asset->buffer;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected audio buffer (jamGetTileMapFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find audio buffer for key %s (jamGetTileMapFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist(jamGetTileMapFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamWorld* jamGetWorldFromHandler(JamAssetHandler *handler, const char* key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamWorld* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_World) {
			returnVal = asset->world;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected world (jamGetTileMapFromHandler)", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find world for key %s (jamGetTileMapFromHandler)", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist(jamGetTileMapFromHandler)");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamFreeAssetHandler(JamAssetHandler *handler) {
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
			else if (handler->vals[i]->type == at_World)
				jamFreeWorld(handler->vals[i]->world);
			else if (handler->vals[i]->type == at_AudioBuffer)
				jamFreeAudioBuffer(handler->vals[i]->buffer);
			free(handler->vals[i]);
		}
		free(handler->vals);
		free(handler);
	}
}
///////////////////////////////////////////////////////////////
