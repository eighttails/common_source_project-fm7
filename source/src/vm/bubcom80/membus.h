/*
	Systems Formulate BUBCOM80 Emulator 'eBUBCOM80'

	Author : Takeda.Toshiya
	Date   : 2018.05.08-

	[ memory bus ]
*/

#ifndef _MEMBUS_H_
#define _MEMBUS_H_

#include "../vm.h"
#include "../memory.h"

namespace BUBCOM80 {

class MEMBUS : public MEMORY
{
private:
//	csp_state_utils *state_entry;
	
	uint8_t boot[0x800];
	uint8_t basic[0x10000];
	uint8_t ram[0x10000];
	
	pair32_t basic_addr;
	bool ram_selected;
	
	void update_bank();
	
public:
	MEMBUS(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : MEMORY(parent_vm, parent_emu)
	{
		set_device_name(_T("Memory Bus"));
	}
	~MEMBUS() {}
	
	// common functions
	void initialize();
	void reset();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	void __FASTCALL write_dma_data8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_dma_data8(uint32_t addr);
	bool process_state(FILEIO* state_fio, bool loading);
};
}
#endif
