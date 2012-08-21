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
project (CYCLOA)

#Main src directory
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE cycloa_base_SRCS ${SRC_DIR}/emulator/*.cpp)

#ソースフォルダの設定
file(GLOB_RECURSE cycloa_sdl_SRCS ${SRC_DIR}/fairy/sdl/*.cpp)

#実行バイナリ
add_executable(CycloaSDL2 ${cycloa_base_SRCS} ${cycloa_sdl_SRCS})
add_executable(CycloaSDL  ${cycloa_base_SRCS} ${cycloa_sdl_SRCS})

SET_TARGET_PROPERTIES(CycloaSDL2 PROPERTIES COMPILE_FLAGS "-DCYCLOA_SDL2")
SET_TARGET_PROPERTIES(CycloaSDL  PROPERTIES COMPILE_FLAGS "-DCYCLOA_SDL")

find_package(PkgConfig)

pkg_check_modules(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})
target_link_libraries(CycloaSDL2 ${CAIRO_LIBRARIES})

pkg_check_modules(FREETYPE REQUIRED freetype2)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})
target_link_libraries(CycloaSDL2 ${FREETYPE_LIBRARIES})

pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
include_directories(${FONTCONFIG_INCLUDE_DIRS})
link_directories(${FONTCONFIG_LIBRARY_DIRS})
target_link_libraries(CycloaSDL2 ${FONTCONFIG_LIBRARIES})

pkg_check_modules(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})
target_link_libraries(CycloaSDL2 ${SDL2_LIBRARIES})

