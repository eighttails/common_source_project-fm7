cmake_minimum_required (VERSION 2.6)

message("* vm/${EXE_NAME}")

add_library(vm_${EXE_NAME}
	../i8237.cpp
	
	display.cpp
	floppy.cpp
	keyboard.cpp
	mfont.cpp
	./memory.cpp
	qc10.cpp
)
