/*
	CASIO FP-1100 Emulator 'eFP-1100'

	Author : Takeda.Toshiya
	Date   : 2010.06.18-

	[ rom pack ]
*/

#ifndef _ROMPACK_H_
#define _ROMPACK_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

namespace FP1100 {
	
class ROMPACK : public DEVICE
{
private:
	uint8_t rom[0x8000];
	
public:
	ROMPACK(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("ROM Pack"));
	}
	~ROMPACK() {}
	
	// common functions
	void initialize();
	uint32_t __FASTCALL read_io8(uint32_t addr);
};

}
#endif
