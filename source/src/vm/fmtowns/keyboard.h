/*
	FUJITSU FMR-50 Emulator 'eFMR-50'
	FUJITSU FMR-60 Emulator 'eFMR-60'

	Author : Takeda.Toshiya
	Date   : 2008.05.01 -

	[ keyboard ]
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"
#include "../../fifo.h"

/*
	BREAK				Pause Break
	ひらがな/ローマ字	ひらがな
	半角/全角			半角/全角
	変換				変換
	無変換				無変換
	かな/漢字			F11
	カタカナ			F12
	前行				PgUp
	次行				PgDn
	実行				Right WIN
	取消				Left WIN
	漢字　辞書			RALT + Print Screen
	単語　抹消			RALT + Scroll Lock
	単語　登録			RALT + Pause
	COPY				Print Screen
	PF11				RALT + F1
	PF12				RALT + F2
	PF13				RALT + F3
	PF14				RALT + F4
	PF15				RALT + F5
	PF16				RALT + F6
	PF17				RALT + F7
	PF18				RALT + F8
	PF19				RALT + F9
	PF20				RALT + F10
	ALT					LALT
*/
namespace FMTOWNS {
/* 
 * From 
 * http://www.nurs.or.jp/~kurati/towns/scancode.html
 * and http://m0115.web.fc2.com/fmr60kb212.jpg :
 * TOWNS の ScanCode
	   +0		+1		+2		+3		+4		+5		+6		+7
0x00			ESC		1		2		3		4		5		6
0x08	7		8		9		0		-		=		\		後退
0x10	TAB		Q		W		E		R		T		Y		U
0x18	I		O		P		@		[		改行	A		S
0x20	D		F		G		H		J		K		L		:
0x28	;		]		Z		X		C		V		B		N
0x30	M		,		.		/		"		空白	[KP]*	[KP]/
0x38	[KP]+	[KP]-	[KP]7	[KP]8	[KP]9	[KP]=	[KP]4	[KP]5
0x40	[KP]6	---		[KP]1	[KP]2	[KP]3	[KP]改行[KP]0	[KP].
0x48	INS		---		[KP]0	DEL		---		↑	 	HOME	←
0x50	↓		→		CTRL	SHIFT	---		CAPS	LANG	ALT
0x58	CTRL	SPACE	LANG	F12		ALT		F1		F2		F3
0x60	F4		F5		F6		F7		F8		F9		F10		---
0x68	---		F11		英字	辞書	抹消	登録	前行	---
0x70	次行	半/全	END		実行	F13		F14		F15		F16
0x78	F17		F18		F19		F20						
*/
static const int key_table[256] = {
/*	VK -> RAW code table                    +8                                           */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x10,0x00,0x00,0x00,0x1D,0x00,0x00,/*0x*/
	0x53,0x52,0x00,0x7C,0x55,0x56,0x00,0x00,0x00,0x71,0x00,0x01,0x58,0x57,0x00,0x00,/*1x*/
	0x35,0x70,0x6E,0x00,0x4E,0x4F,0x4D,0x51,0x50,0x00,0x7D,0x00,0x00,0x48,0x4B,0x00,/*2x*/
	0x0B,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,/*3x*/
	0x00,0x1E,0x2E,0x2C,0x20,0x13,0x21,0x22,0x23,0x18,0x24,0x25,0x26,0x30,0x2F,0x19,/*4x*/
	0x1A,0x11,0x14,0x1F,0x15,0x17,0x2D,0x12,0x2B,0x16,0x2A,0x72,0x73,0x00,0x00,0x00,/*5x*/
	0x46,0x42,0x43,0x44,0x3E,0x3F,0x40,0x3A,0x3B,0x3C,0x36,0x38,0x00,0x39,0x47,0x37,/*6x*/
	0x5D,0x5E,0x5F,0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x59,0x5a,0x74,0x75,0x76,0x77,/*7x*/
	0x78,0x79,0x7A,0x7B,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*8x*/
	0x00,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*9x*/
	0x00,0x00,0x00,0x00,0x5C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*ax*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x28,0x27,0x31,0x0C,0x32,0x33,/*bx*/
	0x1b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*cx*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x0E,0x29,0x0D,0x00,/*dx*/
	0x00,0x00,0x34,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*ex*/
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 /*fx*/
};
}

class FIFO;

namespace FMTOWNS {
class KEYBOARD : public DEVICE
{
protected:
	outputs_t output_intr_line;
	outputs_t output_nmi_line;
	
	FIFO *key_buf;
	FIFO *cmd_buf;
	
	uint8_t kbstat, kbdata, kbint, kbmsk;
	int repeat_start_ms;
	int repeat_tick_ms;
	bool enable_double_pressed_cursor;
	bool device_order;
	
	bool nmi_status;
	uint8_t table[256];

	int event_keycode;
	
	virtual void do_common_command(uint8_t cmd);
	void register_key_interrupt(bool first);

public:
	KEYBOARD(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		key_buf = NULL;
		cmd_buf = NULL;
		
		initialize_output_signals(&output_intr_line);
		initialize_output_signals(&output_nmi_line);
		set_device_name(_T("FM-Towns Keyboard (JIS)"));
	}
	~KEYBOARD() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	void event_callback(int event_id, int err);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void set_context_intr_line(DEVICE* dev, int id, uint32_t mask)
	{
		register_output_signal(&output_intr_line, dev, id, mask);
	}
	void set_context_nmi_line(DEVICE* dev, int id, uint32_t mask)
	{
		register_output_signal(&output_nmi_line, dev, id, mask);
	}
	void key_down(int code);
	void key_up(int code);
};
}

#endif
