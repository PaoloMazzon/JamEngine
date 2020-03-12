# Usual CMake stuff
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./)
project(JamEngine C)
set(CMAKE_C_STANDARD 11)

# Stuff relative to the project
file(GLOB C_FILES ../src/*.c)
file(GLOB H_FILES ../include/*.h)
file(GLOB TMX_FILES ../tmx/*)
include_directories(../include)
include_directories(../tmx)

# This is entirely so CLion will stop complaining
if (WIN32)
	include_directories(../include C:/SDL/include C:/libs/libXML2/include/libxml2 C:/libs/OpenAL/include C:/libs/ALUT/include)
elseif (UNIX)
	find_package(LibXml2 REQUIRED)
	find_package(OpenAL REQUIRED)
	find_package(ALUT REQUIRED)
	find_package(Freetype REQUIRED)
	include_directories(../include /usr/include/SDL2 ${LibXml2_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIRS})
endif()

add_library(${PROJECT_NAME} ${C_FILES} ${H_FILES} ${TMX_FILES})

if (WIN32)
	target_link_libraries(${PROJECT_NAME} C:/libs/libXML2/lib/libxml2.lib C:/libs/libXML2/lib/libxml2_a.lib C:/libs/libXML2/lib/libxml2_a_dll.lib C:/libs/zlib/dll32/zlibwapi.lib C:/SDL/lib/x86/SDL2main.lib C:/SDL/lib/x86/SDL2.lib C:/SDL/lib/x86/SDL2_image.lib C:/libs/OpenAL/libs/Win32/OpenAL32.lib C:/libs/ALUT/lib/alut.lib)
elseif (UNIX)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	target_link_libraries(${PROJECT_NAME} m ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})
endif()
