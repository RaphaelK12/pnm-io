# Copyright (C) 2018 Tommy Hinks <tommy.hinks@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(examples
	rgb_example.cc
)

add_executable(thinks_ppm_io_examples
	main.cc
	${examples})
target_include_directories(thinks_ppm_io_examples SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(thinks_ppm_io_examples PUBLIC thinks_ppm_io)
set_target_properties(thinks_ppm_io_examples PROPERTIES CXX_STANDARD 11)
