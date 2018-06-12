/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2005.06.10-

	[ i8259 ]
*/

#ifndef _I8259_H_
#define _I8259_H_

//#include "vm.h"
//#include "../emu.h"
#include "device.h"

/*
	NOTE: I8259_MAX_CHIPS shoud be 1 or 2
*/

#define SIG_I8259_IR0	0
#define SIG_I8259_IR1	1
#define SIG_I8259_IR2	2
#define SIG_I8259_IR3	3
#define SIG_I8259_IR4	4
#define SIG_I8259_IR5	5
#define SIG_I8259_IR6	6
#define SIG_I8259_IR7	7
#define SIG_I8259_CHIP0	0
#define SIG_I8259_CHIP1	8
//#define SIG_I8259_CHIP2	16
//#define SIG_I8259_CHIP3	24

#define I8259_ADDR_CHIP0	0
#define I8259_ADDR_CHIP1	2
//#define I8259_ADDR_CHIP2	4
//#define I8259_ADDR_CHIP3	6

struct  i8259_pic_t {
	uint8_t imr, isr, irr, irr_tmp, prio;
	uint8_t icw1, icw2, icw3, icw4, ocw3;
	uint8_t icw2_r, icw3_r, icw4_r;
	int irr_tmp_id;
};

class I8259 : public DEVICE
{
private:
	DEVICE* d_cpu;

	struct i8259_pic_t pic[2];
	int req_chip, req_level;
	uint8_t req_bit;
	uint32_t __I8259_MAX_CHIPS;
	uint32_t __CHIP_MASK;
	
	void update_intr();
	
public:
	I8259(VM* parent_vm, EMU* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		d_cpu = NULL;
		__I8259_MAX_CHIPS = 0;
		__CHIP_MASK = 0xffffffff;
		set_device_name(_T("i8259 PIC"));
	}
	~I8259() {}
	
	// common functions
	void initialize();
	void reset();
	void write_io8(uint32_t addr, uint32_t data);
	uint32_t read_io8(uint32_t addr);
	void write_signal(int id, uint32_t data, uint32_t mask);
	uint32_t read_signal(int id);
	void event_callback(int event_id, int err);
	void decl_state();
	void save_state(FILEIO* state_fio);
	bool load_state(FILEIO* state_fio);
	// interrupt common functions
	void set_intr_line(bool line, bool pending, uint32_t bit)
	{
		// request from Z80 familly
		write_signal(bit, line ? 1 : 0, 1);
	}
	uint32_t get_intr_ack();
	
	// unique functions
	void set_context_cpu(DEVICE* device)
	{
		d_cpu = device;
	}
};

#endif

