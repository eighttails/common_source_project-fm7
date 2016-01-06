# Build Common Sourcecode Project, Qt.
# (C) 2014 K.Ohta <whatisthis.sowhat@gmail.com>
# This is part of XM7/SDL, but license is apache 2.2,
# this part was written only me.

cmake_minimum_required (VERSION 2.8)
cmake_policy(SET CMP0011 NEW)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")

set(VM_NAME x1)
set(USE_FMGEN ON)
set(WITH_JOYSTICK ON)
set(WITH_MOUSE ON)

set(VMFILES
		   z80.cpp
		   mcs48.cpp
		   
		   beep.cpp
		   hd46505.cpp
		   i8255.cpp
		   ym2151.cpp
		   ym2203.cpp
		   mb8877.cpp
		   upd1990a.cpp
		   z80ctc.cpp
		   z80pio.cpp
		   z80sio.cpp
		   
		   prnfile.cpp
		   datarec.cpp
		   disk.cpp
		   event.cpp
		   io.cpp
		   memory.cpp
		   prnfile.cpp
)

set(BUILD_X1 OFF CACHE BOOL "Build for X1")
set(BUILD_X1TURBO OFF CACHE BOOL "Build for X1 Turbo")
set(BUILD_X1TURBOZ OFF CACHE BOOL "Build for X1 TurboZ")
set(BUILD_X1TWIN OFF CACHE BOOL "Build for X1 twin")

set(BUILD_SHARED_LIBS OFF)
set(USE_CMT_SOUND ON CACHE BOOL "Sound with CMT")
set(USE_OPENMP ON CACHE BOOL "Build using OpenMP")
set(USE_OPENGL ON CACHE BOOL "Build using OpenGL")
set(XM7_VERSION 3)
set(WITH_DEBUGGER ON CACHE BOOL "Build with debugger.")

include(detect_target_cpu)
#include(windows-mingw-cross)
# set entry
set(CMAKE_SYSTEM_PROCESSOR ${ARCHITECTURE} CACHE STRING "Set processor to build.")

if(BUILD_X1)
  set(EXEC_TARGET emux1)
  add_definitions(-D_X1)
  set(RESOURCE ${CMAKE_SOURCE_DIR}/../../src/qt/common/qrc/x1.qrc)
elseif(BUILD_X1TURBO)
  set(EXEC_TARGET emux1turbo)
  add_definitions(-D_X1TURBO)
  set(VMFILES ${VMFILES} z80dma.cpp)
  set(RESOURCE ${CMAKE_SOURCE_DIR}/../../src/qt/common/qrc/x1turbo.qrc)
elseif(BUILD_X1TURBOZ)
  set(EXEC_TARGET emux1turboz)
  add_definitions(-D_X1TURBOZ)
  set(VMFILES ${VMFILES} z80dma.cpp)
  set(RESOURCE ${CMAKE_SOURCE_DIR}/../../src/qt/common/qrc/x1turboz.qrc)
elseif(BUILD_X1TWIN)
  set(EXEC_TARGET emux1twin)
  add_definitions(-D_X1TWIN)
  set(LOCAL_LIBS ${LOCAL_LIBS} vm_pcengine)
  set(VMFILES ${VMFILES} huc6280.cpp)
  set(RESOURCE ${CMAKE_SOURCE_DIR}/../../src/qt/common/qrc/x1twin.qrc)
endif()


if(USE_CMT_SOUND)
  add_definitions(-DDATAREC_SOUND)
endif()
if(BUILD_X1TWIN)
  set(VM_APPEND_LIBS vm_pcengine)
endif()

include(config_commonsource)
if(BUILD_X1TWIN)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/vm/pcengine)
 add_subdirectory(../../src/vm/pcengine vm/pcengine)
endif()
   

