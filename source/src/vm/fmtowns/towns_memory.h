/*
	FUJITSU FM Towns Emulator 'eFMTowns'

	Author : Kyuma.Ohta <whatisthis.sowhat _at_ gmail.com>
	Date   : 2017.01.07 -

	[ memory ]
*/

#ifndef _TOWNS_MEMORY_H_
#define _TOWNS_MEMORY_H_

#include "device.h"
#include "../../common.h"
#include "../memory.h"
#include "./towns_common.h"

#define SIG_FMTOWNS_MACHINE_ID	1
#define SIG_MEMORY_EXTNMI       2

// MAP:
// 00000000 - 000fffff : SYSTEM RAM PAGE 0 (Similar to FMR-50).
// 00100000 - 3fffffff : EXTRA RAM (for i386 native mode.Page size is 1MB.)
// 40000000 - 7fffffff : External I/O BOX.Not accessible.
// 80000000 - bfffffff : VRAM (Reserved 01020000H bytes with earlier Towns.)
// c0000000 - c21fffff : ROM card and dictionaly/font/DOS ROMs.
// c2200000 - c2200fff : PCM RAM (Banked).
// c2201000 - fffbffff : Reserved.
// FFFC0000 - FFFFFFFF : Towns System ROM.

namespace FMTOWNS {
enum {
	TOWNS_MEMORY_NORMAL = 0,
	TOWNS_MEMORY_FMR_VRAM,
	TOWNS_MEMORY_FMR_TEXT,
	TOWNS_MEMORY_FMR_VRAM_RESERVE,
	TOWNS_MEMORY_SPRITE_ANKCG1,
	TOWNS_MEMORY_ANKCG2,
	
	TOWNS_MEMORY_MMIO_0CC,
	
	TOWNS_MEMORY_TYPE_EXT_MMIO,
	TOWNS_MEMORY_TYPE_LEARN_RAM,
	TOWNS_MEMORY_TYPE_WAVERAM,
};
}
// Please set from config
#define TOWNS_EXTRAM_PAGES 6

class BEEP;
class I386;

namespace FMTOWNS {
	class TOWNS_VRAM;
	class TOWNS_SPRITE;
	class TOWNS_ROM_CARD;
}
	
namespace FMTOWNS {
class TOWNS_MEMORY : public MEMORY
{
protected:
	DEVICE* d_vram;
	DEVICE* d_sprite;       // 0x81000000 - 0x8101ffff ?
	DEVICE* d_romcard[2]; // 0xc0000000 - 0xc0ffffff / 0xc1000000 - 0xc1ffffff
	DEVICE* d_pcm;             // 0xc2200000 - 0xc2200fff 
	DEVICE* d_beep;
	DEVICE* d_dmac;
	DEVICE* d_crtc;
	DEVICE* d_planevram;
	I386*   d_cpu;
	
	DEVICE* d_dictionary;
	DEVICE* d_sysrom;
	DEVICE* d_msdos;
	DEVICE* d_serialrom;
	DEVICE* d_font;
	DEVICE* d_font_20pix;

	DEVICE* d_iccard[2];

	outputs_t outputs_ram_wait;
	outputs_t outputs_rom_wait;
	
	bool bankc0_vram;
	bool ankcg_enabled;
	bool select_d0_rom;
	bool select_d0_dict;
	
	uint16_t machine_id;
	uint8_t cpu_id;
	bool is_compatible;
	bool dma_is_vram;

	// RAM
	uint8_t ram_page0[0xc0000];       // 0x00000000 - 0x000bffff : RAM
	uint8_t ram_pagec[0x10000];       // 0x000c0000 - 0x000cffff : URA? RAM
	uint8_t ram_paged[0x10000];       // 0x000d0000 - 0x000dffff : RAM
	uint8_t ram_pagee[0x10000];       // 0x000e0000 - 0x000effff : RAM
	uint8_t ram_pagef[0x10000];       // 0x000f0000 - 0x000f8fff : RAM

