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
		if (data->type == dt_BytesVal)
			free(data->data);
		if (data->type == dt_StringVal)
			free((void*)data->stringVal);
		free(data);
	}
}

// Finds and checks the data type of key assuming save is a valid pointer and returns it in ret
static bool _jamEasySaveFindAssert(JamEasySave* save, const char* key, _JamEasyDataType type, _JamEasyData** ret) {
	int i;
	bool found = false;

	for (i = 0; i < save->size && !found; i++) {
		if (strcmp(save->data[i]->key, key) == 0) {
			if (save->data[i]->type == type) {
				*ret = save->data[i];
				found = true;
			} else {
				jSetError(ERROR_WARNING, "Data type not matching key [type:key] [%i:%s]", type, key);
			}
		}
	}

	return found;
}

// Finds a location for data in the save and returns the index (easy save must exist and this will either
// overwrite old data or make a new one)
int _jamEasySaveFindDataSpot(JamEasySave* save, const char* key) {
	int i;
	int spot = -1;
	_JamEasyData** newList;
	_JamEasyData* newData;

	// First we check if we can just find it
	for (i = 0; (i < save->size && spot == -1); i++)
		if (strcmp(key, save->data[i]->key) == 0)
			spot = i;

	if (spot == -1) {
		newList = realloc(save->data, sizeof(_JamEasyData*) * (save->size + 1));
		newData = _jamEasyDataCreate(copyString(key), dt_Sint32Val);
		if (newList != NULL && newData != NULL) {
			save->data = newList;
			newList[save->size] = newData;
			spot = save->size;
			save->size++;
		} else {
			i = -1;
			jSetError(ERROR_REALLOC_FAILED, "Failed to append key to save %s", key);
		}
	}

	return spot;
}
//////////////////////////////////////////////////////

