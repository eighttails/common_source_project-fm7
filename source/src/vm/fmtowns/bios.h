/*
	FUJITSU FMR-30 Emulator 'eFMR-30'
	FUJITSU FMR-50 Emulator 'eFMR-50'
	FUJITSU FMR-60 Emulator 'eFMR-60'

	Author : Takeda.Toshiya
	Date   : 2008.10.06 -

	[ bios ]
*/

#ifndef _BIOS_H_
#define _BIOS_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

class DISK;

class BIOS : public DEVICE
{
private:
	DEVICE *d_mem, *d_io;
	DISK *disk[MAX_DRIVE];
	
	// pseudo bios
	uint8_t *cmos, *vram, *cvram;
#ifdef _FMR60
	uint8_t *avram;
#else
	uint8_t *kvram;
#endif
	int secnum, timeout;
	
	// disk bios
	bool access_fdd[MAX_DRIVE], access_scsi;
	int scsi_blocks[MAX_SCSI];
	
public:
	BIOS(VM* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu) {
		set_device_name(_T("PSEUDO BIOS"));
	}
	~BIOS() {}
	
	// common functions
	void initialize();
	void reset();
	void event_frame();
	bool bios_call_i86(uint32_t PC, uint16_t regs[], uint16_t sregs[], int32_t* ZeroFlag, int32_t* CarryFlag);
	bool bios_int_i86(int intnum, uint16_t regs[], uint16_t sregs[], int32_t* ZeroFlag, int32_t* CarryFlag);
	uint32_t read_signal(int ch);
	void save_state(FILEIO* state_fio);
	bool load_state(FILEIO* state_fio);
	
	// unique functions
	void set_context_mem(DEVICE* device)
	{
		d_mem = device;
	}
	void set_context_io(DEVICE* device)
	{
		d_io = device;
	}
	void set_disk_handler(int drv, DISK* dsk)
	{
		disk[drv] = dsk;
	}
	void set_cmos_ptr(uint8_t* ptr)
	{
		cmos = ptr;
	}
	void set_vram_ptr(uint8_t* ptr)
	{
		vram = ptr;
	}
	void set_cvram_ptr(uint8_t* ptr)
	{
		cvram = ptr;
	}
#ifdef _FMR60
	void set_avram_ptr(uint8_t* ptr)
	{
		avram = ptr;
	}
#else
	void set_kvram_ptr(uint8_t* ptr)
	{
		kvram = ptr;
	}
#endif
};

#endif
