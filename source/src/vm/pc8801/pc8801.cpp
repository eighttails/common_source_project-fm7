/*
	NEC PC-8801MA Emulator 'ePC-8801MA'
	NEC PC-8001mkIISR Emulator 'ePC-8001mkIISR'

	Author : Takeda.Toshiya
	Date   : 2012.02.16-

	[ virtual machine ]
*/

#include "pc8801.h"
#include "../../emu.h"
#include "../device.h"
#include "../event.h"

#include "../i8251.h"
#include "../i8255.h"
#include "../pcm1bit.h"
#include "../upd1990a.h"
#include "../ym2203.h"
#include "../z80.h"

#include "../disk.h"
#include "../pc80s31k.h"
#include "../upd765a.h"

#ifdef USE_DEBUGGER
#include "../debugger.h"
#endif

#ifdef SUPPORT_PC88_PCG8100
#include "../i8253.h"
#endif

#include "pc88.h"

// ----------------------------------------------------------------------------
// initialize
// ----------------------------------------------------------------------------

VM::VM(EMU* parent_emu) : emu(parent_emu)
{
	// check configs
	boot_mode = config.boot_mode;
	
	// create devices
	first_device = last_device = NULL;
	dummy = new DEVICE(this, emu);	// must be 1st device
	
	pc88event = new EVENT(this, emu);
//	pc88event->set_frames_per_sec(60);
//	pc88event->set_lines_per_frame(260);
	
	pc88 = new PC88(this, emu);
//	pc88->set_context_event_manager(pc88event);
	pc88sio = new I8251(this, emu);
//	pc88sio->set_context_event_manager(pc88event);
	pc88pio = new I8255(this, emu);
//	pc88pio->set_context_event_manager(pc88event);
	pc88pcm = new PCM1BIT(this, emu);
//	pc88pcm->set_context_event_manager(pc88event);
	pc88rtc = new UPD1990A(this, emu);
//	pc88rtc->set_context_event_manager(pc88event);
	// config.sound_device_type
	// 	0: 44h:OPNA A4h:None		PC-8801FH/MH or later
	// 	1: 44h:OPN  A4h:None		PC-8801mkIISR/TR/MR/FR
	// 	2: 44h:OPN  A4h:OPNA		PC-8801mkIISR/TR/MR/FR + PC-8801-23
	pc88opn = new YM2203(this, emu);
#ifdef SUPPORT_PC88_OPNA
	pc88opn->is_ym2608 = (config.sound_device_type == 0);
#endif
//	pc88opn->set_context_event_manager(pc88event);
#ifdef SUPPORT_PC88_SB2
	if(config.sound_device_type == 2) {
		pc88sb2 = new YM2203(this, emu);
#ifdef SUPPORT_PC88_OPNA
		pc88sb2->is_ym2608 = true;
#endif
//		pc88sb2->set_context_event_manager(pc88event);
	} else {
		pc88sb2 = NULL;
	}
#endif
	pc88cpu = new Z80(this, emu);
//	pc88cpu->set_context_event_manager(pc88event);
	dummycpu = new DEVICE(this, emu);
	
	pc88sub = new PC80S31K(this, emu);
//	pc88sub->set_context_event_manager(pc88event);
	pc88pio_sub = new I8255(this, emu);
//	pc88pio_sub->set_context_event_manager(pc88event);
	pc88fdc_sub = new UPD765A(this, emu);
//	pc88fdc_sub->set_context_event_manager(pc88event);
	pc88cpu_sub = new Z80(this, emu);
//	pc88cpu_sub->set_context_event_manager(pc88event);
	
#ifdef SUPPORT_PC88_PCG8100
	pc88pit = new I8253(this, emu);
//	pc88pit->set_context_event_manager(pc88event);
	pc88pcm0 = new PCM1BIT(this, emu);
//	pc88pcm->set_context_event_manager(pc88event);
	pc88pcm1 = new PCM1BIT(this, emu);
//	pc88pcm->set_context_event_manager(pc88event);
	pc88pcm2 = new PCM1BIT(this, emu);
//	pc88pcm->set_context_event_manager(pc88event);
#endif
	
#ifdef SUPPORT_PC88_HIGH_CLOCK
	pc88event->set_context_cpu(dummycpu, 7987248);
	pc88event->set_context_cpu(pc88cpu, (config.cpu_type != 0) ? 3993624 : 7987248);
#else
	pc88event->set_context_cpu(dummycpu, 3993624);
	pc88event->set_context_cpu(pc88cpu, 3993624);
#endif
	pc88event->set_context_cpu(pc88cpu_sub, 3993624);
	pc88event->set_context_sound(pc88opn);
#ifdef SUPPORT_PC88_SB2
	if(pc88sb2 != NULL) {
		pc88event->set_context_sound(pc88sb2);
	}
#endif
	pc88event->set_context_sound(pc88pcm);
#ifdef SUPPORT_PC88_PCG8100
	pc88event->set_context_sound(pc88pcm0);
	pc88event->set_context_sound(pc88pcm1);
	pc88event->set_context_sound(pc88pcm2);
#endif
#ifdef DATAREC_SOUND
	pc88event->set_context_sound(pc88);
#endif	
	pc88->set_context_cpu(pc88cpu);
	pc88->set_context_opn(pc88opn);
#ifdef SUPPORT_PC88_SB2
	pc88->set_context_sb2(pc88sb2);
#endif
	pc88->set_context_pcm(pc88pcm);
	pc88->set_context_pio(pc88pio);
	pc88->set_context_rtc(pc88rtc);
	pc88->set_context_sio(pc88sio);
#ifdef SUPPORT_PC88_PCG8100
	pc88->set_context_pcg_pit(pc88pit);
	pc88->set_context_pcg_pcm0(pc88pcm0);
	pc88->set_context_pcg_pcm1(pc88pcm1);
	pc88->set_context_pcg_pcm2(pc88pcm2);
#endif
	pc88cpu->set_context_mem(pc88);
	pc88cpu->set_context_io(pc88);
	pc88cpu->set_context_intr(pc88);
#ifdef USE_DEBUGGER
	pc88cpu->set_context_debugger(new DEBUGGER(this, emu));
#endif
	pc88opn->set_context_irq(pc88, SIG_PC88_SOUND_IRQ, 1);
#ifdef SUPPORT_PC88_SB2
	if(pc88sb2 != NULL) {
		pc88sb2->set_context_irq(pc88, SIG_PC88_SB2_IRQ, 1);
	}
#endif
	pc88sio->set_context_rxrdy(pc88, SIG_PC88_USART_IRQ, 1);
	pc88sio->set_context_out(pc88, SIG_PC88_USART_OUT);
	
	pc88sub->set_context_cpu(pc88cpu_sub);
	pc88sub->set_context_fdc(pc88fdc_sub);
	pc88sub->set_context_pio(pc88pio_sub);
	pc88pio->set_context_port_a(pc88pio_sub, SIG_I8255_PORT_B, 0xff, 0);
	pc88pio->set_context_port_b(pc88pio_sub, SIG_I8255_PORT_A, 0xff, 0);
	pc88pio->set_context_port_c(pc88pio_sub, SIG_I8255_PORT_C, 0x0f, 4);
	pc88pio->set_context_port_c(pc88pio_sub, SIG_I8255_PORT_C, 0xf0, -4);
	pc88pio->clear_ports_by_cmdreg = true;
	pc88pio_sub->set_context_port_a(pc88pio, SIG_I8255_PORT_B, 0xff, 0);
	pc88pio_sub->set_context_port_b(pc88pio, SIG_I8255_PORT_A, 0xff, 0);
	pc88pio_sub->set_context_port_c(pc88pio, SIG_I8255_PORT_C, 0x0f, 4);
	pc88pio_sub->set_context_port_c(pc88pio, SIG_I8255_PORT_C, 0xf0, -4);
	pc88pio_sub->clear_ports_by_cmdreg = true;
	pc88fdc_sub->set_context_irq(pc88cpu_sub, SIG_CPU_IRQ, 1);
	pc88cpu_sub->set_context_mem(pc88sub);
	pc88cpu_sub->set_context_io(pc88sub);
	pc88cpu_sub->set_context_intr(pc88sub);
#ifdef USE_DEBUGGER
	pc88cpu_sub->set_context_debugger(new DEBUGGER(this, emu));
#endif
	
#ifdef SUPPORT_PC88_PCG8100
	pc88pit->set_context_ch0(pc88pcm0, SIG_PCM1BIT_SIGNAL, 1);
	pc88pit->set_context_ch1(pc88pcm1, SIG_PCM1BIT_SIGNAL, 1);
	pc88pit->set_context_ch2(pc88pcm2, SIG_PCM1BIT_SIGNAL, 1);
	pc88pit->set_constant_clock(0, 3993624);
	pc88pit->set_constant_clock(1, 3993624);
	pc88pit->set_constant_clock(2, 3993624);
#endif
	
	// initialize all devices
	for(DEVICE* device = first_device; device; device = device->next_device) {
		device->initialize();
	}
}

