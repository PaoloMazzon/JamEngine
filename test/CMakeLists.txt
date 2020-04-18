# Usual CMake stuff
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ./)
project(JamEngineTest C)
set(CMAKE_C_STANDARD 11)
include_directories(../include)

if (UNIX)
	find_package(LibXml2 REQUIRED)
	find_package(OpenAL REQUIRED)
	find_package(ALUT REQUIRED)
	find_package(Freetype REQUIRED)
endif()

add_executable(${PROJECT_NAME} test.c)

if (WIN32)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/cmake-build-debug/lib/libJamEngine.a C:/libs/Freetype/freetype.lib C:/libs/libXML2/lib/libxml2.lib C:/libs/libXML2/lib/libxml2_a.lib C:/libs/libXML2/lib/libxml2_a_dll.lib C:/libs/zlib/dll32/zlibwapi.lib C:/SDL/lib/x86/SDL2main.lib C:/SDL/lib/x86/SDL2.lib C:/SDL/lib/x86/SDL2_image.lib C:/libs/OpenAL/libs/Win32/OpenAL32.lib C:/libs/ALUT/lib/alut.lib)
elseif (UNIX)
	find_package(SDL2 REQUIRED)
	find_package(SDL2_image REQUIRED)
	target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libJamEngine.a m ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${SDL2_IMAGE_LIBRARIES} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})
endif()
