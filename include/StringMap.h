/// \file StringMap.h
/// \author lugi1
/// \brief Declares a string-map system for C-style strings
///
/// This should not be used in real time, rather for things
/// like loading and other operations not done on a frame-
/// to-frame basis.

#pragma once
#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A key/val map for string:string operations
typedef struct {
	int size; ///< Size of the two lists in elements
	int sizeOfGarbagePile; ///< Pretty self-explanatory
	char** keys; ///< The keys
	char** vals; ///< The values
	char** garbage; ///< Heap-based strings that need to be freed upon destruction
} JamStringMap;

/// \brief Creates an empty map
/// \throws ERROR_ALLOC_FAILED
JamStringMap* jamStringMapCreate();

/// \brief Loads a map from an smap file
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_INCORRECT_FORMAT
JamStringMap* jamStringMapLoad(const char *filename);

/// \brief Prints the map in INI format to the given stream
/// \throws ERROR_NULL_POINTER
void jamStringMapOutput(JamStringMap *smap, FILE *stream);

/// \brief Sets a value to a key in a map
/// \throws ERROR_NULL_POINTER
void jamStringMapSet(JamStringMap *smap, const char *key, char *val);

/// \brief Gets a value from a map
/// \throws ERROR_NULL_POINTER
const char* jamStringMapGet(JamStringMap *smap, const char *key, char *def);

/// \brief Frees the map
void jamStringMapFree(JamStringMap *smap);

#ifdef __cplusplus
}
#endif