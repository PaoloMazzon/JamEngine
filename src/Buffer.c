#include "Buffer.h"
#include <malloc.h>
#include <stdio.h>
#include <JamError.h>

////////////////////////////////////////////////
JamBuffer* jamCreateBuffer(uint64 size) {
	// Create the buffer and internal buffer
	uint8* internalBuffer = (uint8*)malloc((size_t)size);
	JamBuffer* buffer = (JamBuffer*)malloc(sizeof(JamBuffer));

	// Check them
	if (internalBuffer == NULL || buffer == NULL) {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate one of the buffers.\n");

		if (buffer != NULL)
			free(buffer);
		if (internalBuffer != NULL)
			free(internalBuffer);
	} else {
		// Assign buffer things
		buffer->buffer = internalBuffer;
		buffer->size = size;
		buffer->pointer = 0;
	}

	return buffer;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
JamBuffer* jamLoadBuffer(const char *filename) {
	FILE* bufferFile = fopen(filename, "rb");
	uint64 bufferSize = 0;
	JamBuffer* returnBuffer = NULL;

	// Make sure the file actually opened
	if (bufferFile != NULL) {

		// This just finds out how large the file is
		while (!feof(bufferFile)) {
			bufferSize++;
			fgetc(bufferFile);
		}

		// Move back to the start
		rewind(bufferFile);

		// Create a buffer of that size then fill it
		returnBuffer = jamCreateBuffer(bufferSize);
		fread((uint64*)returnBuffer->buffer, 1, bufferSize, bufferFile);

		// ERRRRRRRORRRRRRR CHEQUES
		if (ferror(bufferFile) != 0) {
			jamFreeBuffer(returnBuffer);
			returnBuffer = NULL;
			jSetError(ERROR_FILE_FAILED, "Failed to read buffer from file '%s'\n", filename);
		}
		fclose(bufferFile);
	} else {
		jSetError(ERROR_OPEN_FAILED, "Failed to open file '%s'\n", filename);
	}

	return returnBuffer;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamFreeBuffer(JamBuffer *buffer) {
	if (buffer != NULL) {
		if (buffer->buffer != NULL)
			free(buffer->buffer);
		free(buffer);
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool jamResizeBuffer(JamBuffer *buffer, uint64 newSize) {
	uint8* newBuffer = (uint8*)realloc((void*)buffer->buffer, (size_t)newSize);
	bool ret = true;

	// CHECKS
	if (newBuffer == NULL) {
		ret = false;
		jSetError(ERROR_REALLOC_FAILED, "Failed to create new buffer (jamResizeBuffer)");
	} else {
		buffer->buffer = newBuffer;
		buffer->size = newSize;
		buffer->pointer = 0;
	}

	return ret;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamZeroBuffer(JamBuffer *buffer) {
	// Loop through the whole buffer and set it to zero
	uint64 i;
	for (i = 0; i < buffer->size; i++)
		buffer->buffer[i] = 0;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool jamAddByte1(JamBuffer *buffer, uint8 byte) {
	// Run some checks
	if (buffer->pointer + 1 > buffer->size) {
		jSetError(ERROR_OUT_OF_BOUNDS, "");
		return false;
	}

	// Place the actual byte
	buffer->buffer[buffer->pointer] = (uint8)(byte);

	// Displace the pointer
	buffer->pointer += 1;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool jamAddByte2(JamBuffer *buffer, uint16 bytes) {
	// Run some checks
	if (buffer->pointer + 2 > buffer->size) {
		jSetError(ERROR_OUT_OF_BOUNDS, "");
		return false;
	}

	// Place the actual bytes
	buffer->buffer[buffer->pointer] = (uint8)(bytes & 255);
	buffer->buffer[buffer->pointer + 1] = (uint8)((bytes >> 8) & 255);

	// Displace the pointer
	buffer->pointer += 2;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool jamAddByte4(JamBuffer *buffer, uint32 bytes) {
	// Run some checks
	if (buffer->pointer + 4 > buffer->size) {
		jSetError(ERROR_OUT_OF_BOUNDS, "");
		return false;
	}

	// Place the actual bytes
	buffer->buffer[buffer->pointer] = (uint8)(bytes & 255);
	buffer->buffer[buffer->pointer + 1] = (uint8)((bytes >> 8) & 255);
	buffer->buffer[buffer->pointer + 2] = (uint8)((bytes >> 16) & 255);
	buffer->buffer[buffer->pointer + 3] = (uint8)((bytes >> 24) & 255);

	// Displace the pointer
	buffer->pointer += 4;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool jamAddByte8(JamBuffer *buffer, uint64 bytes) {
	// Run some checks
	if (buffer->pointer + 8 > buffer->size) {
		jSetError(ERROR_OUT_OF_BOUNDS, "");
		return false;
	}

	// Place the actual bytes
	buffer->buffer[buffer->pointer] = (uint8)(bytes & 255);
	buffer->buffer[buffer->pointer + 1] = (uint8)((bytes >> 8) & 255);
	buffer->buffer[buffer->pointer + 2] = (uint8)((bytes >> 16) & 255);
	buffer->buffer[buffer->pointer + 3] = (uint8)((bytes >> 24) & 255);
	buffer->buffer[buffer->pointer + 4] = (uint8)((bytes >> 32) & 255);
	buffer->buffer[buffer->pointer + 5] = (uint8)((bytes >> 40) & 255);
	buffer->buffer[buffer->pointer + 6] = (uint8)((bytes >> 48) & 255);
	buffer->buffer[buffer->pointer + 7] = (uint8)((bytes >> 56) & 255);

	// Displace the pointer
	buffer->pointer += 8;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
uint8 jamReadByte1(JamBuffer *buffer, uint8 defaultReturn) {
	// Checks
	if (buffer->pointer + 1 > buffer->size)
		return defaultReturn;

	// The new integer
	uint8 returnInt = 0;

	// Process it
	returnInt = buffer->buffer[buffer->pointer];

// Increase the pointer
	buffer->pointer += 1;
	return returnInt;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
uint16 jamReadByte2(JamBuffer *buffer, uint16 defaultReturn) {
	// Checks
	if (buffer->pointer + 2 > buffer->size)
		return defaultReturn;

	// The new integer
	uint16 returnInt = 0;

	// Process it
	returnInt |= (uint16)buffer->buffer[buffer->pointer + 1] << 8;
	returnInt |= (uint16)buffer->buffer[buffer->pointer];

	// Increase the pointer
	buffer->pointer += 2;
	return returnInt;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
uint32 jamReadByte4(JamBuffer *buffer, uint32 defaultReturn) {
	// Checks
	if (buffer->pointer + 4 > buffer->size)
		return defaultReturn;

	// The new integer
	uint32 returnInt = 0;

	// Process it
	returnInt |= (uint32)buffer->buffer[buffer->pointer + 3] << 24;
	returnInt |= (uint32)buffer->buffer[buffer->pointer + 2] << 16;
	returnInt |= (uint32)buffer->buffer[buffer->pointer + 1] << 8;
	returnInt |= (uint32)buffer->buffer[buffer->pointer];

	// Increase the pointer
	buffer->pointer += 4;
	return returnInt;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
uint64 jamReadByte8(JamBuffer *buffer, uint64 defaultReturn) {
	// Checks
	if (buffer->pointer + 8 > buffer->size)
		return defaultReturn;

	// The new integer
	uint64 returnInt = 0;

	// Process it
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 7] << 56;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 6] << 48;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 5] << 40;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 4] << 32;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 3] << 24;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 2] << 16;
	returnInt |= (uint64)buffer->buffer[buffer->pointer + 1] << 8;
	returnInt |= (uint64)buffer->buffer[buffer->pointer];

	// Increase the pointer
	buffer->pointer += 8;
	return returnInt;
}
////////////////////////////////////////////////
