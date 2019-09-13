/*
	Skelton for retropc emulator

	Origin : MAME i286 core
	Author : Takeda.Toshiya
	Date  : 2012.10.18-

	[ i286 ]
*/

#include "i86.h"
#include "debugger.h"

#include "i80x86_commondefs.h"

/* ----------------------------------------------------------------------------
	MAME i86
---------------------------------------------------------------------------- */

// Note:
// API of bios_int_i86() / bios_caii_i86() has changed.
// regs[8] regs[9] are added.These entries set redirect-address by PSEUDO-BIOS.
// If need, will add more entries for cycle#.
// - 20181126 K.O



//#if defined(HAS_I86) || defined(HAS_I88) || defined(HAS_I186) || defined(HAS_V30)
	#define cpu_state i8086_state
	#include "mame/emu/cpu/i86/i86.c"
	#include "mame/emu/cpu/i386/i386dasm.c"
//#elif defined(HAS_I286)
//#define cpu_state i80286_state
//#include "mame/emu/cpu/i86/i286.c"
//#endif
//#ifdef USE_DEBUGGER
//#ifdef HAS_V30
//#include "mame/emu/cpu/nec/necdasm.c"
//#else
//#endif
//#endif

void I8086::initialize()
{
	DEVICE::initialize();
	n_cpu_type = N_CPU_TYPE_I8086;
	_HAS_i80286 = false;
	_HAS_v30 = false;
	if(osd->check_feature("HAS_I86")) {
		n_cpu_type = N_CPU_TYPE_I8086;
	} else if(osd->check_feature("HAS_I88")) {
		n_cpu_type = N_CPU_TYPE_I8088;
	} else if(osd->check_feature("HAS_I186")) {
		n_cpu_type = N_CPU_TYPE_I80186;
	}
	switch(n_cpu_type) {
	case N_CPU_TYPE_I8086:
		set_device_name(_T("i8086 CPU"));
		opaque = CPU_INIT_CALL( i8086 );
		break;
	case N_CPU_TYPE_I8088:
		set_device_name(_T("i8088 CPU"));
		opaque = CPU_INIT_CALL( i8088 );
		break;
	case N_CPU_TYPE_I80186:
		set_device_name(_T("i80186 CPU"));
		opaque = CPU_INIT_CALL( i80186 );
		break;
	}	
	
	cpu_state *cpustate = (cpu_state *)opaque;
	cpustate->pic = d_pic;
	cpustate->program = d_mem;
	cpustate->io = d_io;
//#ifdef I86_PSEUDO_BIOS
	cpustate->bios = d_bios;
//#endif
//#ifdef SINGLE_MODE_DMA
	cpustate->dma = d_dma;
//#endif
//#ifdef USE_DEBUGGER
	cpustate->emu = emu;
	cpustate->debugger = d_debugger;
	cpustate->program_stored = d_mem;
	cpustate->io_stored = d_io;

	if(d_debugger != NULL) {
		d_debugger->set_context_mem(d_mem);
		d_debugger->set_context_io(d_io);
	}
}

void I8086::release()
{
	free(opaque);
}
void I8086::cpu_reset_generic()
{
	cpu_state *cpustate = (cpu_state *)opaque;
	switch(n_cpu_type) {
	case N_CPU_TYPE_I8086:
		CPU_RESET_CALL( i8086 );
		break;
	case N_CPU_TYPE_I8088:
		CPU_RESET_CALL( i8088 );
		break;
	case N_CPU_TYPE_I80186:
		CPU_RESET_CALL( i80186 );
		break;
	}	
}

void I8086::reset()
{
	cpu_state *cpustate = (cpu_state *)opaque;
	int busreq = cpustate->busreq;
	int haltreq = cpustate->haltreq;

	cpu_reset_generic();
	
	cpustate->pic = d_pic;
	cpustate->program = d_mem;
	cpustate->io = d_io;
//#ifdef I86_PSEUDO_BIOS
	cpustate->bios = d_bios;
//#endif
//#ifdef SINGLE_MODE_DMA
	cpustate->dma = d_dma;
//#endif
//#ifdef USE_DEBUGGER
	cpustate->emu = emu;
	cpustate->debugger = d_debugger;
	cpustate->program_stored = d_mem;
	cpustate->io_stored = d_io;
//#endif
	cpustate->busreq = busreq;
	cpustate->haltreq = haltreq;
}