	uint8_t *extra_ram;                  // 0x00100000 - (0x3fffffff) : Size is defined by extram_size;
	uint32_t extram_size;
	uint32_t cpu_clock_val;
	uint32_t vram_wait_val;
	uint32_t mem_wait_val;

	uint8_t wait_register;
	
	bool extra_nmi_mask;
	bool extra_nmi_val;
	bool nmi_mask;
	bool software_reset;
	bool nmi_vector_protect;
	bool poff_status;
	bool reset_happened;
	
	// misc
	uint32_t vram_size; // Normally 512KB.
	bool initialized;

	virtual void set_wait_values();
	virtual void config_page00();

	inline uint32_t __FASTCALL read_primitive_byte(uint32_t addr)
	{
		int bank = (addr & addr_mask) >> addr_shift;
		if(rd_table[bank].device != NULL) {
			return rd_table[bank].device->read_memory_mapped_io8(addr);
		} else {
			return rd_table[bank].memory[addr & bank_mask];
		}
	}
	
	inline uint32_t __FASTCALL read_primitive_word(uint32_t addr)
	{
		int bank = (addr & addr_mask) >> addr_shift;
		pair32_t n;
		uint32_t naddr = addr & bank_mask;
		n.d = 0;

		if((addr & bank_mask) == bank_mask) {
			if(rd_table[bank].device != NULL) {
				n.b.l = rd_table[bank].device->read_memory_mapped_io8(addr);
			} else {
				n.b.l = rd_table[bank].memory[naddr + 0];
			}
			n.b.h = read_primitive_byte(addr + 1);
		} else {
			if(rd_table[bank].device != NULL) {
				n.w.l = rd_table[bank].device->read_memory_mapped_io16(addr);
			} else {
				n.b.l = rd_table[bank].memory[naddr + 0];
				n.b.h = rd_table[bank].memory[naddr + 1];
			}
		}
		return n.d;
	}
	inline uint32_t __FASTCALL read_primitive_dword(uint32_t addr)
	{
		int bank = (addr & addr_mask) >> addr_shift;
		pair32_t n;
		uint32_t naddr = addr & bank_mask;
		n.d = 0;
		if((addr & bank_mask) > (bank_mask - 3)) {
			switch(addr & 3) {
			case 0:
				n.w.l = read_primitive_word(addr + 0);
				n.w.h = read_primitive_word(addr + 2);
				break;
			case 1:
				n.b.l  = read_primitive_byte(addr + 0);
				n.b.h  = read_primitive_byte(addr + 1);
				n.b.h2 = read_primitive_byte(addr + 2);
				n.b.h3 = read_primitive_byte(addr + 3);
				break;
			case 2:
				n.w.l = read_primitive_word(addr + 0);
				n.w.h = read_primitive_word(addr + 2);
				break;
			case 3:
				n.b.l  = read_primitive_byte(addr + 0);
				n.b.h  = read_primitive_byte(addr + 1);
				n.b.h2 = read_primitive_byte(addr + 2);
				n.b.h3 = read_primitive_byte(addr + 3);
				break;
			}
			return n.d;
		} else { // Inside of boundary
			if(rd_table[bank].device != NULL) {
				return rd_table[bank].device->read_memory_mapped_io32(addr);
			} else {
				n.b.l  = rd_table[bank].memory[naddr + 0];
				n.b.h  = rd_table[bank].memory[naddr + 1];
				n.b.h2 = rd_table[bank].memory[naddr + 2];
				n.b.h3 = rd_table[bank].memory[naddr + 3];
			}
			return n.d;
		}
	}
	inline void __FASTCALL write_primitive_byte(uint32_t addr, uint32_t data)
	{
		int bank = (addr & addr_mask) >> addr_shift;
//		if(addr == 0xd000) {
//			out_debug_log(_T("WRITE  %02X to D000 / DEV=%08X"), data, wr_table[bank].device);
//		}
		if(wr_table[bank].device != NULL) {
			wr_table[bank].device->write_memory_mapped_io8(addr, data);
		} else {
			wr_table[bank].memory[addr & bank_mask] = data;
		}
	}
	
