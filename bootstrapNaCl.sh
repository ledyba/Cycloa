CURDIR=$(cd $(dirname $0); pwd)
sh ${CURDIR}/cleanCmake.sh
cmake -DCMAKE_TOOLCHAIN_FILE=${CURDIR}/cmake/Modules/Platform/NaCl.cmake -G "Eclipse CDT4 - Unix Makefiles" ${CURDIR} -DCMAKE_VERBOSE_MAKEFILE=True
