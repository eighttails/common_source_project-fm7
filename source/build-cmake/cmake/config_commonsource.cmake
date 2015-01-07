# Set configuration for building XM7/SDL.
# (C) 2014 K.Ohta <whatisthis.sowhat@gmail.com>
# This is part of XM7/SDL, but license is apache 2.2,
# this part was written only me.

include(CheckFunctionExists)

set(WITH_AGAR_STATIC ON CACHE BOOL "Link LibAgar statically.")
set(WITH_LIBAGAR_PREFIX "/usr/local" CACHE STRING "Set prefix of LibAgar") 
include_directories(${WITH_LIBAGAR_PREFIX}/include/agar)



add_definitions(-D_USE_AGAR)

# Build Flags


if(WITH_AGAR_STATIC)
  set(AGAR_LIBS ${WITH_LIBAGAR_PREFIX}/lib/libag_dev.a ${WITH_LIBAGAR_PREFIX}/lib/libag_gui.a ${WITH_LIBAGAR_PREFIX}/lib/libag_core.a)
else(WITH_AGAR_STATIC)
  set(AGAR_LIBS ag_core ag_dev ag_gui)
  link_directories(${WITH_LIBAGAR_PREFIX}/lib)
endif()

set(AGAR_DEPLIBS m jpeg png z dl uim-scm uim Xinerama)

find_package(Gettext)
include_directories(${GETTEXT_INCLUDE_PATH})
include(compile_gettext_catalogue)
if(GETTEXT_FOUND)
   add_definitions(-DUSE_GETTEXT)
endif()


find_package(Freetype)
include_directories(${FREETYPE_INCLUDE_PATH})



find_package(Iconv)
if(ICONV_FOUND)
  add_definitions(-DUSE_ICONV)
endif()

if(USE_OPENMP)
  find_package(OpenMP)
  include_directories(${OPENMP_INCLUDE_PATH})
endif()

find_package(Threads)
include_directories(${THREADS_INCLUDE_PATH})

find_package(SDL)
include_directories(${SDL_INCLUDE_PATH})


if(ICONV_FOUND)
 include_directories(${ICONV_INCLUDE_DIRS})
 set(LOCAL_LIBS ${LOCAL_LIBS} ${ICONV_LIBRARIES})
endif()


# GCC Only?
if(CMAKE_COMPILER_IS_GNUCC) 
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -flax-vector-conversions")
endif()

if(CMAKE_COMPILER_IS_GNUCXX) 
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -flax-vector-conversions")
endif()


check_function_exists("nanosleep" HAVE_NANOSLEEP)
if(NOT HAVE_NANOSLEEP)
  check_library_exists("rt" "nanosleep" "" LIB_RT_HAS_NANOSLEEP)
endif(NOT HAVE_NANOSLEEP)

if(HAVE_NANOSLEEP OR LIB_RT_HAS_NANOSLEEP)
  add_definitions(-DHAVE_NANOSLEEP)
endif(HAVE_NANOSLEEP OR LIB_RT_HAS_NANOSLEEP)

find_package(OpenGL)

if(USE_OPENCL)
 if(OPENGL_FOUND)
   find_package(OpenCL)
   if(OPENCL_FOUND)
    include_directories(${OPENCL_INCLUDE_DIRS})
    add_definitions(-D_USE_OPENCL -DUSE_OPENCL)
    set(OPENCL_LIBRARY ${OPENCL_LIBRARIES})
    set(USE_OPENGL ON)
   endif()
 endif()
endif()

if(USE_OPENGL)
 if(OPENGL_FOUND)
   include_directories(${OPENGL_INCLUDE_PATH})
   add_definitions(-D_USE_OPENGL -DUSE_OPENGL)
 else()
   set(USE_OPENGL OFF)
   set(USE_OPENCL OFF)
   unset(OPENCL_LIBRARY)
 endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/vm)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/agar/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/agar/common/scaler/generic)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/agar/ui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/agar/menu)

add_subdirectory(../../src/agar/common/scaler/generic agar/common/scaler/generic)
add_subdirectory(../../src/agar/ui agar/ui)
add_subdirectory(../../src/agar/menu agar/menu)

add_subdirectory(../../src common)
add_subdirectory(../../src/vm vm/)

include(simd-x86)
