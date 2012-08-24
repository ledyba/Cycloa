#
#  Cycloa
#  Copyright (C) 2012 psi
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

set(NACL_PATH $ENV{NACL_SDK_ROOT} CACHE PATH "NaCl ROOT")

set(NACL_HOST)
set(NACL_TARGET x86_64)
#set(NACL_LIB glibc)
set(NACL_LIB newlib)
set(NACL_PEPPER pepper_21)

if (APPLE)
	set(NACL_HOST mac_x86)
elseif (WIN32)
	set(NACL_HOST win_x86)
else()
	set(NACL_HOST linux_x86)
endif()

#すでに知られているプラットフォームじゃないとエラーが出ちゃうのでLinuxに偽装
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION ${NACL_PEPPER})

set(NACL_PEPPER_PATH ${NACL_PATH}/${NACL_PEPPER})
set(NACL_TOOLCHAIN_PATH ${NACL_PEPPER_PATH}/toolchain/${NACL_HOST}_${NACL_LIB})

set(CMAKE_FIND_ROOT_PATH
	${NACL_TOOLCHAIN_PATH}
)


set(CMAKE_ASM_COMPILER ${NACL_TOOLCHAIN_PATH}/bin/${NACL_TARGET}-nacl-as)
set(CMAKE_C_COMPILER   ${NACL_TOOLCHAIN_PATH}/bin/${NACL_TARGET}-nacl-gcc)
set(CMAKE_CXX_COMPILER ${NACL_TOOLCHAIN_PATH}/bin/${NACL_TARGET}-nacl-g++)
set(CMAKE_LINKER ${NACL_TOOLCHAIN_PATH}/bin/${NACL_TARGET}-nacl-g++)
set(CMAKE_OBJDUMP ${NACL_TOOLCHAIN_PATH}/${NACL_TARGET}-nacl/bin/objdump)
set(CMAKE_NMF python ${NACL_PEPPER_PATH}/tools/create_nmf.py)

set(CMAKE_C_FLAGS 
	CACHE STRING "OpenEmbedded - GCC/C flags" FORCE
)

set(CMAKE_CXX_FLAGS 
	""
	CACHE STRING "OpenEmbedded - GCC/C++ flags" FORCE
)

set(NACL_ENABLED True)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
