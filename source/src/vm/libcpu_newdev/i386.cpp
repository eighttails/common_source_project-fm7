

#include "vm.h"
#include "../emu.h"
#include "./i386.h"
#include "./libcpu_i386/i386_real.h"
#ifdef USE_DEBUGGER
#include "debugger.h"
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma warning( disable : 4018 )
#pragma warning( disable : 4065 )
#pragma warning( disable : 4146 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4996 )
#endif

#if defined(HAS_I386)
	#define CPU_MODEL i386
#elif defined(HAS_I486)
	#define CPU_MODEL i486
#elif defined(HAS_PENTIUM)
	#define CPU_MODEL pentium
#elif defined(HAS_MEDIAGX)
	#define CPU_MODEL mediagx
#elif defined(HAS_PENTIUM_PRO)
	#define CPU_MODEL pentium_pro
#elif defined(HAS_PENTIUM_MMX)
	#define CPU_MODEL pentium_mmx
#elif defined(HAS_PENTIUM2)
	#define CPU_MODEL pentium2
#elif defined(HAS_PENTIUM3)
	#define CPU_MODEL pentium3
#elif defined(HAS_PENTIUM4)
	#define CPU_MODEL pentium4
#endif

void I386::initialize()
{
	cpucore = new I386_OPS;
#if defined(HAS_I386)
	cpucore->cpu_init_i386();
#elif defined(HAS_I486)
	cpucore->cpu_init_i486();
#elif defined(HAS_PENTIUM)
	cpucore->cpu_init_pentium();
#elif defined(HAS_MEDIAGX)
	cpucore->cpu_init_mediagx();
#elif defined(HAS_PENTIUM_PRO)
	cpucore->cpu_init_pentium_pro();
#elif defined(HAS_PENTIUM_MMX)
	cpucore->cpu_init_pentium_mmx();
#elif defined(HAS_PENTIUM2)
	cpucore->cpu_init_pentium2();
#elif defined(HAS_PENTIUM3)
	cpucore->cpu_init_pentium3();
#elif defined(HAS_PENTIUM4)
	cpucore->cpu_init_pentium4();
#endif
	cpucore->set_context_pic(d_pic);
	cpucore->set_context_progmem(d_mem);
	cpucore->set_context_io(d_io);
#ifdef I386_PSEUDO_BIOS
	cpucore->set_context_pseudo_bios(d_bios);
#endif
#ifdef SINGLE_MODE_DMA
	cpucore->set_context_dma(d_dma);
#endif
	
#ifdef USE_DEBUGGER
	cpucore->set_context_emu(emu);
	cpucore->set_context_debugger(d_debugger);
	cpucore->set_context_progmem_stored(d_mem);
	cpucore->set_context_io_stored(d_io);
	
	d_debugger->set_context_mem(d_mem);
	d_debugger->set_context_io(d_io);
#endif
	cpucore->set_shutdown_flag(0);
}

void I386::reset()
{
#if defined(HAS_I386)
	cpucore->cpu_reset_i386();
#elif defined(HAS_I486)
	cpucore->cpu_reset_i486();
#elif defined(HAS_PENTIUM)
	cpucore->cpu_reset_pentium();
#elif defined(HAS_MEDIAGX)
	cpucore->cpu_reset_mediagx();
#elif defined(HAS_PENTIUM_PRO)
	cpucore->cpu_reset_pentium_pro();
#elif defined(HAS_PENTIUM_MMX)
	cpucore->cpu_reset_pentium_mmx();
#elif defined(HAS_PENTIUM2)
	cpucore->cpu_reset_pentium2();
#elif defined(HAS_PENTIUM3)
	cpucore->cpu_reset_pentium3();
#elif defined(HAS_PENTIUM4)
	cpucore->cpu_reset_pentium4();
#endif
}

int I386::run(int cycles)
{
	return cpucore->cpu_execute_i386(cycles);
}


