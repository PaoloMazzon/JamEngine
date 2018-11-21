//
// Created by lugi1 on 2018-11-11.
//

#pragma once
#include "Constants.h"
#include "stdio.h"

/// \brief A key/val map for string:string operations
typedef struct {
	uint32 size; ///< Size of the two lists in elements
	const char** keys; ///< The keys
	const char** vals; ///< The values
} SMap;

/// \brief Creates an empty map
SMap* createSMap();

/// \brief Loads a map from an smap file
SMap* loadSMap(const char* filename);

/// \brief Prints the map in INI format to the given stream
void outputSMap(SMap* smap, FILE* stream);

/// \brief Sets a value to a key in a map
void setSMapVal(SMap* smap, const char* key, const char* val);

/// \brief Gets a value from a map
const char* getSMapVal(SMap* smap, const char* key, const char* def);

/// \brief Frees the map
void freeSMap(SMap* smap);