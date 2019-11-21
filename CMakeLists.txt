# File generated at : 19:09:23, Wed 06 Nov
# Converted Project : YARK_CLIENT.vcxproj
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

##################### Variables ############################
# Change if you want modify path or other values           #
############################################################

# Project
get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/YARK_CLIENT" ABSOLUTE)
set(DEPENDENCIES_DIR ${PROJECT_DIR}/dependencies)
set(PROJECT_NAME YARK-MFD)


# Outputs
set(OUTPUT_DEBUG ${PROJECT_DIR}/bin)
set(OUTPUT_RELEASE ${PROJECT_DIR}/bin)

################# CMake Project ############################
# The main options of project                              #
############################################################

project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

############## Artefacts Output ############################
# Defines outputs , depending BUILD TYPE                   #
############################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${PROJECT_DIR}/${OUTPUT_RELEASE}")
endif()

# Messages
message("${PROJECT_NAME}: MAIN PROJECT: ${CMAKE_PROJECT_NAME}")
message("${PROJECT_NAME}: CURR PROJECT: ${CMAKE_CURRENT_SOURCE_DIR}")
message("${PROJECT_NAME}: CURR BIN DIR: ${CMAKE_CURRENT_BINARY_DIR}")

############### Files & Targets ############################
# Files of project and target to build                     #
############################################################

# Source Files
set(SRC_FILES
    #Arduino/Serial.cpp
    #Arduino/Test.cpp
    Client/Client.cpp
    Engine/Draw.cpp
    Engine/Draw2DShader.cpp
    Engine/Draw3DShader.cpp
    Engine/Font.cpp
    Engine/Shader.cpp
    Engine/Sound.cpp
    Engine/TextShader.cpp
    Engine/Texture.cpp
    Engine/Window.cpp
    lodepng/lodepng.cpp
    Main.cpp
    Widgets/AirMap.cpp
    Widgets/AirPlaneAutoPilot.cpp
    Widgets/AtitudeIndicator.cpp
    Widgets/Console.cpp
    Widgets/Control.cpp
    Widgets/Dock.cpp
    Widgets/LaunchAss.cpp
    Widgets/NavBall.cpp
    Widgets/Orbit.cpp
    Widgets/Settings.cpp
    Widgets/SoyuzNavBall.cpp
    Widgets/Util/IM.cpp
    Widgets/Util/TextureLoader.cpp
    Widgets/VInfo.cpp
    Widgets/Widget.cpp
)
source_group("Sources" FILES ${SRC_FILES})

# Header Files
set(HEADERS_FILES
    #Arduino/Serial.h
    #Arduino/Structs.h
    Client/Client.h
    Client/Structs.h
    Engine/Draw.h
    Engine/Draw2DShader.h
    Engine/Draw3DShader.h
    Engine/Font.h
    Engine/Shader.h
    Engine/Sound.h
    Engine/TextShader.h
    Engine/Texture.h
    Engine/Window.h
    lodepng/lodepng.h
    AltiMeter.h
    JoyStick.h
    Reg.h
    resource.h
    Widgets/AirMap.h
    Widgets/AirPlaneAutoPilot.h
    Widgets/AtitudeIndicator.h
    Widgets/Console.h
    Widgets/Control.h
    Widgets/Dock.h
    Widgets/LaunchAss.h
    Widgets/NavBall.h
    Widgets/Orbit.h
    Widgets/Settings.h
    Widgets/SoyuzNavBall.h
    Widgets/Util/IM.h
    Widgets/Util/TextureLoader.h
    Widgets/VInfo.h
    Widgets/Widget.h
)
source_group("Headers" FILES ${HEADERS_FILES})

# Add executable to build.
add_executable(${PROJECT_NAME}
   ${SRC_FILES} ${HEADERS_FILES}
)

######################### Flags ############################
# Defines Flags for Windows and Linux                      #
############################################################

if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -pthread")
   endif()
endif(NOT MSVC)

# Preprocessor definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /MD /Od /EHsc)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
    )
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE  /W3 /GL /Oi /Gy /EHsc)
    endif()
endif()

########### Link & Dependencies ############################
# Add project dependencies and Link to project             #
############################################################

if(MSVC)
   target_link_libraries(${PROJECT_NAME} glu32.lib SDL2.lib freetype271.lib opengl32.lib glew32s.lib )
endif(MSVC)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)
include_directories(${GLEW_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
