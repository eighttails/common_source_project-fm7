/*
	Japan Electronics College MYCOMZ-80A Emulator 'eMYCOMZ-80A'

	Author : Takeda.Toshiya
	Date   : 2009.05.18-

	[ keyboard ]
*/

#include "keyboard.h"
#include "../i8255.h"
#include "../../fifo.h"

static const int keycode[256] = {	// normal
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x008, 0x009, 0x000, 0x000, 0x000, 0x00a, 0x000, 0x000,
	0x000, 0x000, 0x061, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x01b, 0x000, 0x000, 0x000, 0x000,
	0x020, 0x000, 0x000, 0x000, 0x063, 0x175, 0x066, 0x075, 0x166, 0x000, 0x000, 0x000, 0x000, 0x064, 0x164, 0x000,
	0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x048, 0x049, 0x04a, 0x04b, 0x04c, 0x04d, 0x04e, 0x04f,
	0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059, 0x05a, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039, 0x02a, 0x02b, 0x000, 0x02d, 0x02e, 0x02f,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x03a, 0x03b, 0x02c, 0x02d, 0x02e, 0x02f,
	0x040, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x05b, 0x05c, 0x05d, 0x05e, 0x000,
	0x000, 0x000, 0x05f, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};
static const int keycode_s[256] = {	// shift
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x008, 0x009, 0x000, 0x000, 0x000, 0x00a, 0x000, 0x000,
	0x000, 0x000, 0x061, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x01b, 0x000, 0x000, 0x000, 0x000,
	0x020, 0x000, 0x000, 0x000, 0x063, 0x175, 0x066, 0x075, 0x166, 0x000, 0x000, 0x000, 0x000, 0x064, 0x164, 0x000,
	0x000, 0x021, 0x022, 0x023, 0x024, 0x025, 0x026, 0x027, 0x028, 0x029, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x041, 0x042, 0x043, 0x044, 0x045, 0x046, 0x047, 0x048, 0x049, 0x04a, 0x04b, 0x04c, 0x04d, 0x04e, 0x04f,
	0x050, 0x051, 0x052, 0x053, 0x054, 0x055, 0x056, 0x057, 0x058, 0x059, 0x05a, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039, 0x02a, 0x02b, 0x000, 0x02d, 0x02e, 0x02f,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x02a, 0x02b, 0x03c, 0x03d, 0x03e, 0x02f,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x05f, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};
static const int keycode_k[256] = {	// kana
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x088, 0x089, 0x000, 0x000, 0x000, 0x08a, 0x000, 0x000,
	0x000, 0x000, 0x0e1, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x09b, 0x000, 0x000, 0x000, 0x000,
	0x0a0, 0x000, 0x000, 0x000, 0x0e3, 0x1f5, 0x0e6, 0x0f5, 0x1e6, 0x000, 0x000, 0x000, 0x000, 0x0e4, 0x1e4, 0x000,
	0x0dc, 0x0c7, 0x0cc, 0x0b1, 0x0b3, 0x0b4, 0x0b5, 0x0d4, 0x0d5, 0x0d6, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x0c1, 0x0ba, 0x0bf, 0x0bc, 0x0b2, 0x0ca, 0x0b7, 0x0b8, 0x0c6, 0x0cf, 0x0c9, 0x0d8, 0x0d3, 0x0d0, 0x0d7,
	0x0be, 0x0c0, 0x0bd, 0x0c4, 0x0b6, 0x0c5, 0x0cb, 0x0c3, 0x0bb, 0x0dd, 0x0c2, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039, 0x02a, 0x02b, 0x000, 0x02d, 0x02e, 0x02f,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0b9, 0x0da, 0x0c8, 0x0ce, 0x0d9, 0x0d2,
	0x0de, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0df, 0x0b0, 0x0d1, 0x0cd, 0x000,
	0x000, 0x000, 0x0db, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};
