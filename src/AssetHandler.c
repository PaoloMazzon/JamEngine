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
#include <JamEngine.h>
#include <Font.h>
#include "TMXWorldLoader.h"

static JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, const char* key);

///////////////////////////////////////////////////////////////
void jamAssetHandlerLoadAsset(JamAssetHandler *handler, JamAsset *asset, const char *id) {
	uint64 hash;
	JamAsset* current; // For crawling linked lists
	if (handler != NULL && asset != NULL) {
		hash = jamHashString(id, (uint64)handler->size);
		current = handler->vals[hash];

		// If it exists we stick it to that asset's linked list
		if (current != NULL) {
			while (current != NULL) {
				if (current->next != NULL) {
					current = current->next;
				} else {
					current->next = asset;
					current = NULL;
				}
			}
		} else {
			handler->vals[hash] = asset;
		}
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler does not exist (jamAssetHandlerLoadAsset with ID %s)", id);
		}
		if (asset == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamAsset passed was null (jamAssetHandlerLoadAsset with ID %s)", id);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
static JamAsset* createAsset(void* pointer, enum JamAssetType type, const char* key) {
	JamAsset* asset = (JamAsset*)malloc(sizeof(JamAsset));

	if (asset != NULL && pointer != NULL) {
		asset->type = type;
		asset->next = NULL;
		asset->name = malloc(strlen(key) + 1);

		// It's a union so it doesn't matter what type of asset this is, the pointer will end up
		// in the same place
		asset->tex = pointer;

		if (asset->name != NULL) {
			asset->name[strlen(key)] = 0;
			memcpy(asset->name, key, strlen(key));
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to copy asset name %s\n", key);
		}
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
JamAssetHandler* jamAssetHandlerCreate(int size) {
	JamAssetHandler* handler = (JamAssetHandler*)calloc(1, sizeof(JamAssetHandler));

	if (handler != NULL) {
		// Allocate the massive list
		handler->vals = (JamAsset**)calloc((size_t)size, sizeof(JamAsset*));
		handler->size = size;

		if (handler->vals == NULL) {
			free(handler);
			handler = NULL;
			jSetError(ERROR_ALLOC_FAILED, "Could not allocate asset list");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create an asset handler (jamAssetHandlerCreate)");
	}

	return handler;
}
///////////////////////////////////////////////////////////////

//////////////////////// Functions that load individual pieces ////////////////////////
static void _assetLoadSprite(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName) {
	JamSprite* spr = jamSpriteLoadFromSheet(
			jamAssetHandlerGetTexture(assetHandler, (jamINIGetKey(ini, headerName, "texture_id", "0"))),
			(uint32) atof(jamINIGetKey(ini, headerName, "animation_length", "1")),
			(uint32) atof(jamINIGetKey(ini, headerName, "x_in_texture", "0")),
			(uint32) atof(jamINIGetKey(ini, headerName, "y_in_texture", "0")),
			(uint32) atof(jamINIGetKey(ini, headerName, "frame_width", "16")),
			(uint32) atof(jamINIGetKey(ini, headerName, "frame_height", "16")),
			(uint32) atof(jamINIGetKey(ini, headerName, "padding_width", "0")),
			(uint32) atof(jamINIGetKey(ini, headerName, "padding_height", "0")),
			(uint32) atof(jamINIGetKey(ini, headerName, "x_align", "0")),
			(uint16) atof(jamINIGetKey(ini, headerName, "frame_delay", "0")),
			(bool) atof(jamINIGetKey(ini, headerName, "looping", "0")));
	if (spr != NULL) {
		spr->originX = (sint32) atof(jamINIGetKey(ini, headerName, "x_origin", "0"));
		spr->originY = (sint32) atof(jamINIGetKey(ini, headerName, "y_origin", "0"));
	}
	if (jamGetAssetFromHandler(assetHandler, (jamINIGetKey(ini, headerName, "texture_id", "0"))) != NULL) {
		jamAssetHandlerLoadAsset(assetHandler, createAsset(spr, at_Sprite, headerName + 1), (headerName + 1)
		);
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load sprite of id %s, tex not found (jamAssetHandlerLoadINI)\\n", headerName + 1);
	}
}

static void _assetLoadEntity(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName, JamBehaviourMap *map) {
	JamEntity* ent;
	const char* typeString;
	const char* behaviourString;
	// Make sure we have all necessary assets
	if (jamGetAssetFromHandler(assetHandler, (jamINIGetKey(ini, headerName, "sprite_id", "0"))) != NULL
		&& jamGetAssetFromHandler(assetHandler, (jamINIGetKey(ini, headerName, "hitbox_id", "0"))) != NULL) {
		behaviourString = jamINIGetKey(ini, headerName, "behaviour", "default");
		ent = jamEntityCreate(
				jamGetAssetFromHandler(assetHandler,
									   (jamINIGetKey(ini, headerName, "sprite_id", "0")))->spr,
				jamAssetHandlerGetHitbox(assetHandler, (jamINIGetKey(ini, headerName, "hitbox_id", "0"))),
				(int) atof(jamINIGetKey(ini, headerName, "x", "0")),
				(int) atof(jamINIGetKey(ini, headerName, "y", "0")),
				(int) atof(jamINIGetKey(ini, headerName, "hitbox_offset_x", "0")),
				(int) atof(jamINIGetKey(ini, headerName, "hitbox_offset_y", "0")),
				jamBehaviourMapGet(map, behaviourString));

		// Alert the user if a behaviour was expected but not found
		if (strcmp(behaviourString, "default") != 0 && ent->behaviour == NULL)
			jSetError(ERROR_WARNING, "Expected behaviour '%s' for entity '%s'", behaviourString, headerName);
		
		// Figure out the type
		typeString = jamINIGetKey(ini, headerName, "type", "none");
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
		ent->rot = atof(jamINIGetKey(ini, headerName, "rotation", "0"));
		ent->alpha = (uint8)atof(jamINIGetKey(ini, headerName, "alpha", "255"));
		ent->updateOnDraw = (bool)atof(jamINIGetKey(ini, headerName, "update_on_draw", "1"));
		jamAssetHandlerLoadAsset(assetHandler, createAsset(ent, at_Entity, headerName + 1), (headerName + 1));
	} else {
		jSetError(ERROR_ASSET_NOT_FOUND, "Failed to load entity of id %s (jamAssetHandlerLoadINI)", headerName + 1);
	}
}

static void _assetLoadHitbox(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName) {
	JamHitboxType hType = ht_Rectangle;
	const char* key = jamINIGetKey(ini, headerName, "type", "rectangle");
	if (strcmp(key, "rectangle") == 0) hType = ht_Rectangle;
	else if (strcmp(key, "cirlce") == 0) hType = ht_Circle;
	else if (strcmp(key, "polygon") == 0) hType = ht_ConvexPolygon;
	jamAssetHandlerLoadAsset(
			assetHandler,
			createAsset(jamHitboxCreate(
					hType,
					atof(jamINIGetKey(ini, headerName, "radius", "0")),
					atof(jamINIGetKey(ini, headerName, "width", "0")),
					atof(jamINIGetKey(ini, headerName, "height", "0")),
					jamPolygonLoad(jamINIGetKey(ini, headerName, "polygon", ""))
			), at_Hitbox, headerName + 1),
			(headerName + 1)
	);
}

static void _assetLoadAudio(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName) {
	jamAssetHandlerLoadAsset(
			assetHandler,
			createAsset(
					jamAudioLoadBuffer(jamINIGetKey(ini, headerName, "file", "")), at_AudioBuffer, headerName + 1),
			(headerName + 1));
}

static void _assetLoadWorld(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName) {
	// Load the world filtering setup and world
	JamWorld* world = jamTMXLoadWorld(assetHandler, jamINIGetKey(ini, headerName, "file", ""));
	uint16 width = (uint16)atof(jamINIGetKey(ini, headerName, "width", "0"));
	uint16 height = (uint16)atof(jamINIGetKey(ini, headerName, "height", "0"));
	uint16 radius = (uint16)atof(jamINIGetKey(ini, headerName, "radius", "0"));

	if (world != NULL) {
		if (width != 0) {
			jamWorldSetFilterTypeRectangle(world, width, height);
		} else if (radius != 0) {
			jamWorldSetFilterTypeCircle(world, radius);
		}
	}

	jamAssetHandlerLoadAsset(assetHandler, createAsset(world, at_World, headerName + 1), (headerName + 1));
}

static void _assetLoadFont(JamAssetHandler *assetHandler, JamINI *ini, const char *headerName) {
	JamFont* font = jamFontCreate(
			jamINIGetKey(ini, headerName, "latin", ""),
			jamINIGetKey(ini, headerName, "font", "0")
			);

	if (font != NULL) {
		font->characterHeight = (uint8) atof(jamINIGetKey(ini, headerName, "character_height", "0"));
		font->characterWidth  = (uint8) atof(jamINIGetKey(ini, headerName, "character_width", "0"));
		font->uStart          = (uint32)atof(jamINIGetKey(ini, headerName, "uni_range_start", "0"));
		font->uEnd            = (uint32)atof(jamINIGetKey(ini, headerName, "uni_range_end", "0"));
	}

	jamAssetHandlerLoadAsset(assetHandler, createAsset(font, at_Font, headerName + 1), (headerName + 1));
}
//////////////////////// End of assetLoadINI support functions ////////////////////////

///////////////////////////////////////////////////////////////
void jamAssetHandlerLoadINI(JamAssetHandler *assetHandler, const char *filename, JamBehaviourMap *map) {
	JamINI* ini = jamINILoad(filename);
	uint32 i, j;

	if (assetHandler != NULL && jamRendererGetInternalRenderer() != NULL && ini != NULL) {
		// Load the texture ids first
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") == 0) {
				for (j = 0; j < ini->headers[i]->size; j++)
					jamAssetHandlerLoadAsset(
							assetHandler,
							createAsset(jamTextureLoad(ini->headers[i]->vals[j]), at_Texture, ini->headers[i]->keys[j]),
							(ini->headers[i]->keys[j])
					);
			}
		}

		// Phase 1: Sprites, fonts, hitboxes, audio (zero dependency things)
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strlen(ini->headerNames[i]) > 0) {
				if (ini->headerNames[i][0] == INI_SPRITE_PREFIX) {
					_assetLoadSprite(assetHandler, ini, ini->headerNames[i]);
				} else if (ini->headerNames[i][0] == INI_HITBOX_PREFIX) {
					_assetLoadHitbox(assetHandler, ini, ini->headerNames[i]);
				} else if (ini->headerNames[i][0] == INI_AUDIO_PREFIX) {
					_assetLoadAudio(assetHandler, ini, ini->headerNames[i]);
				} else if (ini->headerNames[i][0] == INI_FONT_PREFIX) {
					_assetLoadFont(assetHandler, ini, ini->headerNames[i]);
				}
			}
		}

		// Phase 2: Entities
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_ENTITY_PREFIX) {
					_assetLoadEntity(assetHandler, ini, ini->headerNames[i], map);
				}
			}
		}

		// Phase 3: Worlds from .tmx files
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_WORLD_PREFIX) {
					_assetLoadWorld(assetHandler, ini, ini->headerNames[i]);
				}
			}
		}
	} else {
		if (assetHandler == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamAsset loader does not exist for file %s", filename);
		}
		if (jamRendererGetInternalRenderer() == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist for file %s", filename);
		}
		if (ini == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to load JamINI for file %s", filename);
		}
	}

	jamINIFree(ini);
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
static JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, const char* key) {
	JamAsset* asset = NULL;
	bool found = true;

	if (assetHandler != NULL) {
		// How this deals with hash collisions
		//   1. Find the asset at that hash
		//   2. Make sure it has the same name as that asset
		//   3. If it doesn't, run down its respective linked list and find the right one
		asset = assetHandler->vals[jamHashString(key, (uint64)assetHandler->size)];

		if (asset != NULL) {
			found = false;
		    while (!found && asset != NULL) {
				if (strcmp(asset->name, key) == 0) {
					found = true;
				} else {
					asset = asset->next;
				}
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	if (found)
		return asset;
	else
		return NULL;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamSprite* jamAssetHandlerGetSprite(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamSprite* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Sprite) {
			returnVal = asset->spr;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected sprite", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find sprite for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamEntity* jamAssetHandlerGetEntity(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamEntity* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Entity) {
			returnVal = asset->entity;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected entity", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find entity for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamHitbox* jamAssetHandlerGetHitbox(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamHitbox* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Hitbox) {
			returnVal = asset->hitbox;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected hitbox", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find hitbox for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamTexture* jamAssetHandlerGetTexture(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamTexture* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Texture) {
			returnVal = asset->tex;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected texture", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find texture for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamAudioBuffer* jamAssetHandlerGetAudioBuffer(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamAudioBuffer* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_AudioBuffer) {
			returnVal = asset->buffer;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected audio buffer", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find audio buffer for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamWorld* jamAssetHandlerGetWorld(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamWorld* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_World) {
			returnVal = asset->world;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected world", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find world for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
JamFont* jamAssetHandlerGetFont(JamAssetHandler *handler, const char *key) {
	JamAsset* asset = jamGetAssetFromHandler(handler, key);
	JamFont* returnVal = NULL;

	if (handler != NULL) {
		if (asset != NULL && asset->type == at_Font) {
			returnVal = asset->font;
		} else if (asset != NULL) {
			jSetError(ERROR_ASSET_WRONG_TYPE, "Incorrect asset type for key %s, expected world", key);
		} else {
			jSetError(ERROR_ASSET_NOT_FOUND, "Failed to find world for key %s", key);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamAssetHandler does not exist");
	}

	return returnVal;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
static void jamFreeAsset(JamAsset* asset) {
	if (asset != NULL) {
		// Now dump whatever asset is here
		if (asset->type == at_Texture)
			jamTextureFree(asset->tex);
		else if (asset->type == at_Sprite)
			jamSpriteFree(asset->spr, true, false);
		else if (asset->type == at_Hitbox)
			jamHitboxFree(asset->hitbox);
		else if (asset->type == at_Entity)
			jamEntityFree(asset->entity, false, false, false);
		else if (asset->type == at_World)
			jamWorldFree(asset->world);
		else if (asset->type == at_AudioBuffer)
			jamAudioFreeBuffer(asset->buffer);
		else if (asset->type == at_Font)
			jamFontFree(asset->font);
		free(asset->name);
		free(asset);
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void jamAssetHandlerFree(JamAssetHandler *handler) {
	int i;
	JamAsset* next, *old;
	if (handler != NULL) {
		for (i = 0; i < handler->size; i++) {
			if (handler->vals[i] != NULL) {
			    next = handler->vals[i]->next;
				jamFreeAsset(handler->vals[i]);
				while (next != NULL) {
				    old = next->next;
					jamFreeAsset(next);
					next = old;
				}
			}
		}
		free(handler->vals);
		free(handler);
	}
}
///////////////////////////////////////////////////////////////
