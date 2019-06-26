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
INI* createINI();

/// \brief Loads an INI from a file
INI* loadINI(const char* filename);

/// \brief This will print the INI file to a stream
/// 
/// If you want to save an INI file to load it later,
/// just open a file, pass it here, and close the file.
/// This function will properly print a formatted INI
/// file that loadINI will be able to read. It's also
/// good for debugging because you can print your INIs
/// to stdout/stderr/whatever.
void outputINI(INI* ini, FILE* stream);

/// \brief Set a key in an INI
void setKeyINI(INI* ini, const char* header, const char* key, char* val);

/// \brief Gets a key from an INI
char* getKeyINI(INI* ini, const char* header, const char* key, char* def);

/// \brief Puts something to the garbage pile to be freed later
void throwInGarbageINI(INI* ini, char* string);

/// \brief Destroys an INI and all memory it allocated
void freeINI(INI* ini);