VM::~VM()
{
	// delete all devices
	for(DEVICE* device = first_device; device;) {
		DEVICE *next_device = device->next_device;
		device->release();
		delete device;
		device = next_device;
	}
}

DEVICE* VM::get_device(int id)
{
	for(DEVICE* device = first_device; device; device = device->next_device) {
		if(device->this_device_id == id) {
			return device;
		}
	}
	return NULL;
}

// ----------------------------------------------------------------------------
// drive virtual machine
// ----------------------------------------------------------------------------

void VM::reset()
{
	// reset all devices
	for(DEVICE* device = first_device; device; device = device->next_device) {
		device->reset();
	}
	for(DEVICE* device = first_device; device; device = device->next_device) {
		device->reset();
	}
	
	// initial device settings
	pc88opn->SetReg(0x29, 3); // for Misty Blue
	pc88pio->write_signal(SIG_I8255_PORT_C, 0, 0xff);
	pc88pio_sub->write_signal(SIG_I8255_PORT_C, 0, 0xff);
}

void VM::run()
{
	pc88event->drive();
}

double VM::frame_rate()
{
	return pc88event->frame_rate();
}

// ----------------------------------------------------------------------------
// debugger
// ----------------------------------------------------------------------------

#ifdef USE_DEBUGGER
DEVICE *VM::get_cpu(int index)
{
	if(index == 0) {
		return pc88cpu;
	} else if(index == 1) {
		return pc88cpu_sub;
	}
	return NULL;
}
#endif

