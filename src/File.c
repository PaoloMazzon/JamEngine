//
// Created by lugi1 on 2018-11-15.
//

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "File.h"
#include "JamError.h"

/////////////////////////////////////////////////////////
JamStringList* jamStringListCreate() {
	JamStringList* list;
	list = (JamStringList*)calloc(1, sizeof(JamStringList));

	// Check that it worked alright
	if (list == NULL) {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate string list (jamStringListCreate)");
	}

	return list;
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
JamStringList* jamStringListLoad(const char *fname) {
	JamStringList* list = jamStringListCreate();
	FILE* file = fopen(fname, "r");
	char* currentString;
	fpos_t startOfLine;
	size_t sizeOfLine;
	fpos_t startOfNewLine;
	bool quit = false;
	bool foundEndOfLine;
	char currentChar;

	if (file != NULL && list != NULL) {
		fgetpos(file, &startOfNewLine);

		// Continue to nab lines until EOF or error
		while (!feof(file) && !quit) {
			foundEndOfLine = false;
			sizeOfLine = 0;
			fsetpos(file, &startOfNewLine);
			fgetpos(file, &startOfLine);

			// Find the end of the line
			while (!feof(file) && !foundEndOfLine) {
				currentChar = (char)fgetc(file);
				if (currentChar == '\r' || currentChar == '\n' || feof(file)) {
					// We found the end, but we must move the cursor ahead one if its a CRLF
					if (currentChar == '\r')
						fgetc(file);

					// We must break out of the loop
					if (feof(file))
						quit = true;

					// Record where to start the next line then go back to the start of this one
					fgetpos(file, &startOfNewLine);
					fsetpos(file, &startOfLine);
					foundEndOfLine = true;
				}
				sizeOfLine++;
			}

			// Create a space to copy the string into
			currentString = (char*)calloc(1, sizeOfLine);
			if (currentString != NULL) {
				// Get the string, then throw it in the list
				fgets(currentString, (int)sizeOfLine, file);
				jamStringListAppend(list, currentString, true);
			} else {
				jSetError(ERROR_ALLOC_FAILED, "Failed to create string (jamStringListLoad)");
				quit = true;
			}
		}
	} else {
		if (list == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "List could not be allocated (jamStringListLoad)");
		}
		if (file == NULL) {
			jSetError(ERROR_OPEN_FAILED, "File could not be opened (jamStringListLoad)");
		}
	}

	if (file != NULL)
		fclose(file);
	return list;
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
void jamStringListAppend(JamStringList *list, char *string, bool heapBased) {
	char** newList = NULL;
	bool* newBools = NULL;

	// We must make sure it exists first
	if (list != NULL && string != NULL) {
		// Reallocate the string list
		newList = (char**)realloc(list->strList, (list->size + 1) * sizeof(char*));
		newBools = (bool*)realloc(list->dynamic, (list->size + 1) * sizeof(bool));

		// Double check that it worked
		if (newList != NULL && newBools != NULL) {
			// Set the new list pointer
			list->strList = newList;
			list->dynamic = newBools;

			// Throw in the new string
			list->strList[list->size] = string;
			list->dynamic[list->size] = heapBased;
			list->size++;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Could not reallocate string list(s) (jamStringListAppend)");
			// Just in case one was initialized and the other wasn't
			free(newBools);
			free(newList);
		}
	} else {
		if (list == NULL) {
			jSetError(ERROR_NULL_POINTER, "List does not exist (jamStringListAppend)");
		}
		if (string == NULL) {
			jSetError(ERROR_NULL_POINTER, "String does not exist (jamStringListAppend)");
		}
	}
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
JamStringList* jamStringExplode(const char *string, char delim, bool ignoreQuotes) {
	int i;
	bool inQuotes = false;
	JamStringList* list = jamStringListCreate();
	int lastLocation = 0;
	char* currentBuffer;
	int stringLength = (int)strlen(string);
	bool cameFromQuotes = false;

	for (i = 0; i < stringLength; i++) {
		// Only ignore quotes if we were told to, also both types of quotes
		if ((string[i] == '\'' || string[i] == '"') && ignoreQuotes) {
			inQuotes = !inQuotes;
			cameFromQuotes = true;
		}

		// We hit a delim (or the end of the string) and we aren't in quotes
		if ((string[i] == delim || i == stringLength - 1) && !inQuotes) {
			// Account for the edge case if we're at the end
			if (i == stringLength - 1)
				i++;

			// Don't copy the quotation marks
			if (!cameFromQuotes)
				currentBuffer = (char*)malloc(i - lastLocation + 1);
			else
				currentBuffer = (char*)malloc(i - lastLocation - 1);

			if (currentBuffer != NULL) {
				if (!cameFromQuotes)
					memcpy((void*)currentBuffer, (const void*)(string + lastLocation), i - lastLocation);
				else
					memcpy((void*)currentBuffer, (const void*)(string + lastLocation + 1), i - lastLocation - 2);
				currentBuffer[lastLocation - i] = 0;
				jamStringListAppend(list, currentBuffer, true);
				cameFromQuotes = false;
			} else {
				jSetError(ERROR_ALLOC_FAILED, "Failed to create string (jamStringExplode)");
			}
			lastLocation = i + 1;
		}
	}

	return list;
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
void jamStringListFree(JamStringList *list) {
	int i;
	if (list != NULL) {
		for (i = 0; i < list->size; i++)
			if (list->dynamic[i])
				free(list->strList[i]);
		free(list->strList);
		free(list->dynamic);
		free(list);
	}
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// You're responsible for the memory allocated here
const char* jamGetNameOfFile(const char* filename) {
	int i;
	char* string;
	int len = strlen(filename);
	int startPos = 0;
	int endPos = 0;

	// Locate the name part of the filename
	for (i = 0; i < len; i++) {
		if (filename[i] == '/' || filename[i] == '\\')
			startPos = i + 1;
		if (filename[i] == '.')
			endPos = i;
	}

	// Allocate the string
	string = malloc(endPos - startPos + 1);

	if (string != NULL) {
		string[endPos - startPos] = 0;
		memcpy(string, filename + startPos, endPos - startPos);
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate new string");
	}
	
	return string;
}
///////////////////////////////////////////////////////////