int I8086::run(int icount)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	int ret = 0;
	switch(n_cpu_type) {
	case N_CPU_TYPE_I8086:
	case N_CPU_TYPE_I8088:
		ret = CPU_EXECUTE_CALL( i8086 );
		break;
	case N_CPU_TYPE_I80186:
		ret = CPU_EXECUTE_CALL( i80186 );
		break;
	}	
	return ret;
}

uint32_t I8086::read_signal(int id)
{
	if((id == SIG_CPU_TOTAL_CYCLE_HI) || (id == SIG_CPU_TOTAL_CYCLE_LO)) {
		cpu_state *cpustate = (cpu_state *)opaque;
		pair64_t n;
		if(cpustate != NULL) {
			n.q = cpustate->total_icount;
		} else {
			n.q = 0;
		}
		if(id == SIG_CPU_TOTAL_CYCLE_HI) {
			return n.d.h;
		} else {
			return n.d.l;
		}
	}
	return 0;
}

void I8086::write_signal(int id, uint32_t data, uint32_t mask)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	
	if(id == SIG_CPU_NMI) {
		set_irq_line(cpustate, INPUT_LINE_NMI, (data & mask) ? HOLD_LINE : CLEAR_LINE);
	} else if(id == SIG_CPU_IRQ) {
		set_irq_line(cpustate, INPUT_LINE_IRQ, (data & mask) ? HOLD_LINE : CLEAR_LINE);
	} else if(id == SIG_CPU_BUSREQ) {
		cpustate->busreq = (data & mask) ? 1 : 0;
	} else if(id == SIG_CPU_HALTREQ) {
		cpustate->haltreq = (data & mask) ? 1 : 0;
	} else if(id == SIG_I86_TEST) {
		cpustate->test_state = (data & mask) ? 1 : 0;
	} else if(id == SIG_CPU_WAIT_FACTOR) {
		cpustate->waitfactor = data; // 65536.
		cpustate->waitcount = 0; // 65536.
	}
}

void I8086::set_intr_line(bool line, bool pending, uint32_t bit)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	set_irq_line(cpustate, INPUT_LINE_IRQ, line ? HOLD_LINE : CLEAR_LINE);
}

void I8086::set_extra_clock(int icount)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	cpustate->extra_cycles += icount;
}

int I8086::get_extra_clock()
{
	cpu_state *cpustate = (cpu_state *)opaque;
	return cpustate->extra_cycles;
}

uint32_t I8086::get_pc()
{
	cpu_state *cpustate = (cpu_state *)opaque;
	return cpustate->prevpc;
}

uint32_t I8086::get_next_pc()
{
	cpu_state *cpustate = (cpu_state *)opaque;
	return cpustate->pc;
}

uint32_t I8086::translate_address(int segment, uint32_t offset)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	return cpustate->base[segment] + offset;
}

void I8086::write_debug_data8(uint32_t addr, uint32_t data)
{
	int wait;
	d_mem->write_data8w(addr, data, &wait);
}

uint32_t I8086::read_debug_data8(uint32_t addr)
{
	int wait;
	return d_mem->read_data8w(addr, &wait);
}

void I8086::write_debug_data16(uint32_t addr, uint32_t data)
{
	int wait;
	d_mem->write_data16w(addr, data, &wait);
}

uint32_t I8086::read_debug_data16(uint32_t addr)
{
	int wait;
	return d_mem->read_data16w(addr, &wait);
}

void I8086::write_debug_io8(uint32_t addr, uint32_t data)
{
	int wait;
	d_io->write_io8w(addr, data, &wait);
}

uint32_t I8086::read_debug_io8(uint32_t addr) {
	int wait;
	return d_io->read_io8w(addr, &wait);
}

void I8086::write_debug_io16(uint32_t addr, uint32_t data)
{
	int wait;
	d_io->write_io16w(addr, data, &wait);
}

uint32_t I8086::read_debug_io16(uint32_t addr) {
	int wait;
	return d_io->read_io16w(addr, &wait);
}

