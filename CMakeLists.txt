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

#エミュレータコアのソースコード
file(GLOB_RECURSE cycloa_base_SRCS ${SRC_DIR}/emulator/*.cpp)

#ターゲット別ソースコードの指定
file(GLOB_RECURSE cycloa_sdl_SRCS ${SRC_DIR}/fairy/sdl/*.cpp)
file(GLOB_RECURSE cycloa_nacl_SRCS ${SRC_DIR}/fairy/nacl/*.cpp)

#実行バイナリ

find_package(PkgConfig)

#pkg_check_modules(CAIRO REQUIRED cairo)
#include_directories(${CAIRO_INCLUDE_DIRS})
#link_directories(${CAIRO_LIBRARY_DIRS})
#
#pkg_check_modules(FREETYPE REQUIRED freetype2)
#include_directories(${FREETYPE_INCLUDE_DIRS})
#link_directories(${FREETYPE_LIBRARY_DIRS})
#
#pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
#include_directories(${FONTCONFIG_INCLUDE_DIRS})
#link_directories(${FONTCONFIG_LIBRARY_DIRS})

# ライブラリのあるなしに合わせてコンパイルするバイナリを決定

if(CMAKE_CROSSCOMPILING)
	if(NACL_ENABLED)
		ADD_EXECUTABLE(CycloaNaCl_64 ${cycloa_base_SRCS} ${cycloa_nacl_SRCS})
		SET_TARGET_PROPERTIES(CycloaNaCl_64 PROPERTIES SUFFIX ".nexe" PREFIX "")

		ADD_EXECUTABLE(CycloaNaCl_32 ${cycloa_base_SRCS} ${cycloa_nacl_SRCS})
		SET_TARGET_PROPERTIES(CycloaNaCl_32 PROPERTIES SUFFIX ".nexe" PREFIX "")
		
		execute_process(COMMAND ${NACL_TOOLCHAIN_PATH}/x86_64-nacl/usr/bin/sdl-config --static-libs OUTPUT_VARIABLE SDL64_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		execute_process(COMMAND ${NACL_TOOLCHAIN_PATH}/x86_64-nacl/usr/bin/sdl-config --cflags OUTPUT_VARIABLE SDL64_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

		execute_process(COMMAND ${NACL_TOOLCHAIN_PATH}/i686-nacl/usr/bin/sdl-config --static-libs OUTPUT_VARIABLE SDL32_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
		execute_process(COMMAND ${NACL_TOOLCHAIN_PATH}/i686-nacl/usr/bin/sdl-config --cflags OUTPUT_VARIABLE SDL32_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

		SET_TARGET_PROPERTIES(CycloaNaCl_64 PROPERTIES COMPILE_FLAGS "-m64 -g -pthread -Wall")
		SET_TARGET_PROPERTIES(CycloaNaCl_32 PROPERTIES COMPILE_FLAGS "-m32 -g -pthread -Wall")
		
		SET_TARGET_PROPERTIES(CycloaNaCl_64 PROPERTIES LINK_FLAGS "-Wl,-as-needed -m64 -g -pthread")
		SET_TARGET_PROPERTIES(CycloaNaCl_32 PROPERTIES LINK_FLAGS "-Wl,-as-needed -m32 -g -pthread")
		
		TARGET_LINK_LIBRARIES(CycloaNaCl_64 ppapi_cpp ppapi)
		TARGET_LINK_LIBRARIES(CycloaNaCl_32 ppapi_cpp ppapi)

		ADD_CUSTOM_TARGET(CycloaNaCl.nmf ALL COMMAND ${CMAKE_NMF} -D ${CMAKE_OBJDUMP} -o Cycloa.nmf ./CycloaNaCl_32.nexe ./CycloaNaCl_64.nexe -t ${NACL_LIB} -s ${CMAKE_BINARY_DIR} DEPENDS CycloaNaCl_64 CycloaNaCl_32)
	endif()

else() #Platform-Native
	pkg_check_modules(SDL QUIET sdl)
	if (SDL_FOUND)
		message(Building SDL version)
		add_executable(CycloaSDL  ${cycloa_base_SRCS} ${cycloa_sdl_SRCS})
		include_directories(${SDL_INCLUDE_DIRS})
		link_directories(${SDL_LIBRARY_DIRS})
		SET_TARGET_PROPERTIES(CycloaSDL PROPERTIES COMPILE_FLAGS "-DCYCLOA_SDL")
		target_link_libraries(CycloaSDL ${SDL_LIBRARIES})
	endif()
	
	pkg_check_modules(SDL2 QUIET sdl2)
	if (SDL2_FOUND)
		message(Building SDL2 version)
		add_executable(CycloaSDL2 ${cycloa_base_SRCS} ${cycloa_sdl_SRCS})
		include_directories(${SDL2_INCLUDE_DIRS})
		link_directories(${SDL2_LIBRARY_DIRS})
		SET_TARGET_PROPERTIES(CycloaSDL2 PROPERTIES COMPILE_FLAGS "-DCYCLOA_SDL2")
		target_link_libraries(CycloaSDL2 ${SDL2_LIBRARIES})
	endif()
endif()
