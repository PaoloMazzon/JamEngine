#include "Buffer.h"
#include <malloc.h>
#include <stdio.h>
#include <JamError.h>
#include <string.h>

////////////////////////////////////////////////
JamBuffer* jamBufferCreate(uint64 size) {
	// Create the buffer and internal buffer
	uint8* internalBuffer = (uint8*)malloc((size_t)size);
	JamBuffer* buffer = (JamBuffer*)malloc(sizeof(JamBuffer));

	// Check them
	if (internalBuffer == NULL || buffer == NULL) {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate one of the buffers.\n");
		free(buffer);
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
JamBuffer* jamBufferLoad(const char *filename) {
	FILE* bufferFile = fopen(filename, "rb");
	uint64 bufferSize = 0;
	JamBuffer* returnBuffer = jamBufferCreate(0);

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
		jamBufferResize(returnBuffer, bufferSize);
		if (returnBuffer->size == bufferSize) {
			// Fill the buffer
			fread((uint64*)returnBuffer->buffer, 1, bufferSize, bufferFile);

			if (ferror(bufferFile) != 0) {
				jamBufferFree(returnBuffer);
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
void jamBufferSave(JamBuffer *buffer, const char *filename) {
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
void jamBufferFree(JamBuffer *buffer) {
	if (buffer != NULL) {
		if (buffer->buffer != NULL)
			free(buffer->buffer);
		free(buffer);
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferResize(JamBuffer *buffer, uint64 newSize) {
	uint8* newBuffer;
	if (buffer != NULL) {
		newBuffer = (uint8*)realloc(buffer->buffer, newSize);
		if (newBuffer != NULL) {
			buffer->buffer = newBuffer;
			buffer->size = newSize;
			buffer->pointer = 0;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to resize buffer");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferSeek(JamBuffer* buffer, uint64 position) {
	if (buffer != NULL && position <= buffer->size) {
		buffer->pointer = position;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Attempt to seek out of bounds %i", position);
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferZero(JamBuffer *buffer) {
	uint64 i;
	if (buffer != NULL) {
		for (i = 0; i < buffer->size; i++)
			buffer->buffer[i] = 0;
	} else {
		jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferAddByte1(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 1 <= buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, 1);
		buffer->pointer += 1;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamAddByte2(JamBuffer *buffer, void* data) {
	if (buffer != NULL && buffer->pointer + 2 <= buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, 2);
		buffer->pointer += 2;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferAddByte4(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 4 <= buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, 4);
		buffer->pointer += 4;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferAddByte8(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 8 <= buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, 8);
		buffer->pointer += 8;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferAddByteX(JamBuffer *buffer, void *data, uint32 size) {
	if (buffer != NULL && buffer->pointer + size <= buffer->size) {
		memcpy(&buffer->buffer[buffer->pointer], data, size);
		buffer->pointer += size;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferReadByte1(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 1 <= buffer->size) {
		memcpy(data, &buffer->buffer[buffer->pointer], 1);
		buffer->pointer += 1;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferReadByte2(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 2 <= buffer->size) {
		memcpy(data, &buffer->buffer[buffer->pointer], 2);
		buffer->pointer += 2;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferReadByte4(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 4 <= buffer->size) {
		memcpy(data, &buffer->buffer[buffer->pointer], 4);
		buffer->pointer += 4;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferReadByte8(JamBuffer *buffer, void *data) {
	if (buffer != NULL && buffer->pointer + 8 <= buffer->size) {
		memcpy(data, &buffer->buffer[buffer->pointer], 8);
		buffer->pointer += 8;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

////////////////////////////////////////////////
void jamBufferReadByteX(JamBuffer *buffer, void *data, uint32 size) {
	if (buffer != NULL && buffer->pointer + size <= buffer->size) {
		memcpy(data, &buffer->buffer[buffer->pointer], size);
		buffer->pointer += size;
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer doesn't exist");
		else
			jSetError(ERROR_OUT_OF_BOUNDS, "Buffer is too small to add byte(s)");
	}
}
////////////////////////////////////////////////

