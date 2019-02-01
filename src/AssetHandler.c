//
// Created by lugi1 on 2018-11-11.
//

#include "AssetHandler.h"
#include "StringMap.h"
#include <stdio.h>
#include <dirent.h>

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
			}
		}
	} else {
		if (handler == NULL)
			fprintf(stderr, "Handler does not exist (loadAssetIntoHandler with ID %i)\n", id);
		if (asset == NULL)
			fprintf(stderr, "Asset passed was null (loadAssetIntoHandler with ID %i)\n", id);
	}
}
///////////////////////////////////////////////////////////////

//////////////////////// NOTE!!!! ////////////////////////
/// These following 5 functions do not check weather or
/// not the asset handler (or smap) exists because they are only
/// used by functions that check first anyway. Do not use
/// these functions on your own.

// Loads a sprite
Sprite* assetLoadSprite(AssetHandler* assetHandler, SMap* file) {
	Sprite* asset = NULL;
	uint32 x, y, w, h;
	uint32 animLength;
	uint32 paddingW;
	uint32 paddingH;
	uint32 xAlign;
	uint16 frameDelay;
	bool looping;
	Asset* base;

	// Things relative to the texture
	x = (uint32) strtol(getSMapVal(file, "x_in_texture", "0"), NULL, 10);
	y = (uint32) strtol(getSMapVal(file, "y_in_texture", "0"), NULL, 10);
	w = (uint32) strtol(getSMapVal(file, "frame_width", "0"), NULL, 10);
	h = (uint32) strtol(getSMapVal(file, "frame_height", "0"), NULL, 10);
	base = assetGet(assetHandler, strtol(getSMapVal(file, "texture_id", "-1"), NULL, 10));

	// Load other things
	animLength = (uint32)strtol(getSMapVal(file, "animation_length", "0"), NULL, 10);
	paddingW = (uint32)strtol(getSMapVal(file, "padding_width", "0"), NULL, 10);
	paddingH = (uint32)strtol(getSMapVal(file, "padding_height", "0"), NULL, 10);
	xAlign = (uint32)strtol(getSMapVal(file, "x_align", "0"), NULL, 10);
	frameDelay = (uint16)strtol(getSMapVal(file, "frame_delay", "0"), NULL, 10);
	looping = (bool)strtol(getSMapVal(file, "looping", "0"), NULL, 10);

	// We must make sure that we aren't being given something other than a texture
	if (base != NULL && base->type == texAsset) {
		// We are either going to have just a single image
		if ((bool) strtol(getSMapVal(file, "single_frame", "0"), NULL, 10)) {
			// We must create the sprite and set some things up
			asset = createSprite(0, 0, 0);
			spriteAppendFrame(asset, createFrame(base->tex, x, y, w, h));
		} else {
			// We need to get the sprite sheet things
			asset = loadSpriteFromSheet(base->tex, animLength, x, y, w, h, paddingW, paddingH, xAlign, frameDelay, looping);
		}
	} else if (base != NULL && base->type != texAsset) {
		fprintf(stderr, "Incorrect asset type passed to \"texture_id\" (assetLoadSprite)\n");
	}
	// If we still don't have a sprite, make one
	if (asset == NULL)
		asset = createSprite(0, 0, 0);

	// We can only load if we know our sprite exists
	if (asset != NULL) {
		// These are things that are going to get set regardless of how the sprite is loaded
		asset->originX = (sint32) strtol(getSMapVal(file, "origin_x", "0"), NULL, 10);
		asset->originY = (sint32) strtol(getSMapVal(file, "origin_y", "0"), NULL, 10);
		asset->rot = strtod(getSMapVal(file, "rotation", "0"), NULL);
		asset->alpha = (uint8) strtol(getSMapVal(file, "alpha", "255"), NULL, 10);
		asset->updateOnDraw = (bool) strtol(getSMapVal(file, "update_on_draw", "1"), NULL, 10);
		asset->scaleX = strtof(getSMapVal(file, "scale_x", "1"), NULL);
		asset->scaleY = strtof(getSMapVal(file, "scale_y", "1"), NULL);
		asset->currentFrame = (uint32) strtol(getSMapVal(file, "starting_frame", "0"), NULL, 10);
		asset->animationLength = animLength;
		asset->frameDelay = frameDelay;
		asset->looping = looping;
	}

	return asset;
}

