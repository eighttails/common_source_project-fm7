/*
	NEC PC-8201 Emulator 'ePC-8201'

	Author : Takeda.Toshiya
	Date   : 2009.03.31-

	[ keyboard ]
*/

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

#define SIG_KEYBOARD_COLUMN_L	0
#define SIG_KEYBOARD_COLUMN_H	1

namespace PC8201 {

class KEYBOARD : public DEVICE
{
private:
	const uint8_t* key_stat;
	uint16_t column;
	bool caps, kana;
	
public:
	KEYBOARD(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("Keyboard"));
	}
	~KEYBOARD() {}
	
	// common functions
	void initialize();
	uint32_t __FASTCALL read_io8(uint32_t addr);
	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void key_down(int code);
	bool get_caps_locked()
	{
		return caps;
	}
	bool get_kana_locked()
	{
		return kana;
	}
};

}
#endif
