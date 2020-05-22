//
// Created by lugi1 on 2020-05-20.
//

#include "EasySave.h"
#include <JamError.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <EasySave.h>

////////////////// Helper functions //////////////////
const char* copyString(const char* string) {
	char* str = malloc(strlen(string) + 1);

	if (str != NULL)
		strcpy(str, string);
	else
		jSetError(ERROR_ALLOC_FAILED, "Failed to copy string %s", string);

	return str;
}

_JamEasyData* _jamEasyDataCreate(const char* name, _JamEasyDataType type) {
	_JamEasyData* data = malloc(sizeof(_JamEasyData));

	if (data != NULL) {
		data->type = type;
		data->key = copyString(name);
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create data of name %s", name);
	}

	return data;
}

void _jamEasyDataFree(_JamEasyData* data) {
	if (data != NULL) {
		free((void*)data->key);
		if (data->type == bytesVal)
			free(data->data);
	}
}
//////////////////////////////////////////////////////

// TODO: This file

JamEasySave* jamEasySaveLoad(const char* filename) {
	 return NULL;
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

	} else {
		jSetError(ERROR_NULL_POINTER, "Easy save doesn't exist");
	}
}

void jamEasySaveClose(JamEasySave* easySave) {
	if (easySave != NULL) {

	}
}