// Loads a texture, but directly into the handler
void assetLoadTexture(AssetHandler* assetHandler, SMap* file, Renderer* renderer) {
	int i;
	Asset* currentAsset;

	// We need to loop through all of the id/files in the map
	for (i = 0; i < file->size; i++) {
		// Set up the asset
		currentAsset = (Asset*)malloc(sizeof(Asset));
		if (currentAsset != NULL) {
			currentAsset->type = texAsset;
			currentAsset->tex = loadTexture(renderer, file->keys[i]);
		}

		// Load it into the list
		loadAssetIntoHandler(assetHandler, currentAsset, strtol(file->vals[i], NULL, 10));
	}
}

// Loads a tile map
TileMap* assetLoadTileMap(SMap* file) {
	uint32 w, h, cellW, cellH;
	w = (uint32)strtol(getSMapVal(file, "grid_width", "0"), NULL, 10);
	h = (uint32)strtol(getSMapVal(file, "grid_height", "0"), NULL, 10);
	cellW = (uint32)strtol(getSMapVal(file, "cell_width", "0"), NULL, 10);
	cellH = (uint32)strtol(getSMapVal(file, "cell_height", "0"), NULL, 10);
	return loadTileMap(getSMapVal(file, "file", ""), w, h, cellW, cellH);
}

// Loads a entity
Entity* assetLoadEntity(AssetHandler* assetHandler, SMap* file) {
	int sprID = (int)strtol(getSMapVal(file, "sprite_id", "-1"), NULL, 10);
	int hitID = (int)strtol(getSMapVal(file, "hitbox_id", "-1"), NULL, 10);
	Entity* ent = NULL;
	Asset* spr = assetGet(assetHandler, sprID);
	Asset* hit = assetGet(assetHandler, hitID);
	int x = strtol(getSMapVal(file, "x", "0"), NULL, 10);
	int y = strtol(getSMapVal(file, "y", "0"), NULL, 10);
	int id = strtol(getSMapVal(file, "id", "0"), NULL, 10);

	// We need to know for sure that the assets are correct
	if ((spr != NULL && spr->type != sprAsset) || (hit != NULL && hit->type != hitAsset)) {
		// We can't use these assets
		fprintf(stderr, "Incorrect asset types passed (assetLoadEntity with sprite_id=%i and hitbox_id=%i)\n", sprID, hitID);
	} else {
		if (spr == NULL && hit == NULL)
			ent = createEntity(NULL, NULL, x, y, id);
		else if (spr != NULL && hit == NULL)
			ent = createEntity(spr->spr, NULL, x, y, id);
		else if (spr == NULL && hit != NULL)
			ent = createEntity(NULL, hit->hitbox, x, y, id);
		else
			ent = createEntity(spr->spr, hit->hitbox, x, y, id);
	}

	return ent;
}

// Loads a hitbox
Hitbox* assetLoadHitbox(SMap* file) {
	double w, h, r;
	hitboxType type;
	w = strtod(getSMapVal(file, "width", "0"), NULL);
	h = strtod(getSMapVal(file, "height", "0"), NULL);
	r = strtod(getSMapVal(file, "radius", "0"), NULL);

	if (strcmp(getSMapVal(file, "type", "rectangle"), "rectangle") == 0)
		type = hitRectangle;
	else
		type = hitCircle;

	return createHitbox(type, r, w, h);
}

///////////////////////////////////////////////////////////////
AssetHandler* createAssetHandler() {
	AssetHandler* handler = (AssetHandler*)calloc(1, sizeof(AssetHandler));

	if (handler != NULL) {
		// no need to do anything here for now
	} else {
		fprintf(stderr, "Failed to create an asset handler (createAssetHandler)\n");
	}

	return handler;
}
///////////////////////////////////////////////////////////////

#define PHASE_GENERAL 0
#define PHASE_SPRITES 1
#define PHASE_ENTITIES 2

