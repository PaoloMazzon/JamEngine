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

///////////////////////////////////////////////////////////////
void loadAssetIntoHandler(AssetHandler* handler, Asset* asset, int id) {
	int i;
	int exists = -1;
	int* newInts = NULL;
	Asset** newAssets = NULL;
	if (handler != NULL && asset != NULL) {
		// Look and check if the ID already exists
		for (i = 0; i < handler->size; i++)
			if (handler->ids[i] == id)
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
			newInts = (int*)realloc((void*)handler->ids, sizeof(int) * (handler->size + 1));
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
				handler->ids = (int*)realloc((void*)handler->ids, sizeof(int) * (handler->size));
				handler->vals = (Asset**)realloc((void*)handler->vals, sizeof(Asset*) * (handler->size));
				fprintf(stderr, "Failed to increment handler size (loadAssetIntoHandler with ID %i)\n", id);
				jSetError(ERROR_REALLOC_FAILED);
			}
		}
	} else {
		if (handler == NULL) {
			fprintf(stderr, "Handler does not exist (loadAssetIntoHandler with ID %i)\n", id);
			jSetError(ERROR_NULL_POINTER);
		}
		if (asset == NULL) {
			fprintf(stderr, "Asset passed was null (loadAssetIntoHandler with ID %i)\n", id);
			jSetError(ERROR_NULL_POINTER);
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
			fprintf(stderr, "Failed to create asset (createAsset)\n");
			jSetError(ERROR_ALLOC_FAILED);
		}
		if (pointer == NULL) {
			fprintf(stderr, "Pointer does not exist (createAsset)\n");
			jSetError(ERROR_NULL_POINTER);
		}
	}
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
AssetHandler* createAssetHandler() {
	AssetHandler* handler = (AssetHandler*)calloc(1, sizeof(AssetHandler));

	if (handler != NULL) {
		// no need to do anything here for now
	} else {
		fprintf(stderr, "Failed to create an asset handler (createAssetHandler)\n");
		jSetError(ERROR_ALLOC_FAILED);
	}

	return handler;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
void assetLoadINI(AssetHandler* assetHandler, Renderer* renderer, const char* filename) {
	INI* ini = loadINI(filename);
	uint32 i, j;

	if (assetHandler != NULL && renderer != NULL && ini != NULL) {
		// Load the texture ids first
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") == 0) {
				for (j = 0; j < ini->headers[i]->size; j++)
					loadAssetIntoHandler(
							assetHandler,
							createAsset(loadTexture(renderer, ini->headers[i]->vals[j]), texAsset),
							(int)atof(ini->headers[i]->keys[j])
					);
			}
		}

		// Phase 1: Sprites and hitboxes
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strlen(ini->headerNames[i]) > 0) {
				if (ini->headerNames[i][0] == INI_SPRITE_PREFIX) {
					if (assetGet(assetHandler, (int)atof(getKeyINI(ini, ini->headerNames[i], "texture_id", "0"))) != NULL) {
						loadAssetIntoHandler(
								assetHandler,
								createAsset(loadSpriteFromSheet(
										assetGet(assetHandler, (int) atof(
												getKeyINI(ini, ini->headerNames[i], "texture_id", "0")))->tex,
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "animation_length", "1")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "x_in_texture", "0")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "y_in_texture", "0")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "frame_width", "16")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "frame_height", "16")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "padding_width", "0")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "padding_height", "0")),
										(uint32) atof(getKeyINI(ini, ini->headerNames[i], "x_align", "0")),
										(uint16) atof(getKeyINI(ini, ini->headerNames[i], "frame_delay", "0")),
										(bool) atof(getKeyINI(ini, ini->headerNames[i], "looping", "0"))), sprAsset),
								(int) atof(ini->headerNames[i] + 1)
						);
					} else {
						fprintf(stderr, "Failed to load sprite of id %s, tex not found (assetLoadINI)\\n", ini->headerNames[i] + 1);
						jSetError(ERROR_ASSET_NOT_FOUND);
					}
				} else if (ini->headerNames[i][0] == INI_HITBOX_PREFIX) {
					hitboxType hType = hitRectangle;
					if (getKeyINI(ini, ini->headerNames[i], "type", "rectangle") == "circle") hType = hitCircle;
					loadAssetIntoHandler(
							assetHandler,
							createAsset(createHitbox(
									hType,
									atof(getKeyINI(ini, ini->headerNames[i], "radius", "0")),
									atof(getKeyINI(ini, ini->headerNames[i], "width", "0")),
									atof(getKeyINI(ini, ini->headerNames[i], "height", "0"))
							), hitAsset),
							(int)atof(ini->headerNames[i] + 1)
					);
				}
			}
		}

		// Phase 2: Everything else
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], "texture_ids") != 0) {
				if (ini->headerNames[i][0] == INI_TILEMAP_PREFIX) {
					loadAssetIntoHandler(
							assetHandler,
							createAsset(loadTileMap(
									getKeyINI(ini, ini->headerNames[i], "file", ""),
									(uint32)atof(getKeyINI(ini, ini->headerNames[i], "grid_width", "0")),
									(uint32)atof(getKeyINI(ini, ini->headerNames[i], "grid_height", "0")),
									(uint32)atof(getKeyINI(ini, ini->headerNames[i], "cell_width", "0")),
									(uint32)atof(getKeyINI(ini, ini->headerNames[i], "cell_height", "0"))), tileAsset),
							(int)atof(ini->headerNames[i] + 1)
					);
				} else if (ini->headerNames[i][0] == INI_ENTITY_PREFIX) {
					// Make sure we have all necessary assets
					if (assetGet(assetHandler, (int)atof(getKeyINI(ini, ini->headerNames[i], "sprite_id", "0"))) != NULL
					 && assetGet(assetHandler, (int)atof(getKeyINI(ini, ini->headerNames[i], "hitbox_id", "0"))) != NULL) {
						loadAssetIntoHandler(
								assetHandler,
								createAsset(createEntity(
										assetGet(assetHandler, (int) atof(
												getKeyINI(ini, ini->headerNames[i], "sprite_id", "0")))->spr,
										assetGet(assetHandler, (int) atof(
												getKeyINI(ini, ini->headerNames[i], "hitbox_id", "0")))->hitbox,
										(int) atof(getKeyINI(ini, ini->headerNames[i], "x", "0")),
										(int) atof(getKeyINI(ini, ini->headerNames[i], "y", "0")),
										(int) atof(getKeyINI(ini, ini->headerNames[i], "id", "0"))
								), entAsset),
								(int) atof(ini->headerNames[i] + 1)
						);
					} else {
						fprintf(stderr, "Failed to load entity of id %s (assetLoadINI)\n", ini->headerNames[i] + 1);
						jSetError(ERROR_ASSET_NOT_FOUND);
					}
				}
			}
		}
	} else {
		if (assetHandler == NULL) {
			fprintf(stderr, "Asset loader does not exist for file %s (assetLoadINI)\n", filename);
			jSetError(ERROR_NULL_POINTER);
		}
		if (renderer == NULL) {
			fprintf(stderr, "Renderer does not exist for file %s (assetLoadINI)\n", filename);
			jSetError(ERROR_NULL_POINTER);
		}
		if (ini == NULL) {
			fprintf(stderr, "Failed to load INI for file %s (assetLoadINI)\n", filename);
			jSetError(ERROR_OPEN_FAILED);
		}
	}

	freeINI(ini);
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
bool assetAssertRanges(AssetHandler* handler, int entRangeStart, int entRangeEnd, int sprRangeStart, int sprRangeEnd,
					   int tileRangeStart, int tileRangeEnd, int texRangeStart, int texRangeEnd, int hitRangeStart, int hitRangeEnd) {
	bool assert = true;
	int i = 0;

	if (handler != NULL) {
		while (i < handler->size && assert) {
			// We must make sure that whatever asset is here is proper
			if (handler->vals[i] != NULL) {
				if (handler->vals[i]->type == sprAsset) {
					if (handler->ids[i] > sprRangeEnd || handler->ids[i] < sprRangeStart) {
						assert = false;
						fprintf(stderr, "Asset of ID %i did not pass assertion (Wrong asset type - assetAssertRanges)\n", handler->ids[i]);
					}
				} else if (handler->vals[i]->type == entAsset) {
					if (handler->ids[i] > entRangeEnd || handler->ids[i] < entRangeStart) {
						assert = false;
						fprintf(stderr, "Asset of ID %i did not pass assertion (Wrong asset type - assetAssertRanges)\n", handler->ids[i]);
					}
				} else if (handler->vals[i]->type == hitAsset) {
					if (handler->ids[i] > hitRangeEnd || handler->ids[i] < hitRangeStart) {
						assert = false;
						fprintf(stderr, "Asset of ID %i did not pass assertion (Wrong asset type - assetAssertRanges)\n", handler->ids[i]);
					}
				} else if (handler->vals[i]->type == tileAsset) {
					if (handler->ids[i] > tileRangeEnd || handler->ids[i] < tileRangeStart) {
						assert = false;
						fprintf(stderr, "Asset of ID %i did not pass assertion (Wrong asset type - assetAssertRanges)\n", handler->ids[i]);
					}
				} else if (handler->vals[i]->type == texAsset) {
					if (handler->ids[i] > texRangeEnd || handler->ids[i] < texRangeStart) {
						assert = false;
						fprintf(stderr, "Asset of ID %i did not pass assertion (Wrong asset type - assetAssertRanges)\n", handler->ids[i]);
					}
				}
			} else {
				assert = false;
				fprintf(stderr, "Asset of ID %i did not pass assertion (Does not exist - assetAssertRanges)\n", handler->ids[i]);
			}
			i++;
		}
	} else {
		fprintf(stderr, "Handler does not exist (assetAssertRanges)\n");
		jSetError(ERROR_NULL_POINTER);
	}

	if (!assert)
		jSetError(ERROR_ASSET_WRONG_TYPE);

	return assert;
}
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
Asset* assetGet(AssetHandler* assetHandler, int key) {
	int i;
	Asset* asset = NULL;
	if (assetHandler != NULL) {
		for (i = 0; i < assetHandler->size; i++)
			if (assetHandler->ids[i] == key)
				asset = assetHandler->vals[i];
	} else {
		fprintf(stderr, "AssetHandler does not exist (assetGet)\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return asset;
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
		free(handler);
	}
}
///////////////////////////////////////////////////////////////