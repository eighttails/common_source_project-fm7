/*
	CASIO PV-1000 Emulator 'ePV-1000'

	Author : Takeda.Toshiya
	Date   : 2006.11.16 -

	[ video processor ]
*/

#ifndef _VDP_H_
#define _VDP_H_

#include "../vm.h"
#include "../../emu_template.h"
#include "../device.h"

namespace PV1000 {

class VDP : public DEVICE
{
private:
	DEVICE* d_cpu;
	
	uint8_t bg[192][256];
	uint8_t* vram;
	uint8_t* pcg;
	uint8_t* pattern;
	uint8_t* base;
	bool force_pattern;

	int tmp_vram_size;
	int tmp_pcg_size;
	int tmp_pattern_size;
	
	void __FASTCALL draw_pattern(int x8, int y8, uint16_t top);
	void __FASTCALL draw_pcg(int x8, int y8, uint16_t top);
	
public:
	VDP(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("VDP"));
	}
	~VDP() {}
	
	// common functions
	void initialize();
	void reset();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	void __FASTCALL event_callback(int event_id, int err);
	void event_vline(int v, int clock);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void set_context_cpu(DEVICE* device)
	{
		d_cpu = device;
	}
	void set_memory_ptr(uint8_t* ptr)
	{
		base = ptr;
		vram = ptr + 0xb800;
		pcg = ptr + 0xbc00;
		pattern = ptr;
	}
	void draw_screen();
};


}
#endif
