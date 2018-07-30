/*
	SHARP MZ-2800 Emulator 'EmuZ-2800'

	Author : Takeda.Toshiya
	Date   : 2015.01.17 -

	[ serial ]
*/

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

class Z80SIO;

class SERIAL : public DEVICE
{
private:
	Z80SIO* d_sio;
	bool addr_a0;
	
public:
	SERIAL(VM_TEMPLATE* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("Serial I/F"));
	}
	~SERIAL() {}
	
	// common functions
	void reset();
	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);
	void decl_state();
	void save_state(FILEIO* state_fio);
	bool load_state(FILEIO* state_fio);
	
	// unique functions
	void set_context_sio(Z80SIO* device)
	{
		d_sio = device;
	}
};

#endif

