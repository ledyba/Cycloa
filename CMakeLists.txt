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

## sources
file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/src/*.cpp)

## libraries (nowadays, just vendored SDL2)
add_subdirectory(external/SDL)
include_directories(external/SDL/include)

## executes
add_executable(Cycloa ${SRCS})
target_link_libraries(Cycloa SDL2-static)
