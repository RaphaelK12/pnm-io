CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
PROJECT(ppm-test)

# Default to release build
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release)
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Project Compiler Flags
# ADD_DEFINITIONS(-Wall)

INCLUDE_DIRECTORIES(
  ../include
  include)

ADD_EXECUTABLE(ppm-test
  main.cpp)