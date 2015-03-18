/*
	SORD m5 Emulator 'Emu5'

	Author : Takeda.Toshiya
	Date   : 2006.08.18 -

	[ virtual machine ]
*/

#ifndef _M5_H_
#define _M5_H_

#define DEVICE_NAME		"SORD m5"
#define CONFIG_NAME		"m5"

// device informations for virtual machine
#define FRAMES_PER_SEC		60
#define LINES_PER_FRAME		262
#define CPU_CLOCKS		3800000
#define SCREEN_WIDTH		256
#define SCREEN_HEIGHT		192
#define TMS9918A_VRAM_SIZE	0x4000
//#define TMS9918A_LIMIT_SPRITES
#define MEMORY_ADDR_MAX		0x10000
#define MEMORY_BANK_SIZE	0x1000

// device informations for win32
#define USE_CART1
#define USE_TAPE
#define USE_KEY_TO_JOY
#define USE_ALT_F10_KEY
#define USE_AUTO_KEY		5
#define USE_AUTO_KEY_RELEASE	8
#define USE_DEBUGGER
#define USE_STATE

#include "../../common.h"
#include "../../fileio.h"

class EMU;
class DEVICE;
class EVENT;

class DATAREC;
class IO;
class MEMORY;
class SN76489AN;
class TMS9918A;
class Z80;
class Z80CTC;

class CMT;
class KEYBOARD;

class VM
{
protected:
	EMU* emu;
	
	// devices
	EVENT* event;
	
	DATAREC* drec;
	IO* io;
	MEMORY* memory;
	SN76489AN* psg;
	TMS9918A* vdp;
	Z80* cpu;
	Z80CTC* ctc;
	
	CMT* cmt;
	KEYBOARD* key;
	
	// memory
	uint8 ipl[0x2000];	// ipl (8k)
	uint8 cart[0x5000];	// cartridge (20k)
	uint8 ram[0x1000];	// ram (4k)
	uint8 ext[0x8000];	// ext ram (32k)
	bool inserted;
	
	// FD5 floppy drive uint
	Z80* subcpu;
	
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
	
#ifdef USE_DEBUGGER
	// debugger
	DEVICE *get_cpu(int index);
#endif
	
	// draw screen
	void draw_screen();
	
	// sound generation
	void initialize_sound(int rate, int samples);
	uint16* create_sound(int* extra_frames);
	int sound_buffer_ptr();
	
	// user interface
	void open_cart(int drv, _TCHAR* file_path);
	void close_cart(int drv);
	bool cart_inserted(int drv);
	void play_tape(_TCHAR* file_path);
	void rec_tape(_TCHAR* file_path);
	void close_tape();
	bool tape_inserted();
	bool now_skip();
	
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
