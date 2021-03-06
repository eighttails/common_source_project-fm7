/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2008.04.19-

	[ 74LS244 / 74LS245 ]
*/

#ifndef _LS244_H_
#define _LS244_H_

#include "device.h"

#define SIG_LS244_INPUT	0

class  DLL_PREFIX LS244 : public DEVICE
{
private:
	// output signals
	outputs_t outputs;
	
	uint8_t din;
	
public:
	LS244(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		initialize_output_signals(&outputs);
		set_device_name(_T("74LS244 Octal 3-State Buffer"));
	}
	~LS244() {}
	
	// common functions
	void initialize();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique function
	void set_context_output(DEVICE* device, int id, uint32_t mask, int shift)
	{
		register_output_signal(&outputs, device, id, mask, shift);
	}
};

#endif

