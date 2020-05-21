# Usual CMake stuff
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./ ${CMAKE_CURRENT_SOURCE_DIR})
project(JamEngine C)
set(CMAKE_C_STANDARD 11)

# Stuff relative to the project
file(GLOB C_FILES ../src/*.c)
file(GLOB H_FILES ../include/*.h)
file(GLOB TMX_FILES ../tmx/*)
include_directories(../include)
include_directories(../tmx)

find_package(LibXml2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALUT REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(../include ${SDL2_INCLUDE_DIR} ${LibXml2_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})

add_library(${PROJECT_NAME} ${C_FILES} ${H_FILES} ${TMX_FILES} ../include/EasySave.h ../src/EasySave.c)

target_link_libraries(${PROJECT_NAME} m ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${FREETYPE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY})