bool I8086::write_debug_reg(const _TCHAR *reg, uint32_t data)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	if(_tcsicmp(reg, _T("IP")) == 0) {
		cpustate->pc = ((data & 0xffff) + cpustate->base[CS]) & AMASK;
		CHANGE_PC(cpustate->pc);
	} else if(_tcsicmp(reg, _T("AX")) == 0) {
		cpustate->regs.w[AX] = data;
	} else if(_tcsicmp(reg, _T("BX")) == 0) {
		cpustate->regs.w[BX] = data;
	} else if(_tcsicmp(reg, _T("CX")) == 0) {
		cpustate->regs.w[CX] = data;
	} else if(_tcsicmp(reg, _T("DX")) == 0) {
		cpustate->regs.w[DX] = data;
	} else if(_tcsicmp(reg, _T("SP")) == 0) {
		cpustate->regs.w[SP] = data;
	} else if(_tcsicmp(reg, _T("BP")) == 0) {
		cpustate->regs.w[BP] = data;
	} else if(_tcsicmp(reg, _T("SI")) == 0) {
		cpustate->regs.w[SI] = data;
	} else if(_tcsicmp(reg, _T("DI")) == 0) {
		cpustate->regs.w[DI] = data;
	} else if(_tcsicmp(reg, _T("AL")) == 0) {
		cpustate->regs.b[AL] = data;
	} else if(_tcsicmp(reg, _T("AH")) == 0) {
		cpustate->regs.b[AH] = data;
	} else if(_tcsicmp(reg, _T("BL")) == 0) {
		cpustate->regs.b[BL] = data;
	} else if(_tcsicmp(reg, _T("BH")) == 0) {
		cpustate->regs.b[BH] = data;
	} else if(_tcsicmp(reg, _T("CL")) == 0) {
		cpustate->regs.b[CL] = data;
	} else if(_tcsicmp(reg, _T("CH")) == 0) {
		cpustate->regs.b[CH] = data;
	} else if(_tcsicmp(reg, _T("DL")) == 0) {
		cpustate->regs.b[DL] = data;
	} else if(_tcsicmp(reg, _T("DH")) == 0) {
		cpustate->regs.b[DH] = data;
	} else {
		return false;
	}
	return true;
}

uint32_t I8086::read_debug_reg(const _TCHAR *reg)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	if(_tcsicmp(reg, _T("IP")) == 0) {
		return cpustate->pc - cpustate->base[CS];
	} else if(_tcsicmp(reg, _T("AX")) == 0) {
		return cpustate->regs.w[AX];
	} else if(_tcsicmp(reg, _T("BX")) == 0) {
		return cpustate->regs.w[BX];
	} else if(_tcsicmp(reg, _T("CX")) == 0) {
		return cpustate->regs.w[CX];
	} else if(_tcsicmp(reg, _T("DX")) == 0) {
		return cpustate->regs.w[DX];
	} else if(_tcsicmp(reg, _T("SP")) == 0) {
		return cpustate->regs.w[SP];
	} else if(_tcsicmp(reg, _T("BP")) == 0) {
		return cpustate->regs.w[BP];
	} else if(_tcsicmp(reg, _T("SI")) == 0) {
		return cpustate->regs.w[SI];
	} else if(_tcsicmp(reg, _T("DI")) == 0) {
		return cpustate->regs.w[DI];
	} else if(_tcsicmp(reg, _T("AL")) == 0) {
		return cpustate->regs.b[AL];
	} else if(_tcsicmp(reg, _T("AH")) == 0) {
		return cpustate->regs.b[AH];
	} else if(_tcsicmp(reg, _T("BL")) == 0) {
		return cpustate->regs.b[BL];
	} else if(_tcsicmp(reg, _T("BH")) == 0) {
		return cpustate->regs.b[BH];
	} else if(_tcsicmp(reg, _T("CL")) == 0) {
		return cpustate->regs.b[CL];
	} else if(_tcsicmp(reg, _T("CH")) == 0) {
		return cpustate->regs.b[CH];
	} else if(_tcsicmp(reg, _T("DL")) == 0) {
		return cpustate->regs.b[DL];
	} else if(_tcsicmp(reg, _T("DH")) == 0) {
		return cpustate->regs.b[DH];
	}
	return 0;
}

