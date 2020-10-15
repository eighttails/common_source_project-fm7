cmake .. -DCMAKE_TOOLCHAIN_FILE="$PWD/../cmake/toolchains/toolchain_native_gcc.cmake" \
      -DCMAKE_BUILD_TYPE=Relwithdebinfo \
      -DCMAKE_C_FLAGS_RELWITHDEBINFO=" \
      		-gz=zlib \
      		-g2 \
		-gno-inline-points \
		-O3 \
		-msse2 \
		-mfpmath=sse \
		" \
      -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=" \
      		-gz=zlib \
      		-g2 \
		-O3 \
		-msse2 \
		-mfpmath=sse \
		-gno-inline-points \
		" \
      -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO="\
      		-g2 \
		-ggdb \
		-gz=zlib \
		-O3 \
		-msse2 \
		-mfpmath=sse \
		" \
      -DCMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO="\
      		-g2 \
		-gz=zlib \
		-gno-inline-points \
		-O3 \
		-msse2 \
		-mfpmath=sse \
		" \