// ----------------------------------------------------------------------------
// draw screen
// ----------------------------------------------------------------------------

void VM::draw_screen()
{
	pc88->draw_screen();
}

int VM::access_lamp()
{
	return pc88fdc_sub->read_signal(0);
}

// ----------------------------------------------------------------------------
// soud manager
// ----------------------------------------------------------------------------

void VM::initialize_sound(int rate, int samples)
{
	// init sound manager
	pc88event->initialize_sound(rate, samples);
	
	// init sound gen
#ifdef SUPPORT_PC88_OPNA
	if(pc88opn->is_ym2608) {
		pc88opn->init(rate, 7987248, samples, 0, 0);
	} else
#endif
	pc88opn->init(rate, 3993624, samples, 0, 0);
#ifdef SUPPORT_PC88_SB2
	if(pc88sb2 != NULL) {
#ifdef SUPPORT_PC88_OPNA
		if(pc88sb2->is_ym2608) {
			pc88sb2->init(rate, 7987248, samples, 0, 0);
		} else
#endif
		pc88sb2->init(rate, 3993624, samples, 0, 0);
	}
#endif
	pc88pcm->init(rate, 8000);
#ifdef SUPPORT_PC88_PCG8100
	pc88pcm0->init(rate, 8000);
	pc88pcm1->init(rate, 8000);
	pc88pcm2->init(rate, 8000);
#endif
}

