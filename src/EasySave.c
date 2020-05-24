//
// Created by lugi1 on 2020-05-20.
//

#include "EasySave.h"
#include <JamError.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <EasySave.h>
#include <Buffer.h>

////////////////// Helper functions //////////////////
const char* copyString(const char* string) {
	char* str = malloc(strlen(string) + 1);

	if (str != NULL)
		strcpy(str, string);
	else
		jSetError(ERROR_ALLOC_FAILED, "Failed to copy string %s", string);

	return str;
}

static _JamEasyData* _jamEasyDataCreate(const char* name, _JamEasyDataType type) {
	_JamEasyData* data = malloc(sizeof(_JamEasyData));

	if (data != NULL) {
		data->type = type;
		data->key = name;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create data of name %s", name);
	}

	return data;
}

static void _jamEasyDataFree(_JamEasyData* data) {
	if (data != NULL) {
		free((void*)data->key);
		if (data->type == bytesVal)
			free(data->data);
	}
}
//////////////////////////////////////////////////////
JamEasySave* jamEasySaveLoad(const char* filename) {
	JamBuffer* buffer = jamBufferLoad(filename);
	JamEasySave* save = malloc(sizeof(JamEasySave));
	bool error = false;
	uint32 i, entrySize, strLen, type, dataSize;
	_JamEasyData* entry;
	char* tempString;
	_JamEasyData** array;

	if (buffer != NULL && save != NULL) {
		save->filename = copyString(filename);
		save->data = NULL;
		save->size = 0;

		if (buffer->size != 0) {
			// Read the number of entries and loop over them
			jamBufferReadByte4(buffer, &save->size);
			save->data = malloc(sizeof(_JamEasyData*) * save->size) || jSetError(ERROR_ALLOC_FAILED, "Failed to allocate");
			for (i = 0; i < save->size && save->data != NULL; i++) {
				// Extract data for current entry
				jamBufferReadByte2(buffer, &strLen);
				jamBufferReadByte4(buffer, &type);
				jamBufferReadByte4(buffer, &dataSize);

				// Create the data we need, error check, and load up the struct
				tempString = malloc(strLen);
				entry = _jamEasyDataCreate(tempString, type);
				if (entry != NULL && tempString != NULL) {
					jamBufferReadByteX(buffer, tempString, strLen);

					// Now we extract data based on what this is
					if (type == doubleVal) {
						jamBufferReadByte8(buffer, &entry->doubleVal);
					} else if (type == uint64Val) {
						jamBufferReadByte8(buffer, &entry->uint64Val);
					} else if (type == uint32Val) {
						jamBufferReadByte4(buffer, &entry->uint32Val);
					} else if (type == uint16Val) {
						jamBufferReadByte2(buffer, &entry->uint16Val);
					} else if (type == uint8Val) {
						jamBufferReadByte1(buffer, &entry->uint8Val);
					} else if (type == sint64Val) {
						jamBufferReadByte8(buffer, &entry->sint64Val);
					} else if (type == sint32Val) {
						jamBufferReadByte4(buffer, &entry->sint32Val);
					} else if (type == sint16Val) {
						jamBufferReadByte2(buffer, &entry->sint16Val);
					} else if (type == sint8Val) {
						jamBufferReadByte1(buffer, &entry->sint8Val);
					} else if (type == stringVal) {
						jamBufferReadByteX(buffer, &entry->stringVal, dataSize);
					} else if (type == bytesVal) {
						jamBufferReadByteX(buffer, &entry->data, dataSize);
						entry->size = dataSize;
					}

					save->data[i] = entry;
				} else {
					jSetError(ERROR_ALLOC_FAILED, "Failed to create buffer for string");
				}
			}
		}
	} else {
		if (buffer == NULL)
			jSetError(ERROR_FILE_FAILED, "Failed to open %s", filename);
		if (save == NULL)
			jSetError(ERROR_NULL_POINTER, "Failed to create save");
	}

	return save;
}

