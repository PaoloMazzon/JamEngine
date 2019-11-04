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
	JamBuffer* returnBuffer = jamCreateBuffer(0);

	// Make sure the file actually opened
	if (bufferFile != NULL && returnBuffer != NULL) {

		// This just finds out how large the file is
		while (!feof(bufferFile)) {
			bufferSize++;
			fgetc(bufferFile);
		}

		// Move back to the start
		rewind(bufferFile);

		// Resize buffer to new size
		if (jamResizeBuffer(returnBuffer, bufferSize)) {
			// Fill the buffer
			fread((uint64*)returnBuffer->buffer, 1, bufferSize, bufferFile);

			if (ferror(bufferFile) != 0) {
				jamFreeBuffer(returnBuffer);
				returnBuffer = NULL;
				jSetError(ERROR_FILE_FAILED, "Failed to read buffer from file '%s'\n", filename);
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to resize buffer for file '%s'", filename);
		}
		fclose(bufferFile);
	} else if (returnBuffer == NULL) {
		jSetError(ERROR_ALLOC_FAILED, "Failed to open file '%s'\n", filename);
	}

	return returnBuffer;
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamSaveBuffer(JamBuffer* buffer, const char* filename) {
	FILE* file = fopen(filename, "wb");

	if (buffer != NULL && file != NULL) {
		fwrite(buffer->buffer, buffer->size, 1, file);
		fclose(file);
	} else {
		if (file == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer does not exist for filename \"%s\"", filename);
		if (buffer == NULL)
			jSetError(ERROR_OPEN_FAILED, "Failed to open file \"%s\"", filename);
	}
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
void jamAddByte1(JamBuffer *buffer, void* data) {
	if (buffer != NULL && buffer->pointer + 1 < buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, 1);
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamAddByte2(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamAddByte4(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamAddByte8(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamAddByteX(JamBuffer *buffer, void* data, uint32 size);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamReadByte1(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamReadByte2(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamReadByte4(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamReadByte8(JamBuffer *buffer, void* data);
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamReadByteX(JamBuffer *buffer, void* data, uint32 size);
////////////////////////////////////////////////

