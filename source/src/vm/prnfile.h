/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2015.12.18-

	[ dummy printer ]
*/

#ifndef _PRNFILE_H_
#define _PRNFILE_H_

//#include "vm.h"
//#include "../emu.h"
#include "device.h"

class FILEIO;

class  DLL_PREFIX PRNFILE : public DEVICE
{
private:
	outputs_t outputs_busy;
	outputs_t outputs_ack;
	
	_TCHAR file_path[_MAX_PATH];
	FILEIO *fio;
	int value, busy_id, ack_id, wait_frames;
	bool strobe, res, busy, ack;

	bool _PRINTER_STROBE_RISING_EDGE;

	void __FASTCALL set_busy(bool value);
	void __FASTCALL set_ack(bool value);
	void open_file();
	void close_file();
	
public:
	PRNFILE(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		initialize_output_signals(&outputs_busy);
		initialize_output_signals(&outputs_ack);
		_PRINTER_STROBE_RISING_EDGE = false;
		set_device_name(_T("DUMMY PRINTER DEVICE"));
	}
	~PRNFILE() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	void event_frame();
	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	uint32_t __FASTCALL read_signal(int ch);
	void __FASTCALL event_callback(int event_id, int err);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void set_context_busy(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_busy, device, id, mask);
	}
	void set_context_ack(DEVICE* device, int id, uint32_t mask)
	{
		register_output_signal(&outputs_ack, device, id, mask);
	}
};

#endif

