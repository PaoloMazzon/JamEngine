/// \file INI.h
/// \author lugi1
/// \brief An easy way to manage INI files
///
/// This file makes it easy to create load and manage
/// INI files in C.

#pragma once
#include "StringMap.h"
#include <stdio.h>

/// \brief Holds information pertaining to INI files
typedef struct {
	SMap** headers; ///< Each one is a different header
	char** headerNames; ///< Corrosponding name of each header
	int numberOfHeaders; ///< How many headers we have
	char** garbagePile; ///< Pointers that need to be freed
	int sizeOfGarbagePile; ///< Number of things in the garbage pile
} INI;

/// \brief Creates a new INI struct
/// \throws ERROR_ALLOC_FAILED
INI* createINI();

/// \brief Loads an INI from a file
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPEN_FAILED
INI* loadINI(const char* filename);

/// \brief This will print the INI file to a stream
/// 
/// If you want to save an INI file to load it later,
/// just open a file, pass it here, and close the file.
/// This function will properly print a formatted INI
/// file that loadINI will be able to read. It's also
/// good for debugging because you can print your INIs
/// to stdout/stderr/whatever.
///
/// \throws ERROR_NULL_POINTER
void outputINI(INI* ini, FILE* stream);

/// \brief Set a key in an INI
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_NULL_POINTER
void setKeyINI(INI* ini, const char* header, const char* key, char* val);

/// \brief Gets a key from an INI
/// \throws ERROR_NULL_POINTER
char* getKeyINI(INI* ini, const char* header, const char* key, char* def);

/// \brief Puts something to the garbage pile to be freed on destruction
///
/// Don't use this function yourself unless you specifically
/// want your string destroyed at precisely the time the ini
/// will be.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_REALLOC_FAILED
void throwInGarbageINI(INI* ini, char* string);

/// \brief Destroys an INI and all memory it allocated
void freeINI(INI* ini);