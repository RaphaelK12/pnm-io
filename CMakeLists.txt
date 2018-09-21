# Copyright (C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

cmake_minimum_required(VERSION 3.1)
project(ppm-io)

set(header_files
	${CMAKE_CURRENT_SOURCE_DIR}/include/thinks/ppm_io/ppm_io.h
)
add_library(thinks_ppm_io INTERFACE)
target_sources(thinks_ppm_io INTERFACE ${header_files})
target_include_directories(thinks_ppm_io INTERFACE include)
#if(MSVC)
#    target_compile_definitions(thinks_obj_io INTERFACE "-D_SCL_SECURE_NO_WARNINGS")
#endif()

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
	#add_subdirectory(examples)
endif()