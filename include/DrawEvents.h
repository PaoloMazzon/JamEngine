/// \file DrawEvents.h
/// \author lugi1
/// \date 2018-12-11
/// \brief This file declares things you need to be able to multi-thread with this engine
#pragma once
#include "Constants.h"
#include "Texture.h"
#include "TileMap.h"
#include "Sprite.h"

enum drawEventType {deRectangle, deSetColour, deCircle, deTexture, deSortedMap, deFillColour};

/// \brief This struct represents a single draw call, weather it be a rectangle, circle or otherwise
typedef struct {
	enum drawEventType type; ///< The type of event this is

	union {
		// Rectangle
		struct {
			int rectX;
			int rectY;
			int rectW;
			int rectH;
		};
		// Set colour
		struct {
			uint8 setColourR;
			uint8 setColourG;
			uint8 setColourB;
			uint8 setColourA;
		};
		// Circle
		struct {
			int circleX;
			int circleY;
			int circleR;
		};
		// Texture
		struct {
			Texture* texture;
			sint32 texX;
			sint32 texY;
			sint32 texOriginX;
			sint32 texOriginY;
			float texScaleX;
			float texScaleY;
			double texRot;
			uint8 texAlpha;
			sint32 texTexX;
			sint32 texTexY;
			sint32 texTexW;
			sint32 texTexH;
		};
		// Sorted Map
		struct {
			Sprite* sortedMapSPR;
			TileMap* sortedMapMAP;
			int sortedMapX;
			int sortedMapY;
			uint32 sortedMapStartingCellX;
			uint32 sortedMapStartingCellY;
		};
		// Fill colour
		struct {
			uint8 fillR;
			uint8 fillG;
			uint8 fillB;
			uint8 fillA;
		};
	};
} DrawEvent;

/// \brief A simple draw queue that can manage draw events
///
/// This queue has two separate size variables: length and capacity.
/// Length is equal to the number of things inside of the event queue,
/// while capacity is the amount of memory currently allocated for the
/// queue. This means that there will usually be a bit of redundant
/// memory lying around but it also means we're not constantly calling
/// malloc and free. If you need the memory back, just call the shrink
/// function.
///
/// Something else worthy of note is that once you pass a draw event
/// to the queue, the queue takes responsibility for it. Do not free
/// a draw event you gave to a queue for it will do that itself. This
/// also means don't pass a pointer that is not kept track of
/// elsewhere for it will not be freed by the queue.
typedef struct {
	DrawEvent** events; ///< The list of things to draw
	uint32 length; ///< The number of things in the list
	uint32 capacity; ///< The amount of memory currently allocated for the list (in elements)
} DrawQueue;

/// \brief Creates a draw queue
DrawQueue* createDrawQueue(uint32 initialCapacity);

/// \brief Appends a draw event to the list
void drawQueueAppendEvent(DrawEvent* event);

/// \brief Wipes the contents of a draw queue
void clearDrawQueue(DrawQueue* queue);

/// \brief Shrinks a draw queue's list to fit the currently used-up space
void shrinkDrawQueue(DrawQueue* queue);

/// \brief Frees a draw queue and all its events
void freeDrawQueue(DrawQueue* queue);