# Usual CMake stuff
cmake_minimum_required(VERSION 3.10)
set(PROJ_NAME JamEngine)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
project(${PROJ_NAME} C)
set(CMAKE_C_STANDARD 11)

# Stuff relative to the project
set(OUTPUT_LIB 0)
set(FILES_USED src/File.c src/AssetHandler.c src/StringMap.c include/AssetHandler.h include/File.h include/StringMap.h include/Sprite.h include/Hitbox.h src/Sprite.c src/Hitbox.c src/Renderer.c include/Renderer.h include/Constants.h include/Texture.h src/Texture.c src/Input.c include/Input.h src/Drawing.c include/Drawing.h src/Clock.c include/Clock.h src/Frame.c include/Frame.h src/Vector.c include/Vector.h src/Entity.c include/Entity.h src/TileMap.c include/TileMap.h src/Font.c include/Font.h src/Buffer.c include/Buffer.h src/World.c include/World.h include/DrawEvents.h src/DrawEvents.c include/EntityList.h src/EntityList.c include/StringUtil.h src/StringUtil.c)

# This is entirely so CLion will stop complaining
if (WIN32)
	include_directories(include C:/SDL/include)
elseif (UNIX)
	include_directories(include /usr/include/SDL2)
endif()

if (OUTPUT_LIB)
	add_library(${PROJ_NAME} ${FILES_USED})
else ()
	add_executable(${PROJ_NAME} test.c ${FILES_USED})
endif()

if (WIN32)
	target_link_libraries(${PROJ_NAME} C:/SDL/lib/x86/SDL2main.lib C:/SDL/lib/x86/SDL2.lib C:/SDL/lib/x86/SDL2_mixer.lib C:/SDL/lib/x86/SDL2_image.lib)
elseif (UNIX)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	target_link_libraries(${PROJ_NAME} m ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${SDL2_MIXER_LIBRARIES})
endif()