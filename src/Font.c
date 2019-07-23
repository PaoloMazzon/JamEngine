//////////////////////////////////////////////////////////
// Defines Font.h
//////////////////////////////////////////////////////////

#include "Font.h"
#include <SDL_image.h>
#include <malloc.h>
#include <stdio.h>
#include <Renderer.h>
#include "StringUtil.h"

///////////////////////////////////////////////////////
SDL_Texture* loadTex(SDL_Renderer* renderer, const char* fname) {
	// Load the actual image
	SDL_Surface* image = IMG_Load(fname);
	SDL_Texture* tex;

	// Check it
	if (image == NULL) {
		fprintf(stderr, "Failed to load image, SDL error: %s\n", SDL_GetError());
		tex = NULL;
	} else {
		tex = SDL_CreateTextureFromSurface(renderer, image);
		SDL_FreeSurface(image);

		// More checks
		if (tex == NULL) {
			fprintf(stderr, "Failed to convert texture, SDL error: %s\n", SDL_GetError());
		}
	}

	return tex;
}
///////////////////////////////////////////////////////

/////////////////////////////////////////////////////
Font* createFont(Renderer* renderer, const char* latinFname, const char* fontFname) {
	Font* font = (Font*)calloc(1, sizeof(Font));
	SDL_Texture* tex = NULL;

	// Check it
	if (font == NULL) {
		fprintf(stderr, "Failed to allocate font struct.\n");
	} else {
		// Load latin font
		SDL_Texture* latin = loadTex(renderer->internalRenderer, latinFname);

		// Some more checks
		if (latin == NULL) {
			freeFont(font);
			fprintf(stderr, "Failed to load latin font texture, SDL error: %s\n", SDL_GetError());
			font = NULL;
		} else {
			// Set up the font things
			font->latin = latin;
			font->uStart = 0;
			font->uEnd = 255;
			font->characterWidth = 0;
			font->characterHeight = 0;
			font->font = NULL;
			SDL_QueryTexture(latin, NULL, NULL, (int*)&font->latinWidth, (int*)&font->latinHeight);

			if (fontFname != NULL) {
				// Load the texture
				tex = loadTex(renderer->internalRenderer, fontFname);

				if (tex == NULL) {
					freeFont(font);
					fprintf(stderr, "Failed to load font texture, SDL error: %s\n", SDL_GetError());
					font = NULL;
				} else {
					font->font = tex;
					SDL_QueryTexture(tex, NULL, NULL, (int*)&font->fontWidth, (int*)&font->fontHeight);
				}
			}
		}
	}

	return font;
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void freeFont(Font* font) {
	if (font != NULL) {
		if (font->font != NULL)
			SDL_DestroyTexture(font->font);
		if (font->latin != NULL) {
			SDL_DestroyTexture(font->latin);
		}
		free(font);
	}
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////

///////// UTF-8 checking functions /////////
bool isContinuationByte(uint8 byte) {
	return ((byte & 128) == 128 && (byte & 64) == 0);
}
bool is1ByteCharacter(uint8 byte) {
	return ((byte & 128) == 0);
}
bool is2ByteCharacter(uint8 byte) {
	return ((byte & 128) == 128 && (byte & 64) == 64 && (byte & 32) == 0);
}
bool is3ByteCharacter(uint8 byte) {
	return ((byte & 128) == 128 && (byte & 64) == 64 && (byte & 32) == 32 && (byte & 16) == 0);
}
bool is4ByteCharacter(uint8 byte) {
	return ((byte & 128) == 128 && (byte & 64) == 64 && (byte & 32) == 32 && (byte & 16) == 16 && (byte & 8) == 0);
}

void renderFont(int x, int y, const char* string, Font* font, Renderer* renderer) {
	uint32 unichar;
	uint32 tempChar;
	bool ready = false;
	uint8 bytesLeft = 0;
	int i = 0;
	uint16 wh = font->latinWidth / font->characterWidth;
	uint16 uwh = font->fontWidth / font->characterWidth;
	SDL_Rect charPlace;
	SDL_Rect charSheetBox;

	// Set up the boxes
	charSheetBox.x = 0;
	charSheetBox.y = 0;
	charSheetBox.w = font->characterWidth;
	charSheetBox.h = font->characterHeight;
	charPlace.x = x;
	charPlace.y = y;
	charPlace.w = font->characterWidth;
	charPlace.h = font->characterHeight;

	// Loop through the whole string
	while (string[i] != 0) {
		// UTF-8 bit checks
		if (isContinuationByte(string[i]) && bytesLeft > 0) {
			bytesLeft--;
			unichar |= string[i] & 63;

			// Weather there is another byte or we are done
			if (bytesLeft == 0)
				ready = true;
			else
				unichar = unichar << 6;
		} else if (is1ByteCharacter(string[i]) && bytesLeft == 0) {
			// Just ascii, we are ready
			unichar = string[i];
			ready = true;
		} else if (is2ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another byte is coming
			bytesLeft = 1;

			// Set up the uchar
			unichar = (string[i] & 31) << 6;
		} else if (is3ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another 2 bytes are coming
			bytesLeft = 2;

			// Set up the uchar
			unichar = (string[i] & 15) << 6;
		} else if (is4ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another 3 bytes are coming
			bytesLeft = 3;

			// Set up the uchar
			unichar = (string[i] & 7) << 6;
		} else {
			// Encoding is wrong, restart
			unichar = 0;
			ready = false;
			bytesLeft = 0;
		}

		// Ready to draw the character
		if (ready) {
			ready = false;

			// It is a latin character
			if (unichar != 13) {
				if (unichar < 255) {
					// Locate the character
					charSheetBox.x = (unichar - wh * (unichar / wh)) * font->characterWidth;
					charSheetBox.y = unichar / wh * font->characterHeight;

					// Print the character
					SDL_RenderCopy(renderer->internalRenderer, font->latin, &charSheetBox, &charPlace);
				} else if (unichar >= font->uStart && unichar <= font->uEnd) { // It is the unicode part
					tempChar = unichar - font->uStart;

					// Locate the character
					charSheetBox.x = (tempChar - uwh * (tempChar / uwh)) * font->characterWidth;
					charSheetBox.y = tempChar / uwh * font->characterHeight;

					// Print the character
					SDL_RenderCopy(renderer->internalRenderer, font->font, &charSheetBox, &charPlace);
				} else { // Not in the font
					fprintf(stderr, "Error: Character '%u' is out of the font's range.\n", unichar);
				}
			} else {
				charPlace.y += font->characterHeight;
				charPlace.x = x;
			}

			// Move the text over
			charPlace.x += font->characterWidth;
		}

		i++;
	}
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void renderFontExt(int x, int y, const char* string, Font* font, Renderer* renderer, int w, ...) {
	// Variable length parameter things
	va_list params;
	va_start(params, w);

	uint32 unichar = 0; // The final unicode character once UTF-8 is processed
	uint32 tempChar = 0;
	uint32 lastunichar = 0; // The last unicode character
	bool readyToProcessCharacter = false; // Done processing UTF-8 or not
	const char* currentString = string; // Useful in case we're in a different string
	char* currentBuffer = NULL; // For when we make temporary strings for doubles and such
	bool continueRendering = true; // Since there can be two strings at once we need a more advanced method of counting than i
	int i = 0; // Counter for the base string
	int j = 0; // Counter for any extra string
	int *currentIterator = &i; // The iterator for whichever string we're in
	bool insideParameterString = false;
	uint8 bytesLeft = 0;
	bool renderCurrentChar = true;

	// The boxes and settings for rendering the font
	uint16 wh = font->latinWidth / font->characterWidth;
	uint16 uwh = font->fontWidth / font->characterWidth;
	SDL_Rect charPlace;
	SDL_Rect charSheetBox;

	// Set up the boxes
	charSheetBox.x = 0;
	charSheetBox.y = 0;
	charSheetBox.w = font->characterWidth;
	charSheetBox.h = font->characterHeight;
	charPlace.x = x;
	charPlace.y = y;
	charPlace.w = font->characterWidth;
	charPlace.h = font->characterHeight;

	if (renderer != NULL && font != NULL) {
		while (continueRendering) {
			// UTF-8 bit checks
			if (isContinuationByte(currentString[*currentIterator]) && bytesLeft > 0) {
				bytesLeft--;
				unichar |= currentString[*currentIterator] & 63;

				// Weather there is another byte or we are done
				if (bytesLeft == 0)
					readyToProcessCharacter = true;
				else
					unichar = unichar << 6;
			} else if (is1ByteCharacter(currentString[*currentIterator]) && bytesLeft == 0) {
				// Just ascii, we are ready
				unichar = currentString[*currentIterator];
				readyToProcessCharacter = true;
			} else if (is2ByteCharacter(currentString[*currentIterator]) && bytesLeft == 0) {
				// Another byte is coming
				bytesLeft = 1;

				// Set up the uchar
				unichar = (currentString[*currentIterator] & 31) << 6;
			} else if (is3ByteCharacter(currentString[*currentIterator]) && bytesLeft == 0) {
				// Another 2 bytes are coming
				bytesLeft = 2;

				// Set up the uchar
				unichar = (currentString[*currentIterator] & 15) << 6;
			} else if (is4ByteCharacter(currentString[*currentIterator]) && bytesLeft == 0) {
				// Another 3 bytes are coming
				bytesLeft = 3;

				// Set up the uchar
				unichar = (currentString[*currentIterator] & 7) << 6;
			} else {
				// Encoding is wrong, restart
				unichar = 0;
				readyToProcessCharacter = false;
				bytesLeft = 0;
			}


			// Once we ready for the whole character
			/*
			 * 1. \ was last character
			 * 2. % was last character
			 * 3. if neither of the above, current character is not % or \
			 */
			if (readyToProcessCharacter) {
				renderCurrentChar = true;
				// Escapes
				if ((lastunichar == '\\' && !insideParameterString) || unichar == 10) {
					renderCurrentChar = false;
					if (unichar == 'n' || unichar == 10) {
						charPlace.y += font->characterHeight;
						charPlace.x = x - font->characterWidth;
					} else if (unichar == '%') {
						renderCurrentChar = true;
					}
				} else if (lastunichar == '%' && !insideParameterString) {
					renderCurrentChar = false;
					if (unichar == 's') { // Strings
						j = -1;
						i++;
						currentIterator = &j;
						currentString = va_arg(params, const char*);
					} else if (unichar == 'c') { // Character
						unichar = (uint32)va_arg(params, int);
						renderCurrentChar = true;
					} else if (unichar == 'f') { // Double
						currentBuffer = ftoa(va_arg(params, double));
						if (currentBuffer != NULL) {
							currentIterator = &j;
							j = -1;
							i++;
							currentString = currentBuffer;
						}
					}
				}
				if (unichar != '\\' && unichar != '%' && renderCurrentChar) {
					// It is a latin character
					if (unichar < 255) {
						// Locate the character
						charSheetBox.x = (unichar - wh * (unichar / wh)) * font->characterWidth;
						charSheetBox.y = unichar / wh * font->characterHeight;

						// Print the character
						SDL_RenderCopy(renderer->internalRenderer, font->latin, &charSheetBox, &charPlace);
					} else if (unichar >= font->uStart && unichar <= font->uEnd) { // It is the unicode part
						tempChar = unichar - font->uStart;

						// Locate the character
						charSheetBox.x = (tempChar - uwh * (tempChar / uwh)) * font->characterWidth;
						charSheetBox.y = tempChar / uwh * font->characterHeight;

						// Print the character
						SDL_RenderCopy(renderer->internalRenderer, font->font, &charSheetBox, &charPlace);
					} else { // Not in the font
						fprintf(stderr, "Error: Character '%u' is out of the font's range. (renderFontExt)\n",
								unichar);
					}

					// Check for text width
					if (charPlace.x - x + font->characterWidth > w && w != -1) {
						charPlace.y += font->characterHeight;
						charPlace.x = x;
					} else {
						charPlace.x += font->characterWidth;
					}
				}
			}

			// We're done so store the old character
			if (!insideParameterString)
				lastunichar = unichar;
			*currentIterator = *currentIterator + 1;

			// Check if we're done rendering
			if (currentString[*currentIterator] == 0) {
				// Inserted string; not the base
				if (currentIterator == &j) {
					free(currentBuffer);
					currentBuffer = NULL;
					currentIterator = &i;
					currentString = string;
					insideParameterString = false;

					// Check if we're at the end of the base string
					if (currentString[*currentIterator] == 0)
						continueRendering = false;
				} else { // The base string
					continueRendering = false;
				}
			}

			readyToProcessCharacter = false;
		}
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Renderer does not exist (renderFontExt)\n");
		if (font == NULL)
			fprintf(stderr, "Font does not exist (renderFontExt)\n");
	}
	va_end(params);
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
void renderFontWrap(int x, int y, uint16 w, const char* string, Font* font, Renderer* renderer) {
	uint32 unichar;
	uint32 tempChar;
	bool ready = false;
	uint8 bytesLeft = 0;
	int i = 0;
	uint16 wh = font->latinWidth / font->characterWidth;
	uint16 uwh = font->fontWidth / font->characterWidth;
	SDL_Rect charPlace;
	SDL_Rect charSheetBox;

	// Set up the boxes
	charSheetBox.x = 0;
	charSheetBox.y = 0;
	charSheetBox.w = font->characterWidth;
	charSheetBox.h = font->characterHeight;
	charPlace.x = x;
	charPlace.y = y;
	charPlace.w = font->characterWidth;
	charPlace.h = font->characterHeight;

	// Loop through the whole string
	while (string[i] != 0) {
		// UTF-8 bit checks
		if (isContinuationByte(string[i]) && bytesLeft > 0) {
			bytesLeft--;
			unichar |= string[i] & 63;

			// Weather there is another byte or we are done
			if (bytesLeft == 0)
				ready = true;
			else
				unichar = unichar << 6;
		} else if (is1ByteCharacter(string[i]) && bytesLeft == 0) {
			// Just ascii, we are ready
			unichar = string[i];
			ready = true;
		} else if (is2ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another byte is coming
			bytesLeft = 1;

			// Set up the uchar
			unichar = (string[i] & 31) << 6;
		} else if (is3ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another 2 bytes are coming
			bytesLeft = 2;

			// Set up the uchar
			unichar = (string[i] & 15) << 6;
		} else if (is4ByteCharacter(string[i]) && bytesLeft == 0) {
			// Another 3 bytes are coming
			bytesLeft = 3;

			// Set up the uchar
			unichar = (string[i] & 7) << 6;
		} else {
			// Encoding is wrong, restart
			unichar = 0;
			ready = false;
			bytesLeft = 0;
		}

		// Ready to draw the character
		if (ready) {
			ready = false;

			// It is a latin character
			if (unichar != 13) {
				if (unichar < 255) {
					// Locate the character
					charSheetBox.x = (unichar - wh * (unichar / wh)) * font->characterWidth;
					charSheetBox.y = unichar / wh * font->characterHeight;

					// Print the character
					SDL_RenderCopy(renderer->internalRenderer, font->latin, &charSheetBox, &charPlace);
				} else if (unichar >= font->uStart && unichar <= font->uEnd) { // It is the unicode part
					tempChar = unichar - font->uStart;

					// Locate the character
					charSheetBox.x = (tempChar - uwh * (tempChar / uwh)) * font->characterWidth;
					charSheetBox.y = tempChar / uwh * font->characterHeight;

					// Print the character
					SDL_RenderCopy(renderer->internalRenderer, font->font, &charSheetBox, &charPlace);
				} else { // Not in the font
					fprintf(stderr, "Error: Character '%u' is out of the font's range.\n", unichar);
				}
			} else {
				charPlace.y += font->characterHeight;
				charPlace.x = 0;
			}

			// Move the text over
			if (font->characterWidth + charPlace.x - x >= w) {
				charPlace.y += font->characterHeight;
				charPlace.x = 0;
			} else {
				charPlace.x += font->characterWidth;
			}
		}

		i++;
	}
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
uint16 getStringWidth(Font* font, const char* string) {
	int i = 0;
	uint16 lw = 0;
	uint16 w = 0;

	while (string[i] != 0) {
		// Newline checks
		if (string[i] == '\n') {
			if (w > lw)
				lw = w;
			w = 0;
		}

		// Just look for actual characters
		if (string[i] != '\n' && (is1ByteCharacter(string[i]) || is2ByteCharacter(string[i]) || is3ByteCharacter(string[i]) || is4ByteCharacter(string[i]))) {
			w += font->characterWidth;
		}
		i++;
	}
	if (w > lw)
		lw = w;

	return lw;
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
uint16 getStringHeight(Font* font, const char* string) {
	int i = 0;
	uint16 h = font->characterHeight;

	while (string[i] != 0) {
		// Just look for newlines
		if (string[i] == '\n') {
			h += font->characterHeight;
		}
		i++;
	}

	return h;
}
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
uint16 getStringHeightWrap(Font* font, const char* string, uint16 w) {
	int i = 0;
	uint16 h = font->characterHeight;
	uint16 width = 0;

	while (string[i] != 0) {
		// Width
		if (string[i] != '\n' && (is1ByteCharacter(string[i]) || is2ByteCharacter(string[i]) || is3ByteCharacter(string[i]) || is4ByteCharacter(string[i]))) {
			width += font->characterWidth;
		}

		// Newline check
		if (string[i] == '\n') {
			h += font->characterHeight;
			width = 0;
		}

		// Wrap check
		if (width >= w) {
			width = 0;
			h += font->characterHeight;
		}
		i++;
	}

	return h;
}
/////////////////////////////////////////////////////
