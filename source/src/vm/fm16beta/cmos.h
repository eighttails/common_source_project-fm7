/*
	FUJITSU FM16beta Emulator 'eFM16beta'

	Author : Takeda.Toshiya
	Date   : 2017.12.30-

	[ cmos ]
*/

#ifndef _CMOS_H_
#define _CMOS_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

namespace FM16BETA {
	
class CMOS : public DEVICE
{
private:
	uint8_t cmos[0x800];
	bool modified;
	
public:
	CMOS(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("CMOS RAM"));
	}
	~CMOS() {}
	
	// common functions
	void initialize();
	void release();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	bool process_state(FILEIO* state_fio, bool loading);
};

}
#endif

