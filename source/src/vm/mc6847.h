/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2010.08.03-

	[ mc6847 ]
*/

#ifndef _MC6847_H_
#define _MC6847_H_

//#include "vm.h"
//#include "../emu.h"
#include "device.h"

#define SIG_MC6847_AG		0
#define SIG_MC6847_AS		1
#define SIG_MC6847_INTEXT	2
#define SIG_MC6847_GM		3
#define SIG_MC6847_CSS		4
#define SIG_MC6847_INV		5
#define SIG_MC6847_ENABLE	6
#define SIG_MC6847_DISABLE	7

class  DLL_PREFIX MC6847_BASE : public DEVICE
{
protected:
	DEVICE *d_cpu;
	
	// output signals
	outputs_t outputs_vsync;
	outputs_t outputs_hsync;
	
	uint8_t extfont[256 * 16];
	uint8_t sg4[16 * 12];
	uint8_t sg6[64 * 12];
	uint8_t screen[192][256];
	uint8_t *vram_ptr;
	int vram_size;
	scrntype_t palette_pc[16];
	
	bool ag, as;
	bool intext;
	uint8_t gm;
	bool css, inv;
	
	bool vsync, hsync, disp;
	int tWHS;
	bool disabled;
	
	void __FASTCALL set_vsync(bool val);
	void __FASTCALL set_hsync(bool val);
	void __FASTCALL set_disp(bool val);
	void __FASTCALL draw_cg(int xofs, int yofs);
	void __FASTCALL draw_rg(int xofs, int yofs);
	virtual void __FASTCALL draw_alpha();
	
public:
	MC6847_BASE(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		d_cpu = NULL;
		ag = as = intext = css = inv = false;
		gm = 0;
		initialize_output_signals(&outputs_vsync);
		initialize_output_signals(&outputs_hsync);
		set_device_name(_T("MC6847 VIDEO DISPLAY CONTOROLLER"));
	}
	~MC6847_BASE() {}
	
	// common functions
	virtual void initialize();
	void reset();
	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	void event_vline(int v, int clock);
	void __FASTCALL event_callback(int event_id, int err);
	void update_timing(int new_clocks, double new_frames_per_sec, int new_lines_per_frame);
	bool process_state(FILEIO* state_fio, bool loading);

	// unique functions
	void set_context_cpu(DEVICE* device)
	{
		d_cpu = device;
	}
	void set_context_vsync(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_vsync, device, id, mask);
	}
	void set_context_hsync(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_hsync, device, id, mask);
	}
	void set_vram_ptr(uint8_t* ptr, int size)
	{
		vram_ptr = ptr; vram_size = size;
	}
	void load_font_image(const _TCHAR *file_path);
	void draw_screen();
};

class MC6847 : public MC6847_BASE
{
protected:
	void __FASTCALL draw_alpha();
public:
	MC6847(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : MC6847_BASE(parent_vm, parent_emu)
	{
	}
	~MC6847() {};

	void initialize();
};

#endif