double jamEasySaveGetDouble(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetDouble(JamEasySave* easySave, const char* key, double val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint64 jamEasySaveGetUint64(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetUint64(JamEasySave* easySave, const char* key, uint64 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint32 jamEasySaveGetUint32(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetUint32(JamEasySave* easySave, const char* key, uint32 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint16 jamEasySaveGetUint16(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetUint16(JamEasySave* easySave, const char* key, uint16 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint8 jamEasySaveGetUint8(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetUint8(JamEasySave* easySave, const char* key, uint8 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint64 jamEasySaveGetSint64(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetSint64(JamEasySave* easySave, const char* key, sint64 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint32 jamEasySaveGetSint32(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetSint32(JamEasySave* easySave, const char* key, sint32 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint16 jamEasySaveGetSint16(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetSint16(JamEasySave* easySave, const char* key, sint16 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint8 jamEasySaveGetSint8(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetSint8(JamEasySave* easySave, const char* key, sint8 val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

const char* jamEasySaveGetString(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetString(JamEasySave* easySave, const char* key, const char* val) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void* jamEasySaveGetData(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetData(JamEasySave* easySave, const char* key, void* data, uint32 size) {
	if (easySave != NULL) {

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveFlush(JamEasySave* easySave) {
	if (easySave != NULL) {
		FILE* output = fopen(easySave->filename, "wb");
		int i;
		uint16 strLen;

		if (output != NULL) {
			fwrite(easySave->size, 4, 1, output);

			for (i = 0; i < easySave->size; i++) {
				strLen = (uint16)(strlen(easySave->data[i]->stringVal) + 1);
				fwrite(&strLen, 2, 1, output);
				fwrite(&easySave->data[i]->type, 4, 1, output);
				fwrite(&easySave->data[i]->size, 4, 1, output);
				fwrite(&easySave->data[i]->key, strLen, 1, output);

				if (easySave->data[i]->type == doubleVal) {
					fwrite(&easySave->data[i]->doubleVal, 8, 1, output);
				} else if (easySave->data[i]->type == uint64Val) {
					fwrite(&easySave->data[i]->uint64Val, 8, 1, output);
				} else if (easySave->data[i]->type == uint32Val) {
					fwrite(&easySave->data[i]->uint32Val, 4, 1, output);
				} else if (easySave->data[i]->type == uint16Val) {
					fwrite(&easySave->data[i]->uint16Val, 2, 1, output);
				} else if (easySave->data[i]->type == uint8Val) {
					fwrite(&easySave->data[i]->uint8Val, 1, 1, output);
				} else if (easySave->data[i]->type == sint64Val) {
					fwrite(&easySave->data[i]->sint64Val, 8, 1, output);
				} else if (easySave->data[i]->type == sint32Val) {
					fwrite(&easySave->data[i]->sint32Val, 4, 1, output);
				} else if (easySave->data[i]->type == sint16Val) {
					fwrite(&easySave->data[i]->sint16Val, 2, 1, output);
				} else if (easySave->data[i]->type == sint8Val) {
					fwrite(&easySave->data[i]->sint8Val, 1, 1, output);
				} else if (easySave->data[i]->type == stringVal) {
					fwrite(&easySave->data[i]->stringVal, strLen, 1, output);
				} else if (easySave->data[i]->type == bytesVal) {
					fwrite(&easySave->data[i]->data, easySave->data[i]->size, 1, output);
				}
			}
			
			fclose(output);
		} else {
			jSetError(ERROR_FILE_FAILED, "Failed to flush to %s", easySave->filename);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveClose(JamEasySave* easySave) {
	int i;
	if (easySave != NULL) {
		jamEasySaveFlush(easySave);

		// Free entries
		for (i = 0; i < easySave->size; i++) {
			if (easySave->data[i]->type == stringVal)
				free((void*)easySave->data[i]->stringVal);
			else if (easySave->data[i]->type == bytesVal)
				free(easySave->data[i]->data);
			free((void*)easySave->data[i]->key);
		}

		// Free the vector and save
		free(easySave->data);
		free((void*)easySave->filename);
		free(easySave);
	}
}