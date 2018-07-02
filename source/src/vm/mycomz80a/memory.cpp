/*
	Japan Electronics College MYCOMZ-80A Emulator 'eMYCOMZ-80A'

	Author : Takeda.Toshiya
	Date   : 2009.05.13-

	[ memory ]
*/

#include "memory.h"

#define SET_BANK_W(s, e, w) { \
	int sb = (s) >> 12, eb = (e) >> 12; \
	for(int i = sb; i <= eb; i++) { \
		if((w) == wdmy) { \
			wbank[i] = wdmy; \
		} else { \
			wbank[i] = (w) + 0x1000 * (i - sb); \
		} \
	} \
}
#define SET_BANK_R(s, e, r) { \
	int sb = (s) >> 12, eb = (e) >> 12; \
	for(int i = sb; i <= eb; i++) { \
		if((r) == rdmy) { \
			rbank[i] = rdmy; \
		} else { \
			rbank[i] = (r) + 0x1000 * (i - sb); \
		} \
	} \
}

void MEMORY::initialize()
{
	// init memory
	memset(ram, 0, sizeof(ram));
	memset(bios, 0xff, sizeof(bios));
	memset(basic, 0xff, sizeof(basic));
	memset(rdmy, 0xff, sizeof(rdmy));
	
	// load rom images
	FILEIO* fio = new FILEIO();
	if(fio->Fopen(create_local_path(_T("BIOS.ROM")), FILEIO_READ_BINARY)) {
		fio->Fread(bios, sizeof(bios), 1);
		fio->Fclose();
	}
	if(fio->Fopen(create_local_path(_T("BASIC.ROM")), FILEIO_READ_BINARY)) {
		fio->Fread(basic, sizeof(basic), 1);
		fio->Fclose();
	}
	delete fio;
	
	SET_BANK_W(0x0000, 0xffff, ram);
	SET_BANK_R(0x0000, 0xffff, ram);
}

void MEMORY::reset()
{
	addr_mask = 0xc000;
	rom_sel = true;
	update_memory_map();
}

void MEMORY::write_data8(uint32_t addr, uint32_t data)
{
	addr = (addr & 0xffff) | addr_mask;
	wbank[addr >> 12][addr & 0xfff] = data;
}

uint32_t MEMORY::read_data8(uint32_t addr)
{
	addr = (addr & 0xffff) | addr_mask;
	return rbank[addr >> 12][addr & 0xfff];
}

void MEMORY::write_io8(uint32_t addr, uint32_t data)
{
	// $00: system control
	switch(data) {
	case 0:
		addr_mask = 0xc000;
		break;
	case 1:
		addr_mask = 0;
		break;
	case 2:
		rom_sel = true;
		update_memory_map();
		break;
	case 3:
		rom_sel = false;
		update_memory_map();
		break;
	}
}

void MEMORY::update_memory_map()
{
	if(rom_sel) {
		SET_BANK_R(0xc000, 0xefff, bios);
		SET_BANK_R(0xf000, 0xffff, basic);
	} else {
		SET_BANK_R(0xc000, 0xffff, ram + 0xc000);
	}
}

#define STATE_VERSION	1

#include "../../statesub.h"

void MEMORY::decl_state()
{
	enter_decl_state(STATE_VERSION);

	DECL_STATE_ENTRY_1D_ARRAY(ram, sizeof(ram));
	DECL_STATE_ENTRY_UINT32(addr_mask);
	DECL_STATE_ENTRY_BOOL(rom_sel);

	leave_decl_state();
}

void MEMORY::save_state(FILEIO* state_fio)
{
	if(state_entry != NULL) {
		state_entry->save_state(state_fio);
	}
//	state_fio->FputUint32(STATE_VERSION);
//	state_fio->FputInt32(this_device_id);
	
//	state_fio->Fwrite(ram, sizeof(ram), 1);
//	state_fio->FputUint32(addr_mask);
//	state_fio->FputBool(rom_sel);
}

bool MEMORY::load_state(FILEIO* state_fio)
{
	bool mb = false;
	if(state_entry != NULL) {
		mb = state_entry->load_state(state_fio);
	}
	if(!mb) {
		return false;
	}
//	if(state_fio->FgetUint32() != STATE_VERSION) {
//		return false;
//	}
//	if(state_fio->FgetInt32() != this_device_id) {
//		return false;
//	}
//	state_fio->Fread(ram, sizeof(ram), 1);
//	addr_mask = state_fio->FgetUint32();
//	rom_sel = state_fio->FgetBool();
	
	// post process
	update_memory_map();
	return true;
}