bool I8086::get_debug_regs_info(_TCHAR *buffer, size_t buffer_len)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	my_stprintf_s(buffer, buffer_len,
	_T("AX=%04X  BX=%04X CX=%04X DX=%04X SP=%04X  BP=%04X  SI=%04X  DI=%04X\n")
	_T("DS=%04X  ES=%04X SS=%04X CS=%04X IP=%04X  FLAG=[%c%c%c%c%c%c%c%c%c]\n")
	_T("Clocks = %llu (%llu) Since Scanline = %d/%d (%d/%d)"),
	cpustate->regs.w[AX], cpustate->regs.w[BX], cpustate->regs.w[CX], cpustate->regs.w[DX], cpustate->regs.w[SP], cpustate->regs.w[BP], cpustate->regs.w[SI], cpustate->regs.w[DI],
	cpustate->sregs[DS], cpustate->sregs[ES], cpustate->sregs[SS], cpustate->sregs[CS], cpustate->pc - cpustate->base[CS],
	OF ? _T('O') : _T('-'), DF ? _T('D') : _T('-'), cpustate->IF ? _T('I') : _T('-'), cpustate->TF ? _T('T') : _T('-'),
	SF ? _T('S') : _T('-'), ZF ? _T('Z') : _T('-'), AF ? _T('A') : _T('-'), PF ? _T('P') : _T('-'), CF ? _T('C') : _T('-'),
	cpustate->total_icount, cpustate->total_icount - cpustate->prev_total_icount,
	get_passed_clock_since_vline(), get_cur_vline_clocks(), get_cur_vline(), get_lines_per_frame());
	cpustate->prev_total_icount = cpustate->total_icount;
	return true;
}

int I8086::debug_dasm_with_userdata(uint32_t pc, _TCHAR *buffer, size_t buffer_len, uint32_t userdata)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	UINT64 eip = pc - cpustate->base[CS];
	UINT8 ops[16];
	for(int i = 0; i < 16; i++) {
		int wait;
		ops[i] = d_mem->read_data8w(pc + i, &wait);
	}
	UINT8 *oprom = ops;
	
	return CPU_DISASSEMBLE_CALL(x86_16) & DASMFLAG_LENGTHMASK;
}


#define STATE_VERSION	8

bool I8086::process_state(FILEIO* state_fio, bool loading)
{
	cpu_state *cpustate = (cpu_state *)opaque;
	
	if(!state_fio->StateCheckUint32(STATE_VERSION)) {
 		return false;
 	}
	if(!state_fio->StateCheckInt32(this_device_id)) {
 		return false;
 	}
//#if defined(HAS_I86) || defined(HAS_I88) || defined(HAS_I186) || defined(HAS_V30)
	state_fio->StateArray(cpustate->regs.w, sizeof(cpustate->regs.w), 1);
	state_fio->StateValue(cpustate->pc);
	state_fio->StateValue(cpustate->prevpc);
	state_fio->StateArray(cpustate->base, sizeof(cpustate->base), 1);
	state_fio->StateArray(cpustate->sregs, sizeof(cpustate->sregs), 1);
	state_fio->StateValue(cpustate->flags);
	state_fio->StateValue(cpustate->AuxVal);
	state_fio->StateValue(cpustate->OverVal);
	state_fio->StateValue(cpustate->SignVal);
	state_fio->StateValue(cpustate->ZeroVal);
	state_fio->StateValue(cpustate->CarryVal);
	state_fio->StateValue(cpustate->DirVal);
	state_fio->StateValue(cpustate->ParityVal);
	state_fio->StateValue(cpustate->TF);
	state_fio->StateValue(cpustate->IF);
	state_fio->StateValue(cpustate->MF);
	state_fio->StateValue(cpustate->int_vector);
	state_fio->StateValue(cpustate->nmi_state);
	state_fio->StateValue(cpustate->irq_state);
	state_fio->StateValue(cpustate->test_state);
	state_fio->StateValue(cpustate->rep_in_progress);
	state_fio->StateValue(cpustate->extra_cycles);
	state_fio->StateValue(cpustate->halted);
	state_fio->StateValue(cpustate->busreq);
	state_fio->StateValue(cpustate->ip);
	state_fio->StateValue(cpustate->sp);
//#ifdef USE_DEBUGGER
	state_fio->StateValue(cpustate->total_icount);
//#endif
	state_fio->StateValue(cpustate->icount);
	state_fio->StateValue(cpustate->seg_prefix);
	state_fio->StateValue(cpustate->prefix_seg);
	state_fio->StateValue(cpustate->ea);
	state_fio->StateValue(cpustate->eo);
	state_fio->StateValue(cpustate->ea_seg);
	state_fio->StateValue(cpustate->waitfactor);
	state_fio->StateValue(cpustate->waitcount);
	state_fio->StateValue(cpustate->memory_wait);
//#endif
 	
 	// post process
	if(loading) {
		cpustate->prev_total_icount = cpustate->total_icount;
	}
 	return true;
}
