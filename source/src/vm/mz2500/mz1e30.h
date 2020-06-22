/*
	SHARP MZ-2500 Emulator 'EmuZ-2500'

	Author : Takeda.Toshiya
	Date   : 2004.09.10 -

	[ MZ-1E30 (SASI I/F) ]
*/

#ifndef _MZ1E30_H_
#define _MZ1E30_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

#define SIG_MZ1E30_IRQ	0
#define SIG_MZ1E30_DRQ	1

namespace MZ2500 {

class MZ1E30 : public DEVICE
{
private:
	DEVICE *d_host;
	
	// rom file
	uint8_t *rom_buffer;
	uint32_t rom_address, rom_size;
	
	// sasi
	bool irq_status;
	bool drq_status;
	
public:
	MZ1E30(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("MZ-1E30 (SASI I/F)"));
	}
	~MZ1E30() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	void __FASTCALL write_dma_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_dma_io8(uint32_t addr);
	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique function
	void set_context_host(DEVICE* device)
	{
		d_host = device;
	}
};

}
#endif

