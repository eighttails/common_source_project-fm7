/*
 * Main memory for FM-7 [FM7_MAINMEM/MAINMEM_PAGE2]
 *  Author: K.Ohta
 *  Date  : 2017.04.01-
 *  License: GPLv2
 *
 */
#include "vm.h"
#include "emu.h"
#include "fm7_mainmem.h"

int FM7_MAINMEM::check_page2(uint32_t addr, uint32_t *realaddr, bool write_state, bool dmamode)
{
#if defined(_FM77AV40) || defined(_FM77AV40EX) || defined(_FM77AV40SX)
	uint32_t mmr_bank = (addr >> 12) & 0x3f;
	uint32_t raddr = addr & 0x00fff;
	if((mmr_bank == 0x2e) && (!write_state)) {
		int banknum = check_extrom(raddr, realaddr);
		if(banknum >= 0) {
			return banknum;
		} else {
#  if defined(CAPABLE_DICTROM)
			if(dictrom_connected && dictrom_enabled) { // Dictionary ROM
				uint32_t dbank = extcard_bank & 0x3f;
				*realaddr = raddr | (dbank << 12);
				return FM7_MAINMEM_DICTROM;
			}
#  else
			*realaddr = 0;
			return FM7_MAINMEM_NULL;
#  endif			   
		}
	}
#  if defined(CAPABLE_DICTROM)
	switch(mmr_bank) {
	case 0x28:
	case 0x29: // Backuped RAM
		if(dictrom_connected && dictram_enabled){ // Battery backuped RAM
			raddr = (((uint32_t)mmr_bank & 0x01) << 12) | raddr;
			raddr =  raddr & 0x1fff;
			*realaddr = raddr;
			return FM7_MAINMEM_BACKUPED_RAM;
		}
		break;
	}
	*realaddr = (raddr | (mmr_bank << 12)) & 0x0ffff;
	return FM7_MAINMEM_AV_PAGE2;
#  else
	if(use_page2_extram) {
		*realaddr = ((mmr_bank << 12) | raddr) & 0x0ffff;
		return FM7_MAINMEM_AV_PAGE2;
	} else {
		*realaddr = 0;
		return FM7_MAINMEM_NULL;
	}
#  endif
#endif
	return -1;
}

uint8_t FM7_MAINMEM::read_page2(uint32_t addr, bool dmamode)
{
#if defined(_FM77AV40) || defined(_FM77AV40EX) || defined(_FM77AV40SX)
	uint32_t raddr;
	int stat;
	addr = (addr & 0xffff) | 0x20000; 
	stat = check_page2(addr, &raddr, false, dmamode);
	if(stat < 0) return 0xff;
	switch(stat) {
	case FM7_MAINMEM_AV_PAGE2:
		return fm7_mainmem_mmrbank_2[raddr & 0xffff];
		break;
	case FM7_MAINMEM_KANJI_DUMMYADDR:
		return raddr & 0x01;
		break;
	case FM7_MAINMEM_KANJI_LEVEL1:
		return read_kanjirom_level1(raddr, dmamode);
		break;
	case FM7_MAINMEM_77AV40_EXTRAROM:
		return fm7_mainmem_extrarom[raddr & 0xffff];
		break;
	case FM7_MAINMEM_BASICROM:
		return fm7_mainmem_basicrom[raddr & 0x7fff];
		break;
	case FM7_MAINMEM_BOOTROM_MMR:
		return fm77av_hidden_bootmmr[raddr & 0x01ff];
		break;
	case FM7_MAINMEM_RESET_VECTOR:
		return fm7_mainmem_reset_vector[raddr & 0x01];
		break;
	case FM7_MAINMEM_DICTROM:
		return fm7_mainmem_dictrom[raddr];
		break;
	case FM7_MAINMEM_BACKUPED_RAM:
		return fm7_mainmem_learndata[raddr & 0x1fff];
		break;
	case FM7_MAINMEM_NULL:
		return 0xff;
		break;
	default:
		return 0xff;
		break;
	}
#else
	return 0xff;
#endif
}

void FM7_MAINMEM::write_page2(uint32_t addr, uint32_t data, bool dmamode)
{
#if defined(_FM77AV40) || defined(_FM77AV40EX) || defined(_FM77AV40SX)
	uint32_t raddr;
	int stat;
	addr = (addr & 0xffff) | 0x20000; 
	stat = check_page2(addr, &raddr, true, dmamode);
	if(stat < 0) return;
	switch(stat) {
	case FM7_MAINMEM_AV_PAGE2:
		fm7_mainmem_mmrbank_2[raddr & 0xffff] = (uint8_t)data;
		break;
	case FM7_MAINMEM_RESET_VECTOR:
		fm7_mainmem_reset_vector[raddr & 0x01] = (uint8_t)data;
		break;
	case FM7_MAINMEM_BACKUPED_RAM:
		fm7_mainmem_learndata[raddr & 0x1fff] = (uint8_t)data;
		break;
	default:
		break;
	}
#endif
}

uint8_t FM7_MAINMEM::read_kanjirom_level1(uint32_t realaddr, bool dmamode)
{
#if defined(CAPABLE_DICTROM)
	return kanjiclass1->read_data8(KANJIROM_DIRECTADDR + realaddr);
#else
	return 0xff;
#endif
}

uint8_t FM7_MAINMEM::read_kanji_dummyaddr(uint32_t realaddr, bool dmamode)
{
		return (realaddr & 0x01);
}
