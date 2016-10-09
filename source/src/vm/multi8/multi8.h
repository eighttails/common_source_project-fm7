/*
	MITSUBISHI Electric MULTI8 Emulator 'EmuLTI8'

	Author : Takeda.Toshiya
	Date   : 2006.09.15 -

	[ virtual machine ]
*/

#ifndef _MULTI8_H_
#define _MULTI8_H_

#define DEVICE_NAME		"MITSUBISHI Electric MULTI 8"
#define CONFIG_NAME		"multi8"

// device informations for virtual machine
#define FRAMES_PER_SEC		60.58
#define LINES_PER_FRAME 	260
#define CHARS_PER_LINE		112
#define HD46505_HORIZ_FREQ	15750
#define CPU_CLOCKS		3993600
#define SCREEN_WIDTH		640
#define SCREEN_HEIGHT		400
#define WINDOW_HEIGHT_ASPECT	480
#define I8259_MAX_CHIPS		1
#define MAX_DRIVE		4
#define UPD765A_DONT_WAIT_SEEK
#define HAS_AY_3_8912
#define SUPPORT_VARIABLE_TIMING

// device informations for win32
#define USE_TAPE
#define TAPE_BINARY_ONLY
#define USE_FD1
#define USE_FD2
//#define USE_FD3
//#define USE_FD4
#define USE_SHIFT_NUMPAD_KEY
#define USE_ALT_F10_KEY
#define USE_AUTO_KEY		5
#define USE_AUTO_KEY_RELEASE	6
#define USE_AUTO_KEY_CAPS
#define USE_CRT_FILTER
#define USE_SCANLINE
#define USE_ACCESS_LAMP
#define USE_SOUND_FILES		2
#define USE_SOUND_FILES_FDD
//#define USE_SOUND_FILES_RELAY
#if defined(USE_SOUND_FILES)
#define USE_SOUND_VOLUME	3
#else
#define USE_SOUND_VOLUME	2
#endif
#define USE_DEBUGGER
#define USE_STATE

#include "../../common.h"
#include "../../fileio.h"

#ifdef USE_SOUND_VOLUME
static const _TCHAR *sound_device_caption[] = {
	_T("PSG"), _T("Beep"),
#if defined(USE_SOUND_FILES)
	_T("FDD Seek"),
#endif
};
#endif

class EMU;
class DEVICE;
class EVENT;

class BEEP;
class HD46505;
class I8251;
class I8253;
class I8255;
class I8259;
class IO;
class UPD765A;
class YM2203;
class Z80;

class CMT;
class DISPLAY;
class FLOPPY;
class KANJI;
class KEYBOARD;
class MEMORY;

class VM
{
protected:
	EMU* emu;
	
	// devices
	EVENT* event;
	
	BEEP* beep;
	HD46505* crtc;
	I8251* sio;
	I8253* pit;
	I8255* pio;
	I8259* pic;
	IO* io;
	UPD765A* fdc;
	YM2203* psg;
	Z80* cpu;
	
	CMT* cmt;
	DISPLAY* display;
	FLOPPY* floppy;
	KANJI* kanji;
	KEYBOARD* key;
	MEMORY* memory;
	
public:
	// ----------------------------------------
	// initialize
	// ----------------------------------------
	
	VM(EMU* parent_emu);
	~VM();
	
	// ----------------------------------------
	// for emulation class
	// ----------------------------------------
	
	// drive virtual machine
	void reset();
	void run();
	double get_frame_rate();
	
#ifdef USE_DEBUGGER
	// debugger
	DEVICE *get_cpu(int index);
#endif
	
	// draw screen
	void draw_screen();
	uint32_t get_access_lamp_status();
	
	// sound generation
	void initialize_sound(int rate, int samples);
	uint16_t* create_sound(int* extra_frames);
	int get_sound_buffer_ptr();
#ifdef USE_SOUND_VOLUME
	void set_sound_device_volume(int ch, int decibel_l, int decibel_r);
#endif
	
	// user interface
	void open_floppy_disk(int drv, const _TCHAR* file_path, int bank);
	void close_floppy_disk(int drv);
	bool is_floppy_disk_inserted(int drv);
	void is_floppy_disk_protected(int drv, bool value);
	bool is_floppy_disk_protected(int drv);
	void play_tape(const _TCHAR* file_path);
	void rec_tape(const _TCHAR* file_path);
	void close_tape();
	bool is_tape_inserted();
	bool is_frame_skippable();
	
	void update_config();
	void save_state(FILEIO* state_fio);
	bool load_state(FILEIO* state_fio);
	
	// ----------------------------------------
	// for each device
	// ----------------------------------------
	
	// devices
	DEVICE* get_device(int id);
	DEVICE* dummy;
	DEVICE* first_device;
	DEVICE* last_device;
};

#endif