JamEasySave* jamEasySaveLoad(const char* filename) {
	JamBuffer* buffer = jamBufferLoad(filename);
	JamEasySave* save = malloc(sizeof(JamEasySave));
	uint32 i, strLen, type, dataSize;
	_JamEasyData* entry;
	char* tempString;

	if (buffer != NULL && save != NULL) {
		save->filename = copyString(filename);
		save->data = NULL;
		save->size = 0;

		if (buffer->size != 0) {
			// Read the number of entries and loop over them
			jamBufferReadByte4(buffer, &save->size);
			save->data = malloc(sizeof(_JamEasyData*) * save->size);
			if (save->data == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to allocate");
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
					if (type == dt_DoubleVal) {
						jamBufferReadByte8(buffer, &entry->doubleVal);
					} else if (type == dt_Uint64Val) {
						jamBufferReadByte8(buffer, &entry->uint64Val);
					} else if (type == dt_Uint32Val) {
						jamBufferReadByte4(buffer, &entry->uint32Val);
					} else if (type == dt_Uint16Val) {
						jamBufferReadByte2(buffer, &entry->uint16Val);
					} else if (type == dt_Uint8Val) {
						jamBufferReadByte1(buffer, &entry->uint8Val);
					} else if (type == dt_Sint64Val) {
						jamBufferReadByte8(buffer, &entry->sint64Val);
					} else if (type == dt_Sint32Val) {
						jamBufferReadByte4(buffer, &entry->sint32Val);
					} else if (type == dt_Sint16Val) {
						jamBufferReadByte2(buffer, &entry->sint16Val);
					} else if (type == dt_Sint8Val) {
						jamBufferReadByte1(buffer, &entry->sint8Val);
					} else if (type == dt_StringVal) {
						entry->stringVal = malloc(dataSize);
						if (entry->stringVal != NULL)
							jamBufferReadByteX(buffer, (void*)entry->stringVal, dataSize);
						else
							jSetError(ERROR_ALLOC_FAILED, "Failed to allocate string in %s", filename);
					} else if (type == dt_BytesVal) {
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

	jamBufferFree(buffer);

	return save;
}

double jamEasySaveGetDouble(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_DoubleVal, &data))
			return data->doubleVal;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetDouble(JamEasySave* easySave, const char* key, double val) {
if (easySave != NULL) {
	int i = _jamEasySaveFindDataSpot(easySave, key);
	if (i != -1) {
		easySave->data[i]->doubleVal = val;
		easySave->data[i]->type = dt_DoubleVal;
	}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint64 jamEasySaveGetUint64(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Uint64Val, &data))
			return data->uint64Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetUint64(JamEasySave* easySave, const char* key, uint64 val) {
if (easySave != NULL) {
	int i = _jamEasySaveFindDataSpot(easySave, key);
	if (i != -1) {
		easySave->data[i]->uint64Val = val;
		easySave->data[i]->type = dt_Uint64Val;
	}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint32 jamEasySaveGetUint32(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Uint32Val, &data))
			return data->uint32Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetUint32(JamEasySave* easySave, const char* key, uint32 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->uint32Val = val;
			easySave->data[i]->type = dt_Uint32Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint16 jamEasySaveGetUint16(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Uint16Val, &data))
			return data->uint16Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetUint16(JamEasySave* easySave, const char* key, uint16 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->uint16Val = val;
			easySave->data[i]->type = dt_Uint16Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

uint8 jamEasySaveGetUint8(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Uint8Val, &data))
			return data->uint8Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetUint8(JamEasySave* easySave, const char* key, uint8 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->uint8Val = val;
			easySave->data[i]->type = dt_Uint8Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint64 jamEasySaveGetSint64(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Sint64Val, &data))
			return data->sint64Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetSint64(JamEasySave* easySave, const char* key, sint64 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->sint64Val = val;
			easySave->data[i]->type = dt_Sint64Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint32 jamEasySaveGetSint32(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Sint32Val, &data))
			return data->sint32Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetSint32(JamEasySave* easySave, const char* key, sint32 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->sint32Val = val;
			easySave->data[i]->type = dt_Sint32Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint16 jamEasySaveGetSint16(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Sint16Val, &data))
			return data->sint16Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetSint16(JamEasySave* easySave, const char* key, sint16 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->sint16Val = val;
			easySave->data[i]->type = dt_Sint16Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

sint8 jamEasySaveGetSint8(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_Sint8Val, &data))
			return data->sint8Val;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetSint8(JamEasySave* easySave, const char* key, sint8 val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->sint8Val = val;
			easySave->data[i]->type = dt_Sint8Val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

const char* jamEasySaveGetString(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_StringVal, &data))
			return data->stringVal;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}

	return 0;
}

void jamEasySaveSetString(JamEasySave* easySave, const char* key, const char* val) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->stringVal = copyString(val);
			easySave->data[i]->type = dt_StringVal;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void* jamEasySaveGetData(JamEasySave* easySave, const char* key) {
	if (easySave != NULL) {
		_JamEasyData* data;
		if (_jamEasySaveFindAssert(easySave, key, dt_BytesVal, &data))
			return data->data;
	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveSetData(JamEasySave* easySave, const char* key, void* data, uint32 size) {
	if (easySave != NULL) {
		int i = _jamEasySaveFindDataSpot(easySave, key);
		if (i != -1) {
			easySave->data[i]->data = malloc(size);
			memcpy(easySave->data[i]->data, data, size);
			easySave->data[i]->type = dt_BytesVal;
		}
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
			fwrite(&easySave->size, 4, 1, output);

			for (i = 0; i < easySave->size; i++) {
				strLen = (uint16)(strlen(easySave->data[i]->key) + 1);
				fwrite(&strLen, 2, 1, output);
				fwrite(&easySave->data[i]->type, 4, 1, output);
				if (easySave->data[i]->type == dt_StringVal) {
					uint32 len = (uint32)strlen(easySave->data[i]->stringVal) + 1;
					fwrite(&len, 4, 1, output);
				} else {
					fwrite(&easySave->data[i]->size, 4, 1, output);
				}
				fwrite(easySave->data[i]->key, strLen, 1, output);

				if (easySave->data[i]->type == dt_DoubleVal) {
					fwrite(&easySave->data[i]->doubleVal, 8, 1, output);
				} else if (easySave->data[i]->type == dt_Uint64Val) {
					fwrite(&easySave->data[i]->uint64Val, 8, 1, output);
				} else if (easySave->data[i]->type == dt_Uint32Val) {
					fwrite(&easySave->data[i]->uint32Val, 4, 1, output);
				} else if (easySave->data[i]->type == dt_Uint16Val) {
					fwrite(&easySave->data[i]->uint16Val, 2, 1, output);
				} else if (easySave->data[i]->type == dt_Uint8Val) {
					fwrite(&easySave->data[i]->uint8Val, 1, 1, output);
				} else if (easySave->data[i]->type == dt_Sint64Val) {
					fwrite(&easySave->data[i]->sint64Val, 8, 1, output);
				} else if (easySave->data[i]->type == dt_Sint32Val) {
					fwrite(&easySave->data[i]->sint32Val, 4, 1, output);
				} else if (easySave->data[i]->type == dt_Sint16Val) {
					fwrite(&easySave->data[i]->sint16Val, 2, 1, output);
				} else if (easySave->data[i]->type == dt_Sint8Val) {
					fwrite(&easySave->data[i]->sint8Val, 1, 1, output);
				} else if (easySave->data[i]->type == dt_StringVal) {
					fwrite(easySave->data[i]->stringVal, strlen(easySave->data[i]->stringVal) + 1, 1, output);
				} else if (easySave->data[i]->type == dt_BytesVal) {
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
		for (i = 0; i < easySave->size; i++)
			_jamEasyDataFree(easySave->data[i]);

		// Free the vector and save
		free(easySave->data);
		free((void*)easySave->filename);
		free(easySave);
	}
}