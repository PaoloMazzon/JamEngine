/// \file StringMap.h
/// \author lugi1
/// \brief Declares a string-map system for C-style strings
///
/// This should not be used in real time, rather for things
/// like loading and other operations not done on a frame-
/// to-frame basis.

#pragma once
#include "Constants.h"
#include "stdio.h"

/// \brief A key/val map for string:string operations
typedef struct {
	uint32 size; ///< Size of the two lists in elements
	uint32 sizeOfGarbagePile; ///< Pretty self-explanatory
	char** keys; ///< The keys
	char** vals; ///< The values
	char** garbage; ///< Heap-based strings that need to be freed upon destruction
} SMap;

/// \brief Creates an empty map
SMap* createSMap();

/// \brief Loads a map from an smap file
SMap* loadSMap(const char* filename);

/// \brief Prints the map in INI format to the given stream
void outputSMap(SMap* smap, FILE* stream);

/// \brief Sets a value to a key in a map
void setSMapVal(SMap* smap, char* key, char* val);

/// \brief Gets a value from a map
const char* getSMapVal(SMap* smap, char* key, char* def);

/// \brief Throws a string into the garbage
///
/// Do this for strings you're using in this thing that are dynamically
/// allocated so freeSMap can take care of them.
void throwAStringIntoTheGarbage(SMap* smap, char* garbage);

/// \brief Frees the map
void freeSMap(SMap* smap);