#ifdef USE_DEBUGGER
void I386::write_debug_data8(uint32_t addr, uint32_t data)
{
	int wait;
	d_mem->write_data8w(addr, data, &wait);
}

uint32_t I386::read_debug_data8(uint32_t addr)
{
	int wait;
	return d_mem->read_data8w(addr, &wait);
}

void I386::write_debug_data16(uint32_t addr, uint32_t data)
{
	int wait;
	d_mem->write_data16w(addr, data, &wait);
}

uint32_t I386::read_debug_data16(uint32_t addr)
{
	int wait;
	return d_mem->read_data16w(addr, &wait);
}

void I386::write_debug_data32(uint32_t addr, uint32_t data)
{
	int wait;
	d_mem->write_data32w(addr, data, &wait);
}

uint32_t I386::read_debug_data32(uint32_t addr)
{
	int wait;
	return d_mem->read_data32w(addr, &wait);
}

void I386::write_debug_io8(uint32_t addr, uint32_t data)
{
	int wait;
	d_io->write_io8w(addr, data, &wait);
}

uint32_t I386::read_debug_io8(uint32_t addr) {
	int wait;
	return d_io->read_io8w(addr, &wait);
}

void I386::write_debug_io16(uint32_t addr, uint32_t data)
{
	int wait;
	d_io->write_io16w(addr, data, &wait);
}

uint32_t I386::read_debug_io16(uint32_t addr) {
	int wait;
	return d_io->read_io16w(addr, &wait);
}

void I386::write_debug_io32(uint32_t addr, uint32_t data)
{
	int wait;
	d_io->write_io32w(addr, data, &wait);
}

uint32_t I386::read_debug_io32(uint32_t addr) {
	int wait;
	return d_io->read_io32w(addr, &wait);
}

bool I386::write_debug_reg(const _TCHAR *reg, uint32_t data)
{
	return cpucore->write_debug_reg(reg, data);
}

void I386::get_debug_regs_info(_TCHAR *buffer, size_t buffer_len)
{
	cpucore->get_debug_regs_info(buffer, buffer_len);
}

int I386::debug_dasm(uint32_t pc, _TCHAR *buffer, size_t buffer_len)
{
	return cpucore->debug_dasm(pc, buffer, buffer_len);
}
#endif

void I386::set_context_bios(DEVICE* device)
{
#ifdef I386_PSEUDO_BIOS
	d_bios = device;
	if(cpucore != NULL) cpucore->set_context_pseudo_bios(d_bios);
#endif
}
void I386::set_context_dma(DEVICE* device)
{
#ifdef SINGLE_MODE_DMA
	d_dma = device;
	if(cpucore != NULL) cpucore->set_context_dma(d_dma);
#endif
}

#ifdef USE_DEBUGGER
void I386::set_context_debugger(DEBUGGER* device)
{
	d_debugger = device;
	if(cpucore != NULL) {
		cpucore->set_context_emu(emu);
		cpucore->set_context_debugger(d_debugger);
		cpucore->set_context_progmem_stored(d_mem);
		cpucore->set_context_io_stored(d_io);
	}
}
#endif


void I386::save_state(FILEIO* state_fio)
{
	I386_BASE::save_state(state_fio);
}

bool I386::load_state(FILEIO *state_fio)
{
	if(!I386_BASE::load_state(state_fio)) return false;
	
#ifdef I386_PSEUDO_BIOS
	cpucore->set_context_pseudo_bios(d_bios);
#endif
#ifdef SINGLE_MODE_DMA
	cpucore->set_context_dma(d_dma);
#endif
	
#ifdef USE_DEBUGGER
	cpucore->set_context_emu(emu);
	cpucore->set_context_debugger(d_debugger);
	cpucore->set_context_progmem_stored(d_mem);
	cpucore->set_context_io_stored(d_io);
	//d_debugger->set_context_mem(d_mem);
	//d_debugger->set_context_io(d_io);
#endif
	cpucore->set_shutdown_flag(0);
	return true;
}