/*
	Common Source Code Project
	MSX Series (experimental)

	Origin : Zodiac
	Author : umaiboux
	Date   : 2016.03.xx-

	[ Kanji Rom ]
*/

#ifndef _KANJIROM_H_
#define _KANJIROM_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

namespace MSX {

class KANJIROM : public DEVICE
{
private:
	uint8_t rom[256*1024];
	int index;

public:
	KANJIROM(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu);
	~KANJIROM() {}
	
	// common functions
	void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8(uint32_t addr);
	
	// unique functions
};

}
#endif


