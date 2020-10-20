#
#  Cycloa
#  Copyright (C) 2011-2012 psi
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required (VERSION 2.8)
project (Cycloa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (WIN32)
    # https://stackoverflow.com/questions/47690822/possible-to-force-cmake-msvc-to-use-utf-8-encoding-for-source-files-without-a-bo
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif(WIN32)

## sources
file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)

###############################################################################
## libraries
###############################################################################
# SDL2
add_subdirectory(external/SDL)
include_directories(external/SDL/include)
# fmt
set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_FUZZ OFF CACHE BOOL "" FORCE)
add_subdirectory(external/fmt)
###############################################################################

## executes
add_executable(Cycloa ${SRCS})
target_link_libraries(Cycloa PRIVATE fmt::fmt)
target_link_libraries(Cycloa PRIVATE SDL2-static)
target_link_libraries(Cycloa PRIVATE SDL2main)
