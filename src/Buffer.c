//////////////////////////////////////////////////////////
// Defines the buffer struct
//
// Copyright (c) Paolo Mazzon
// All rights reserved
//////////////////////////////////////////////////////////

#include "Buffer.h"
#include <malloc.h>
#include <stdio.h>

////////////////////////////////////////////////
Buffer* createBuffer(uint64 size) {
	// Create the buffer and internal buffer
	char* internalBuffer = (char*)malloc((size_t)size);
	Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));

	// Check them
	if (internalBuffer == NULL || buffer == NULL) {
		fprintf(stderr, "Failed to allocate one of the buffers.\n");

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
Buffer* loadBuffer(const char* filename) {
	FILE* bufferFile = fopen(filename, "rb");
	uint64 bufferSize = 0;
	Buffer* returnBuffer = NULL;

	// Make sure the file actually opened
	if (ferror(bufferFile) == 0) {

		// This just finds out how large the file is
		while (!feof(bufferFile)) {
			bufferSize++;
			fgetc(bufferFile);
		}

		// Move back to the start
		rewind(bufferFile);

		// Create a buffer of that size then fill it
		returnBuffer = createBuffer(bufferSize);
		fread((uint64*)returnBuffer->buffer, 1, bufferSize, bufferFile);

		// ERRRRRRRORRRRRRR CHEQUES
		if (ferror(bufferFile) != 0) {
			freeBuffer(returnBuffer);
			returnBuffer = NULL;
			fprintf(stderr, "Failed to read buffer from file '%s'\n", filename);
		}
	} else {
		fprintf(stderr, "Failed to open file '%s'\n", filename);
	}

	fclose(bufferFile);
	return returnBuffer;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void freeBuffer(Buffer* buffer) {
	if (buffer != NULL) {
		if (buffer->buffer != NULL)
			free(buffer->buffer);
		free(buffer);
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool resizeBuffer(Buffer* buffer, uint64 newSize) {
	char* newBuffer = (char*)realloc((void*)buffer->buffer, (size_t)newSize);

	// CHECKS
	if (newBuffer == NULL) {
		return false;
	} else {
		buffer->buffer = newBuffer;
		buffer->size = newSize;
		buffer->pointer = 0;
	}

	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void zeroBuffer(Buffer* buffer) {
	// Loop through the whole buffer and set it to zero
	uint64 i;
	for (i = 0; i < buffer->size; i++)
		buffer->buffer[i] = 0;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool addByte1(Buffer* buffer, uint8 byte) {
	// Run some checks
	if (buffer->pointer + 1 > buffer->size)
		return false;

	// Place the actual byte
	buffer->buffer[buffer->pointer] = (uint8)(byte);

	// Displace the pointer
	buffer->pointer += 1;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool addByte2(Buffer* buffer, uint16 bytes) {
	// Run some checks
	if (buffer->pointer + 2 > buffer->size)
		return false;

	// Place the actual bytes
	buffer->buffer[buffer->pointer] = (uint8)(bytes & 255);
	buffer->buffer[buffer->pointer + 1] = (uint8)((bytes >> 8) & 255);

	// Displace the pointer
	buffer->pointer += 2;
	return true;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
bool addByte4(Buffer* buffer, uint32 bytes) {
	// Run some checks
	if (buffer->pointer + 4 > buffer->size)
		return false;

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
bool addByte8(Buffer* buffer, uint64 bytes) {
	// Run some checks
	if (buffer->pointer + 8 > buffer->size)
		return false;

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
uint8 readByte1(Buffer* buffer, uint8 defaultReturn) {
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
uint16 readByte2(Buffer* buffer, uint16 defaultReturn) {
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
uint32 readByte4(Buffer* buffer, uint32 defaultReturn) {
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
uint64 readByte8(Buffer* buffer, uint64 defaultReturn) {
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
