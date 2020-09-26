#!/bin/sh
export PATH="/opt/llvm-mingw/bin:\
            $PATH:\
            /usr/local/i586-mingw-msvc/icu/bin:\
	    /usr/local/i586-mingw-msvc/Angle/bin:\
	    /usr/local/i586-mingw-msvc/SDL/i686-w64-mingw32/bin: \
            /usr/local/i586-mingw-msvc/DirectX_June_2010/Developer Runtime/x86 \
            "
export LLVM_INSTALL_DIR="/opt/llvm-mingw"
export PKG_CONFIG_LIBDIR=/usr/i686-w64-mingw32/lib

#taskset 0xfbe make CROSS_COMPILE=i686-w64-mingw32- PKG_CONFIG_LIBDIR=/usr/i686-w64-mingw32/lib $@ 2>&1 
make CROSS_COMPILE=i686-w64-mingw32- PKG_CONFIG_LIBDIR=/usr/local/i586-mingw-msvc/pkgconfig/lib $@ 2>&1 