static const int keycode_ks[256] = {	// kana shift
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x088, 0x089, 0x000, 0x000, 0x000, 0x08a, 0x000, 0x000,
	0x000, 0x000, 0x0e1, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x09b, 0x000, 0x000, 0x000, 0x000,
	0x0a0, 0x000, 0x000, 0x000, 0x0e3, 0x1f5, 0x0e6, 0x0f5, 0x1e6, 0x000, 0x000, 0x000, 0x000, 0x0e4, 0x1e4, 0x000,
	0x0a6, 0x000, 0x000, 0x0a7, 0x0a9, 0x0aa, 0x0ab, 0x0ac, 0x0ad, 0x0ae, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x0a8, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0af, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x030, 0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038, 0x039, 0x02a, 0x02b, 0x000, 0x02d, 0x02e, 0x02f,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0a4, 0x000, 0x0a1, 0x0a5,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x0a2, 0x000, 0x0a3, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000
};

void KEYBOARD::initialize()
{
	key_buf = new FIFO(8);
	key_stat = emu->key_buffer();
	
	// register event
	register_frame_event(this);
}

void KEYBOARD::release()
{
	key_buf->release();
	delete key_buf;
}

void KEYBOARD::reset()
{
	key_buf->clear();
	key_code = 0;
	event_cnt = 0;
	kana = false;
	
	d_pio1->write_signal(SIG_I8255_PORT_C, 0xf0, 0xf0);
	d_pio2->write_signal(SIG_I8255_PORT_A, 0, 3);
}

void KEYBOARD::key_down(int code)
{
	if(code == 0x13) {
		// break
		d_cpu->write_signal(SIG_CPU_NMI, 1, 1);
	} else if(code == 0x14) {
		// caps
		kana = false;
	} else if(code == 0x15) {
		// kana
		kana = !kana;
	} else if(code == 0x70) {
		// f1 -> s2
		d_pio1->write_signal(SIG_I8255_PORT_C, 0, 0x10);
	} else if(code == 0x71) {
		// f2 -> s3
		d_pio1->write_signal(SIG_I8255_PORT_C, 0, 0x20);
	} else if(code == 0x72) {
		// f3 -> s4
		d_pio1->write_signal(SIG_I8255_PORT_C, 0, 0x40);
	} else if(code == 0x73) {
		// f4 -> s5
		d_pio1->write_signal(SIG_I8255_PORT_C, 0, 0x80);
	} else {
		if(kana) {
			if(key_stat[0x11] && (0x40 <= code && code < 0x60)) {
				code += 0x40;
			} else if(key_stat[0x10]) {
				code = keycode_ks[code];
			} else {
				code = keycode_k[code];
			}
		} else {
			if(key_stat[0x11] && (0x40 <= code && code < 0x60)) {
				code -= 0x40;
			} else if(key_stat[0x10]) {
				code = keycode_s[code];
			} else {
				code = keycode[code];
			}
		}
		if(key_stat[0x10]) {
			code |= 0x100;
		}
		if(code) {
			key_buf->write(code);
		}
	}
}

void KEYBOARD::key_up(int code)
{
	if(code == 0x70) {
		// f1 -> s2
		d_pio1->write_signal(SIG_I8255_PORT_C, 0x10, 0x10);
	} else if(code == 0x71) {
		// f2 -> s3
		d_pio1->write_signal(SIG_I8255_PORT_C, 0x20, 0x20);
	} else if(code == 0x72) {
		// f3 -> s4
		d_pio1->write_signal(SIG_I8255_PORT_C, 0x40, 0x40);
	} else if(code == 0x73) {
		// f4 -> s5
		d_pio1->write_signal(SIG_I8255_PORT_C, 0x80, 0x80);
	}
}

void KEYBOARD::event_frame()
{
	switch(event_cnt) {
	case 0:
		if(key_buf->empty()) {
			key_code = 0;
		} else {
			key_code = key_buf->read();
		}
		if(key_code) {
			// shift
			if(key_code & 0x100) {
				d_pio2->write_signal(SIG_I8255_PORT_A, 2, 2);
			} else {
				d_pio2->write_signal(SIG_I8255_PORT_A, 0, 2);
			}
			// key code
			if(key_code & 0xff) {
				d_pio1->write_signal(SIG_I8255_PORT_B, key_code, 0xff);
			}
		}
		break;
	case 1:
		// key pressed
		if(key_code & 0xff) {
			d_pio2->write_signal(SIG_I8255_PORT_A, 1, 1);
		}
		break;
	case 2:
		// key released
		if(key_code & 0xff) {
			d_pio2->write_signal(SIG_I8255_PORT_A, 0, 1);
		}
		break;
	}
	if(++event_cnt > 5) {
		event_cnt = 0;
	}
}

