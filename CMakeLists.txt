# Usual CMake stuff
cmake_minimum_required(VERSION 3.10)
project(JamEngine C)
set(CMAKE_C_STANDARD 11)

# Stuff relative to the project
set(OUTPUT_LIB 0)
set(PROJ_NAME JamEngine)
set(FILES_USED src/File.c src/AssetHandler.c src/StringMap.c include/AssetHandler.h include/File.h include/StringMap.h include/Sprite.h include/Hitbox.h src/Sprite.c src/Hitbox.c src/Renderer.c include/Renderer.h include/Constants.h include/Texture.h src/Texture.c src/Input.c include/Input.h src/Drawing.c include/Drawing.h src/Clock.c include/Clock.h src/Frame.c include/Frame.h src/Vector.c include/Vector.h src/Entity.c include/Entity.h src/TileMap.c include/TileMap.h src/Font.c include/Font.h src/Buffer.c include/Buffer.h src/World.c include/World.h)

# This is entirely so CLion will stop complaining
include_directories(include C:/SDL/include)

if (OUTPUT_LIB)
	add_library(${PROJ_NAME} ${FILES_USED})
else ()
	add_executable(${PROJ_NAME} test.c ${FILES_USED})
endif()
target_link_libraries(${PROJ_NAME} C:/SDL/lib/x86/SDL2main.lib C:/SDL/lib/x86/SDL2.lib C:/SDL/lib/x86/SDL2_mixer.lib C:/SDL/lib/x86/SDL2_image.lib)