uint16* VM::create_sound(int* extra_frames)
{
	return pc88event->create_sound(extra_frames);
}

int VM::sound_buffer_ptr()
{
	return pc88event->sound_buffer_ptr();
}

// ----------------------------------------------------------------------------
// notify key
// ----------------------------------------------------------------------------

void VM::key_down(int code, bool repeat)
{
	pc88->key_down(code, repeat);
}

void VM::key_up(int code)
{
}

// ----------------------------------------------------------------------------
// user interface
// ----------------------------------------------------------------------------

void VM::open_disk(int drv, _TCHAR* file_path, int bank)
{
	pc88fdc_sub->open_disk(drv, file_path, bank);
}

void VM::close_disk(int drv)
{
	pc88fdc_sub->close_disk(drv);
}

bool VM::disk_inserted(int drv)
{
	return pc88fdc_sub->disk_inserted(drv);
}

void VM::set_disk_protected(int drv, bool value)
{
	pc88fdc_sub->set_disk_protected(drv, value);
}

bool VM::get_disk_protected(int drv)
{
	return pc88fdc_sub->get_disk_protected(drv);
}

void VM::play_tape(_TCHAR* file_path)
{
	pc88->play_tape(file_path);
}

void VM::rec_tape(_TCHAR* file_path)
{
	pc88->rec_tape(file_path);
}

void VM::close_tape()
{
	pc88->close_tape();
}

bool VM::tape_inserted()
{
	return pc88->tape_inserted();
}

#if defined(USE_TAPE_PTR)
int VM::get_tape_ptr()
{
        return pc88->get_tape_ptr();
}
#endif

bool VM::now_skip()
{
//	return event->now_skip();
	return pc88->now_skip();
}

void VM::update_config()
{
	int ii;
	uint32 vol1, vol2;
#ifdef USE_MULTIPLE_SOUNDCARDS
	vol1 = vol2 = (config.sound_device_level[1] + 32768) >> 8; // Card1
	pc88opn->write_signal(SIG_YM2203_LVOLUME, vol1, 0xffffffff); // OPN: LEFT
	pc88opn->write_signal(SIG_YM2203_RVOLUME, vol2, 0xffffffff); // OPN: RIGHT
# ifdef SUPPORT_PC88_SB2
	if(pc88sb2 != NULL) {
		vol1 = vol2 = (config.sound_device_level[2] + 32768) >> 8; // Card2
		pc88sb2->write_signal(SIG_YM2203_LVOLUME, vol1, 0xffffffff); // OPN: LEFT
		pc88sb2->write_signal(SIG_YM2203_RVOLUME, vol2, 0xffffffff); // OPN: RIGHT
	}
# endif
#endif
	if(boot_mode != config.boot_mode) {
		// boot mode is changed !!!
		boot_mode = config.boot_mode;
		reset();
	} else {
		for(DEVICE* device = first_device; device; device = device->next_device) {
			device->update_config();
		}
	}
#ifdef SUPPORT_PC88_HIGH_CLOCK
	pc88event->set_cpu_clock(pc88cpu, (config.cpu_type != 0) ? 3993624 : 7987248);
#endif
   
}

#define STATE_VERSION	4

void VM::save_state(FILEIO* state_fio)
{
	state_fio->FputUint32(STATE_VERSION);
	
	for(DEVICE* device = first_device; device; device = device->next_device) {
		device->save_state(state_fio);
	}
	state_fio->FputInt32(boot_mode);
}

bool VM::load_state(FILEIO* state_fio)
{
	if(state_fio->FgetUint32() != STATE_VERSION) {
		return false;
	}
	for(DEVICE* device = first_device; device; device = device->next_device) {
		if(!device->load_state(state_fio)) {
			return false;
		}
	}
	boot_mode = state_fio->FgetInt32();
	return true;
}