	inline void __FASTCALL write_primitive_word(uint32_t addr, uint32_t data)
	{
		int bank = (addr & addr_mask) >> addr_shift;
		
		if(wr_table[bank].device != NULL) {
			if((addr & bank_mask) == bank_mask) {
				pair32_t n;
				n.d = data;
				wr_table[bank].device->write_memory_mapped_io8(addr + 0, n.b.l);
				write_primitive_byte(addr + 1, n.b.h);
			} else {
				wr_table[bank].device->write_memory_mapped_io16(addr, data);
			}
		} else {
			pair32_t n;
			uint32_t naddr = addr & bank_mask;
			n.d = data;
			if((addr & bank_mask) == bank_mask) {
				wr_table[bank].memory[naddr + 0] = n.b.l;
				write_primitive_byte(addr + 1, n.b.h);
			} else {
				wr_table[bank].memory[naddr + 0] = n.b.l;
				wr_table[bank].memory[naddr + 1] = n.b.h;
			}
		}
	}
	inline void __FASTCALL write_primitive_dword(uint32_t addr, uint32_t data)
	{
		int bank = (addr & addr_mask) >> addr_shift;
		uint32_t naddr = addr & bank_mask;
		pair32_t n;
		n.d = data;
		if((addr & bank_mask) <= (bank_mask - 3)) {
			if(wr_table[bank].device != NULL) {
				wr_table[bank].device->write_memory_mapped_io32(addr, data);
			} else {
				wr_table[bank].memory[naddr + 0] = n.b.l;
				wr_table[bank].memory[naddr + 1] = n.b.h;
				wr_table[bank].memory[naddr + 2] = n.b.h2;
				wr_table[bank].memory[naddr + 3] = n.b.h3;
			}
		} else {
			// BEYOND BOUNDARY
			switch(addr & 3) {
			case 0:
				write_primitive_word(addr + 0, n.w.l);
				write_primitive_word(addr + 2, n.w.h);
				break;
			case 1:
				{
					write_primitive_byte(addr + 0, n.b.l);
					pair16_t nn;
					nn.b.l = n.b.h;
					nn.b.h = n.b.h2;
					write_primitive_word(addr + 1, nn.w);
					write_primitive_byte(addr + 3, n.b.h3);
				}
				break;
			case 2:
				write_primitive_word(addr + 0, n.w.l);
				write_primitive_word(addr + 2, n.w.h);
				break;
			case 3:
				{
					write_primitive_byte(addr + 0, n.b.l);
					pair16_t nn;
					nn.b.l = n.b.h;
					nn.b.h = n.b.h2;
					write_primitive_word(addr + 1, nn.w);
					write_primitive_word(addr + 3, n.b.h3);
				}
				break;
			}
		}
	}
public:
	TOWNS_MEMORY(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : MEMORY(parent_vm, parent_emu) {
		set_device_name(_T("FMTOWNS_MEMORY"));
		addr_max = 0x100000000; // 4GiB
		bank_size = 1024; // 1024
		addr_shift = 10;
		bank_size_was_set = false;
		addr_max_was_set = false;
		
		_MEMORY_DISABLE_DMA_MMIO = false;
		
		extram_size = 0x00200000; // Basically 2MB
		
		d_cpu = NULL;
		
		d_vram = NULL;
		d_sprite = NULL;
		d_romcard[0] = d_romcard[1] = NULL;
		d_pcm = NULL;
		d_beep = NULL;
		d_dmac = NULL;
		d_crtc = NULL;
		d_planevram = NULL;
		d_iccard[0] = NULL;
		d_iccard[1] = NULL;
		
		d_dictionary = NULL;
		d_sysrom = NULL;
		d_msdos = NULL;
		d_serialrom = NULL;
		d_font = NULL;
		d_font_20pix = NULL;
		initialized = false;

		initialize_output_signals(&outputs_ram_wait);
		initialize_output_signals(&outputs_rom_wait);
		// Note: machine id must set before initialize() from set_context_machine_id() by VM::VM().
		// machine_id = 0x0100;   // FM-Towns 1,2
		// machine_id = 0x0200 // FM-Towns  1F/2F/1H/2H
		// machine_id = 0x0300 // FM-Towns  UX10/UX20/UX40
		// machine_id = 0x0400 // FM-Towns  10F/20F/40H/80H
		// machine_id = 0x0500 // FM-Towns2 CX10/CX20/CX40/CX100
		// machine_id = 0x0600 // FM-Towns2 UG10/UG20/UG40/UG80
		// machine_id = 0x0700 // FM-Towns2 HR20/HR100/HR200
		// machine_id = 0x0800 // FM-Towns2 HG20/HG40/HG100
		// machine_id = 0x0900 // FM-Towns2 UR20/UR40/UR80
		// machine_id = 0x0b00 // FM-Towns2 MA20/MA170/MA340
		// machine_id = 0x0c00 // FM-Towns2 MX20/MX170/MX340
		// machine_id = 0x0d00 // FM-Towns2 ME20/ME170
		// machine_id = 0x0f00 // FM-Towns2 MF20/MF170/Fresh
		machine_id = 0x0100;   // FM-Towns 1,2
		
		// Note: cpu id must set before initialize() from set_context_cpu_id() by VM::VM().
		// cpu_id = 0x00; // 80286. 
		// cpu_id = 0x01; // 80386DX. 
		// cpu_id = 0x02; // 80486SX/DX. 
		// cpu_id = 0x03; // 80386SX. 
		cpu_id = 0x01; // 80386DX. 
		extra_ram = NULL;
	}
	~TOWNS_MEMORY() {}
	
	// common functions
	void initialize();
	void release();
	void reset();
	virtual void     __FASTCALL write_io8(uint32_t addr, uint32_t data);
	virtual uint32_t __FASTCALL read_io8(uint32_t addr);
	virtual uint32_t __FASTCALL read_io16(uint32_t addr);

	uint32_t __FASTCALL read_data8(uint32_t addr);
	uint32_t __FASTCALL read_data16(uint32_t addr);
	uint32_t __FASTCALL read_data32(uint32_t addr);
	void __FASTCALL write_data8(uint32_t addr, uint32_t data);
	void __FASTCALL write_data16(uint32_t addr, uint32_t data);
	void __FASTCALL write_data32(uint32_t addr, uint32_t data);
	
	uint32_t __FASTCALL read_data8w(uint32_t addr, int* wait);
	uint32_t __FASTCALL read_data16w(uint32_t addr, int* wait);
	uint32_t __FASTCALL read_data32w(uint32_t addr, int* wait);
	void __FASTCALL write_data8w(uint32_t addr, uint32_t data, int* wait);
	void __FASTCALL write_data16w(uint32_t addr, uint32_t data, int* wait);
	void __FASTCALL write_data32w(uint32_t addr, uint32_t data, int* wait);

	uint32_t __FASTCALL read_dma_data8w(uint32_t addr, int* wait);
	uint32_t __FASTCALL read_dma_data16w(uint32_t addr, int* wait);
	uint32_t __FASTCALL read_dma_data32w(uint32_t addr, int* wait);
	void __FASTCALL write_dma_data8w(uint32_t addr, uint32_t data, int* wait);
	void __FASTCALL write_dma_data16w(uint32_t addr, uint32_t data, int* wait);
	void __FASTCALL write_dma_data32w(uint32_t addr, uint32_t data, int* wait);
	
	uint32_t __FASTCALL read_dma_data8(uint32_t addr);
	uint32_t __FASTCALL read_dma_data16(uint32_t addr);
	uint32_t __FASTCALL read_dma_data32(uint32_t addr);
	void __FASTCALL write_dma_data8(uint32_t addr, uint32_t data);
	void __FASTCALL write_dma_data16(uint32_t addr, uint32_t data);
	void __FASTCALL write_dma_data32(uint32_t addr, uint32_t data);
	
	virtual void __FASTCALL write_memory_mapped_io8(uint32_t addr, uint32_t data);
//	virtual void __FASTCALL write_memory_mapped_io16(uint32_t addr, uint32_t data);
//	virtual void __FASTCALL write_memory_mapped_io32(uint32_t addr, uint32_t data);
	virtual uint32_t __FASTCALL read_memory_mapped_io8(uint32_t addr);
//	virtual uint32_t __FASTCALL read_memory_mapped_io16(uint32_t addr);
//	virtual uint32_t __FASTCALL read_memory_mapped_io32(uint32_t addr);


	void __FASTCALL write_signal(int id, uint32_t data, uint32_t mask);
	uint32_t __FASTCALL read_signal(int ch);
	
	//void event_frame();
	virtual void __FASTCALL set_intr_line(bool line, bool pending, uint32_t bit);
	
	bool process_state(FILEIO* state_fio, bool loading);
	
	// unique functions
	void set_extra_ram_size(uint32_t megabytes)
	{
		uint32_t limit = 5;
		uint32_t minimum = 2;
		switch(machine_id & 0xff00) {
		case 0x0000: // ???
		case 0x0100: // Towns Model 1/2
			minimum = 1;
			break;
		case 0x0200: // TOWNS 2F/2H
		case 0x0400: // TOWNS 10F/10H/20F/20H
			limit = 8; // 2MB + 2MB x 3
			break;
		case 0x0300: // TOWNS2 UX
		case 0x0600: // TOWNS2 UG
			limit = 9; // 2MB + 4MB x 2? - 1MB
			break;
		case 0x0500: // TOWNS2 CX
		case 0x0800: // TOWNS2 HG
			limit = 15; // 8MB x 2 - 1MB?
			break;
		case 0x0700: // TOWNS2 HR
		case 0x0900: // TOWNS2 UR
		case 0x0B00: // TOWNS2 MA
		case 0x0C00: // TOWNS2 MX
		case 0x0D00: // TOWNS2 ME
		case 0x0F00: // TOWNS2 MF/Fresh
			limit = 31; // 16MB x 2 - 1MB? 
			break;
		}
		if(megabytes > limit) megabytes = limit;
		if(megabytes < minimum) megabytes = minimum;
		extram_size = megabytes << 20;
	}
	void set_context_cpu(I386* device)
	{
		d_cpu = device;
	}
	void set_context_dmac(DEVICE* device)
	{
		d_dmac = device;
	}
	void set_context_vram(DEVICE* device)
	{
		d_vram = device;
	}
	void set_context_system_rom(DEVICE* device)
	{
		d_sysrom = device;
	}
	void set_context_font_rom(DEVICE* device)
	{
		d_font = device;
	}
	void set_context_font_20pix_rom(DEVICE* device)
	{
		d_font_20pix = device;
	}
	void set_context_dictionary(DEVICE* device)
	{
		d_dictionary = device;
	}
	void set_context_msdos(DEVICE* device)
	{
		d_msdos = device;
	}
	void set_context_beep(DEVICE* device)
	{
		d_beep = device;
	}
	void set_context_sprite(DEVICE* device)
	{
		d_sprite = device;
	}
	void set_context_crtc(DEVICE* device)
	{
		d_crtc = device;
	}
	void set_context_romcard(DEVICE* device, int num)
	{
		d_romcard[num & 1] = device;
	}
	void set_context_pcm(DEVICE* device)
	{
		d_pcm = device;
	}
	void set_context_serial_rom(DEVICE* device)
	{
		d_serialrom = device;
	}
	void set_context_planevram(DEVICE *dev)
	{
		d_planevram = dev;
	}
	void set_context_iccard(DEVICE* device, int num)
	{
		if((num >= 0) && (num < 2)) {
			d_iccard[num] = device;
		}
	}
	void set_machine_id(uint16_t val)
	{
		machine_id = val & 0xfff8;
	}
	void set_cpu_id(uint16_t val)
	{
		cpu_id = val & 0x07;
	}
};

}
#endif

