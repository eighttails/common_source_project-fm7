cmake_minimum_required (VERSION 2.6)

message("* vm/${EXE_NAME}")

string(TOUPPER "${EXE_NAME}" U_EXE_NAME)

if("${U_EXE_NAME}" STREQUAL "EMUMZ800")
add_library(vm_${EXE_NAME}
	./cmos.cpp
	./emm.cpp
	./kanji.cpp
	./keyboard.cpp
	./memory.cpp
	./ramfile.cpp
	./floppy.cpp
	./quickdisk.cpp
	
	./mz700.cpp
)
elseif("${U_EXE_NAME}" STREQUAL "EMUMZ1500")
add_library(vm_${EXE_NAME}
	../mz1p17.cpp

	./cmos.cpp
	./emm.cpp
	./kanji.cpp
	./keyboard.cpp
	./memory.cpp
	./ramfile.cpp
	./floppy.cpp
	./quickdisk.cpp
	./ramfile.cpp
	./psg.cpp
	
	./mz700.cpp
)
else()
# MZ700
add_library(vm_${EXE_NAME}
	./cmos.cpp
	./emm.cpp
	./kanji.cpp
	./keyboard.cpp
	./memory.cpp
	./ramfile.cpp
	
	./mz700.cpp
)


endif()
