/*
	Common Source Code Project
	MSX Series (experimental)

	Author : umaiboux
	Date   : 2016.03.xx-

	[ Sound Manager for Cartridge ]
*/

#ifndef _SOUND_CART_H_
#define _SOUND_CART_H_

#include "../vm.h"
#include "../../emu.h"
#include "../device.h"

#include "scc.h"

#define	SOUND_CHIP_SCC 0
#define	SOUND_CHIP_MAX 0

namespace MSX {

class SOUND_CART : public DEVICE
{
private:
	DEVICE *d_chip[SOUND_CHIP_MAX+1];
	bool enable_chip[SOUND_CHIP_MAX+1];

public:
	SOUND_CART(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu);
	~SOUND_CART() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	void __FASTCALL mix(int32_t* buffer, int cnt);
	void set_volume(int ch, int decibel_l, int decibel_r);
	
	// unique functions
	void initialize_sound(int rate, int clock, int samples);
	void disable_all();
	void __FASTCALL enable_c(int chip, bool enable);
	void __FASTCALL write_io8_c(int chip, uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_io8_c(int chip, uint32_t addr);
	void __FASTCALL write_data8_c(int chip, uint32_t addr, uint32_t data);
	uint32_t __FASTCALL read_data8_c(int chip, uint32_t addr);
};

}
#endif