///////////////////////////////////////////////////////////////
// TODO: Investigate the problems in here
void assetLoadDirectory(AssetHandler* assetHandler, Renderer* renderer, const char* directory) {
	DIR *dir;
	struct dirent* entCurrent;
	char* tempString = (char*)calloc(1, 256);
	char* fullFileName = (char*)calloc(1, 256 * 2);
	char* extension = NULL;
	int id = 0;
	Asset* asset;
	uint8 phase = PHASE_GENERAL;
	SMap* file = NULL;

	if (tempString != NULL && assetHandler != NULL && fullFileName != NULL) {
		if ((dir = opendir(directory)) != NULL) {
			/* There are multiple phases in loading assets because the order
			 * in which we load them matters. This means we need to loop through
			 * the directory multiple times to hit the right files at the right
			 * time.
			 */
			while (phase < 3) {
				// Here we will loop through the directory
				while ((entCurrent = readdir(dir)) != NULL) {
					/* The dirent's struct chooses to use an array of chars instead
					 * of a classic C-string, and as such we need to convert it to
					 * a C-string to use the built-in functions for extracting file
					 * extension (as apposed to writing my own that works with char
					 * arrays).
					 */
					memset((void *) fullFileName, 0, 256 * 2);
					memset((void *) tempString, 0, 256);
					memcpy(tempString, entCurrent->d_name, 256);
					memcpy(fullFileName, directory, 256);
					strcat(fullFileName, tempString);
					extension = strrchr(tempString, (int) '.');

					// Now we must check the file extension and act accordingly
					if (strcmp(extension, ".sprite") == 0 || strcmp(extension, ".map") == 0 ||
						strcmp(extension, ".texhashes") == 0 || strcmp(extension, ".hitbox") == 0 ||
						strcmp(extension, ".entity") == 0) {
						// We know that it is a file we want
						file = loadSMap(fullFileName);

						if (file != NULL) {
							/* This is an interesting segment. We already know that the
							 * file in question is open and it is an extension we want,
							 * but we don't know if we're ready for a given asset or not
							 * since we can't load entities before sprites and such.
							 * Because of this, we have 3 phases: general, sprites, and
							 * entities. Once we have everything except for sprites and
							 * entities, we load sprites, then entities after that.
							 */
							if (strcmp(extension, ".sprite") == 0 && phase == PHASE_SPRITES) {
								id = strtol(getSMapVal(file, "id", "0"), NULL, 10);
								asset = (Asset *) malloc(sizeof(Asset));
								if (asset != NULL) {
									asset->type = sprAsset;
									asset->spr = assetLoadSprite(assetHandler, file);
									loadAssetIntoHandler(assetHandler, asset, id);
								} else
									fprintf(stderr, "Failed to create asset (assetLoadDirectory)\n");
							} else if (strcmp(extension, ".map") == 0 && phase == PHASE_GENERAL) {
								id = strtol(getSMapVal(file, "id", "0"), NULL, 10);
								asset = (Asset *) malloc(sizeof(Asset));
								if (asset != NULL) {
									asset->type = tileAsset;
									asset->tileMap = assetLoadTileMap(file);
									loadAssetIntoHandler(assetHandler, asset, id);
								} else
									fprintf(stderr, "Failed to create asset (assetLoadDirectory)\n");
							} else if (strcmp(extension, ".entity") == 0 && phase == PHASE_ENTITIES) {
								id = strtol(getSMapVal(file, "id", "0"), NULL, 10);
								asset = (Asset *) malloc(sizeof(Asset));
								if (asset != NULL) {
									asset->type = entAsset;
									asset->entity = assetLoadEntity(assetHandler, file);
									loadAssetIntoHandler(assetHandler, asset, id);
								} else
									fprintf(stderr, "Failed to create asset (assetLoadDirectory)\n");
							} else if (strcmp(extension, ".hitbox") == 0 && phase == PHASE_GENERAL) {
								id = strtol(getSMapVal(file, "id", "0"), NULL, 10);
								asset = (Asset *) malloc(sizeof(Asset));
								if (asset != NULL) {
									asset->type = hitAsset;
									asset->hitbox = assetLoadHitbox(file);
									loadAssetIntoHandler(assetHandler, asset, id);
								} else
									fprintf(stderr, "Failed to create asset (assetLoadDirectory)\n");
							} else if (strcmp(extension, ".texhashes") == 0 && phase == PHASE_GENERAL) {
								assetLoadTexture(assetHandler, file, renderer);
							}
						} else {
							fprintf(stderr, "Failed to load file %s (assetLoadDirectory)\n", tempString);
						}
					}
				}
				phase += 1;
				rewinddir(dir);
			}
			closedir(dir);
		} else {
			fprintf(stderr, "Failed to open directory (assetLoadDirectory)\n");
		}
	} else {
		if (assetHandler == NULL)
			fprintf(stderr, "Asset handler does not exist(assetLoadDirectory: %s)", directory);
		if (tempString == NULL)
			fprintf(stderr, "Failed to create temporary string (assetLoadDirectory: %s)", directory);
	}
	free(tempString);
	free(fullFileName);
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
	}

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
	}
}
///////////////////////////////////////////////////////////////