//
// Created by lugi1 on 2018-11-11.
//

#pragma once
#include "Sprite.h"
#include "Texture.h"
#include "TileMap.h"
#include "Font.h"
#include "Entity.h"
#include "Hitbox.h"

/// \brief Loads lots of assets at once from files
/// to manage large projects
///
/// Do not use these lightly, they are an incredibly
/// helpful tool but they gobble up memory like there
/// is no tomorrow.
typedef struct {
	// For each type of asset, their is a key/val pair like a string map

	// Sprites
	int sprSize; ///< The size of the sprite list
	const char** sprKeys; ///< The keys that match up with the vals
	Sprite** spriteVals; ///< The actual values

	// Textures
	int texSize; ///< The size of the tex list
	const char** texKeys; ///< The keys that match up with the vals
	Texture** sheetVals; ///< The actual values

	// Tile maps
	int tileSize; ///< The size of the tile list
	const char** tileKeys; ///< The keys that match up with the vals
	TileMap** tileVals; ///< The actual values

	// Entities
	int entSize; ///< The size of the entity list
	const char** entKeys; ///< The keys that match up with the vals
	Entity** entityVals; ///< The actual values

	// Hitboxes
	int hitSize; ///< The size of the hitbox list
	const char** hitKeys; ///< The keys that match up with the vals
	Hitbox** hitboxVals; ///< The actual values
} AssetHandler;

/// \brief Creates an asset handler
AssetHandler* createAssetHandler();

/// \brief Loads a sprite
void assetLoadSprite(AssetHandler* assetHandler, const char* filename);

/// \brief Loads a texture
void assetLoadTexture(AssetHandler* assetHandler, const char* filename);

/// \brief Loads a tile map
void assetLoadTileMap(AssetHandler* assetHandler, const char* filename);

/// \brief Loads a entity
void assetLoadEntity(AssetHandler* assetHandler, const char* filename);

/// \brief Loads a hitbox
void assetLoadHitbox(AssetHandler* assetHandler, const char* filename);

/// \brief Loads all recognized assets from a directory
void assetLoadDirectory(AssetHandler* assetHandler, const char* directory);

/// \brief Finds and returns the asset for a given key or NULL if it can't find it
Sprite* assetGetSprite(AssetHandler* assetHandler, const char* key);

/// \brief Finds and returns the asset for a given key or NULL if it can't find it
Texture* assetGetTexture(AssetHandler* assetHandler, const char* key);

/// \brief Finds and returns the asset for a given key or NULL if it can't find it
TileMap* assetGetTileMap(AssetHandler* assetHandler, const char* key);

/// \brief Finds and returns the asset for a given key or NULL if it can't find it
Entity* assetGetEntity(AssetHandler* assetHandler, const char* key);

/// \brief Finds and returns the asset for a given key or NULL if it can't find it
Hitbox* assetGetHitbox(AssetHandler* assetHandler, const char* key);

/// \brief Frees an asset handler and all of its
/// components
AssetHandler* freeAssetHandler();