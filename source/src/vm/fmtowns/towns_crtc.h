/*
	Skelton for retropc emulator

	Author : Kyuma Ohta <whatisthis.sowhat _at_ gmail.com>
	Date   : 2016.12.28 -

	[ FM-Towns CRTC ]
	History: 2016.12.28 Initial from HD46505 .
*/

#ifndef _TOWNS_CRTC_H_
#define _TOWNS_CRTC_H_

#include "../vm.h"
#include "../emu.h"
#include "device.h"

class TOWNS_CRTC : public DEVICE
{
private:
	// output signals
	outputs_t outputs_disp;
	outputs_t outputs_vblank;
	outputs_t outputs_vsync;
	outputs_t outputs_hsync;
	
	uint8_t regs[32];
	bool regs_written[32];
	int ch;
	bool timing_changed;
	
	int cpu_clocks;
#if defined(TOWNS_CRTC_CHAR_CLOCK)
	double char_clock, next_char_clock;
#elif defined(TOWNS_CRTC_HORIZ_FREQ)
	double horiz_freq, next_horiz_freq;
#endif
	double frames_per_sec;
	
	int hz_total, hz_disp;
	int hs_start, hs_end;
	
	int vt_total, vt_disp;
	int vs_start, vs_end;
	
	int disp_end_clock;
	int hs_start_clock, hs_end_clock;
	
	bool display, vblank, vsync, hsync;
	
	void set_display(bool val);
	void set_vblank(bool val);
	void set_vsync(bool val);
	void set_hsync(bool val);
	
public:
	TOWNS_CRTC(VM *parent_vm, EMU *parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		initialize_output_signals(&outputs_disp);
		initialize_output_signals(&outputs_vblank);
		initialize_output_signals(&outputs_vsync);
		initialize_output_signals(&outputs_hsync);
		set_device_name(_T("FM-Towns CRTC"));
	}
	~TOWNS_CRTC() {}

	void initialize();
	void reset();
	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);
	void event_pre_frame();
	void event_frame();
	void event_vline(int v, int clock);
	void event_callback(int event_id, int err);
	void update_timing(int new_clocks, double new_frames_per_sec, int new_lines_per_frame);
	void save_state(FILEIO* state_fio);
	bool load_state(FILEIO* state_fio);
	// unique function
	void set_context_disp(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_disp, device, id, mask);
	}
	void set_context_vblank(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_vblank, device, id, mask);
	}
	void set_context_vsync(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_vsync, device, id, mask);
	}
	void set_context_hsync(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_hsync, device, id, mask);
	}
#if defined(TOWNS_CRTC_CHAR_CLOCK)
	void set_char_clock(double clock)
	{
		next_char_clock = clock;
	}
#elif defined(TOWNS_CRTC_HORIZ_FREQ)
	void set_horiz_freq(double freq)
	{
		next_horiz_freq = freq;
	}
#endif
	uint8_t* get_regs()
	{
		return regs;
	}

};

#endif
