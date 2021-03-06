/*
	TOSHIBA PASOPIA Emulator 'EmuPIA'

	Author : Takeda.Toshiya
	Date   : 2006.12.28 -

	[ pac slot 2 ]
*/

#ifndef _PAC2_H_
#define _PAC2_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

namespace PASOPIA {
	class PAC2DEV;
	class RAMPAC2;
	class KANJIPAC2;
	class JOYPAC2;
}
	
namespace PASOPIA {

class PAC2 : public DEVICE
{
private:
	int device_type;
	
	PASOPIA::RAMPAC2* rampac2;
	PASOPIA::KANJIPAC2* kanji;
	PASOPIA::JOYPAC2* joy;
	PASOPIA::PAC2DEV* dummy;
	
	PASOPIA::PAC2DEV* get_device();
	
public:
	PAC2(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		set_device_name(_T("PAC2 Slot"));
	}
	~PAC2() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique function
	void open_rampac2(const _TCHAR* file_path);
};

}
#endif

