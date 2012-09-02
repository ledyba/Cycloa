SRCDIR=$(cd $(dirname $0);cd ..; pwd)
sh ${SRCDIR}/misc/cleanCmake.sh
cmake -DCMAKE_TOOLCHAIN_FILE=${SRCDIR}/cmake/Modules/Platform/NaCl.cmake -G "Eclipse CDT4 - Unix Makefiles" ${SRCDIR} -DCMAKE_VERBOSE_MAKEFILE=True
