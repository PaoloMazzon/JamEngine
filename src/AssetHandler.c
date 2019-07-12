//
// Created by lugi1 on 2018-11-11.
//

#include "AssetHandler.h"
#include "INI.h"
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

///////////////////////////////////////////////////////////////
#define PHASE_GENERAL 0
#define PHASE_SPRITES 1
#define PHASE_ENTITIES 2
void assetLoadINI(AssetHandler* assetHandler, Renderer* renderer, const char* filename) {
	// TODO: This
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