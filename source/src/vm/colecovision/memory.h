/*
	COLECO ColecoVision Emulator 'yaCOLECOVISION'

	Author : tanam
	Date   : 2016.08.14-

	[ memory ]
*/

#ifndef _COLECOVISION_MEMORY_H_
#define _COLECOVISION_MEMORY_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

namespace COLECOVISION {
class MEMORY : public DEVICE
{
private:
	// memory
	uint8_t cart[0x8000];
	uint8_t ipl[0x2000];
	uint8_t ram[0x10000];
	
	uint8_t wdmy[0x10000];
	uint8_t rdmy[0x10000];
	uint8_t* wbank[16];
	uint8_t* rbank[16];
	
	bool inserted;
	
public:
	MEMORY(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("Memory Bus"));
	}
	~MEMORY() {}
	
	// common functions
	void initialize();
	void __FASTCALL write_data8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_data8(uint32_t addr);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void open_cart(const _TCHAR* file_path);
	void close_cart();
	bool is_cart_inserted()
	{
		return inserted;
	}
};
}
#endif
