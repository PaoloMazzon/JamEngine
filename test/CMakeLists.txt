# Usual CMake stuff
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
project(JamEngineTest C)
set(CMAKE_C_STANDARD 11)
include_directories(../include ../Vulkan2D/)

find_package(LibXml2 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ALUT REQUIRED)
find_package(Freetype REQUIRED)
find_package(SDL2 REQUIRED)

if ($ENV{CLION_IDE})
	set(JAM_LIB ${CMAKE_SOURCE_DIR}/cmake-build-debug/lib/libJamEngine.a)
else()
	set(JAM_LIB ${CMAKE_SOURCE_DIR}/lib/libJamEngine.a)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(${PROJECT_NAME} test.c)
target_link_libraries(${PROJECT_NAME} ${JAM_LIB} m ${SDL2_LIBRARY} ${ALUT_LIBRARY} ${LIBXML2_LIBRARIES} ${OPENAL_LIBRARY} ${FREETYPE_LIBRARIES})