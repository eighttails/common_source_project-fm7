/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2007.08.14 -

	[ uPD71071 ]
*/

#ifndef _UPD71071_H_
#define _UPD71071_H_

//#include "vm.h"
//#include "../emu.h"
#include "device.h"

#define SIG_UPD71071_CH0				0
#define SIG_UPD71071_CH1				1
#define SIG_UPD71071_CH2				2
#define SIG_UPD71071_CH3				3
 /* UBE: INDICATE TARGET DEVICE HAS 16bit capability YES=1 NO=0*/
#define SIG_UPD71071_UBE_CH0			4
#define SIG_UPD71071_UBE_CH1			5
#define SIG_UPD71071_UBE_CH2			6
#define SIG_UPD71071_UBE_CH3			7
#define SIG_UPD71071_EOT_CH0			8
#define SIG_UPD71071_EOT_CH1			9
#define SIG_UPD71071_EOT_CH2			10
#define SIG_UPD71071_EOT_CH3			11
#define SIG_UPD71071_IS_TRANSFERING		16 /* 16 - 19 */
#define SIG_UPD71071_IS_16BITS_TRANSFER	20 /* 20 - 23 */
#define SIG_UPD71071_CREG				24 /* 24 - 27 */
#define SIG_UPD71071_BCREG				28 /* 28 - 31 */
#define SIG_UPD71071_AREG				32 /* 32 - 35 */
#define SIG_UPD71071_BAREG				36 /* 36 - 39 */

class DEBUGGER;
class  DLL_PREFIX UPD71071 : public DEVICE
{
protected:
	DEVICE* d_mem;
//#ifdef SINGLE_MODE_DMA
	DEVICE* d_dma;
//#endif
	DEBUGGER *d_debugger;
	outputs_t outputs_tc[4];
	outputs_t outputs_ube[4]; // If "1" word transfer, "0" byte transfer (OUT)
	
	struct {
		DEVICE* dev;
		uint32_t areg, bareg;
		uint32_t creg, bcreg; // 20200318 K.O (Temporally workaround for Towns)
		uint8_t mode;
		bool is_16bit;
		bool endreq;
		bool end;
	} dma[4];
	
	uint8_t b16, selch, base;
	uint16_t cmd, tmp;
	uint8_t req, sreq, mask, tc;
	bool inputs_ube[4];
	bool stats_ube[4];
	

	bool _SINGLE_MODE_DMA;
	bool _USE_DEBUGGER;

	virtual void __FASTCALL set_ube(int ch);
	virtual void __FASTCALL reset_ube(int ch);
	virtual void __FASTCALL do_dma_verify_8bit(int c);
	virtual void __FASTCALL do_dma_dev_to_mem_8bit(int c);
	virtual void __FASTCALL do_dma_mem_to_dev_8bit(int c);
	virtual void __FASTCALL do_dma_inc_dec_ptr_8bit(int c);
	virtual void __FASTCALL do_dma_verify_16bit(int c);
	virtual void __FASTCALL do_dma_dev_to_mem_16bit(int c);
	virtual void __FASTCALL do_dma_mem_to_dev_16bit(int c);
	virtual void __FASTCALL do_dma_inc_dec_ptr_16bit(int c);
	virtual bool __FASTCALL do_dma_epilogue(int c);
	virtual bool __FASTCALL do_dma_per_channel(int c);
	virtual void __FASTCALL reset_tc(int ch);
	virtual void __FASTCALL set_tc(int ch);
	virtual void reset_all_tc();

public:
	UPD71071(VM_TEMPLATE* parent_vm, EMU_TEMPLATE* parent_emu) : DEVICE(parent_vm, parent_emu)
	{
		// TIP: if((DEVICE::prev_device == NULL) || (DEVICE::this_device_id == 0)) DEVICE must be DUMMY.
		// And, at this device, should not be FIRST DEVICE. 20170613 Ohta.
		DEVICE *__dev = this;
		while((__dev->prev_device != NULL) && (__dev->this_device_id > 0)) {
			__dev = __dev->prev_device;
		}
		for(int i = 0; i < 4; i++) {
			//dma[i].dev = vm->dummy;
			dma[i].dev = __dev;
		}
		d_mem = __dev;
//#ifdef SINGLE_MODE_DMA
		d_dma = NULL;
//#endif
		d_debugger = NULL;
		_SINGLE_MODE_DMA = false;
		_USE_DEBUGGER = false;
		for(int i = 0; i < 4; i++) {
			initialize_output_signals(&outputs_tc[i]);
			initialize_output_signals(&outputs_ube[i]);
		}
		set_device_name(_T("uPD71071 DMAC"));
	}
	~UPD71071() {}
	
	// common functions
	virtual void initialize();
	virtual void reset();
	virtual void __FASTCALL write_io8(uint32_t addr, uint32_t data);
	virtual uint32_t __FASTCALL read_io8(uint32_t addr);
	virtual void __FASTCALL write_signal(int id, uint32_t data, uint32_t _mask);
	virtual uint32_t __FASTCALL read_signal(int id);
	virtual void __FASTCALL do_dma();
	// for debug
	virtual void __FASTCALL write_via_debugger_data8(uint32_t addr, uint32_t data);
	virtual uint32_t __FASTCALL read_via_debugger_data8(uint32_t addr);
	virtual void __FASTCALL write_via_debugger_data16(uint32_t addr, uint32_t data);
	virtual uint32_t __FASTCALL read_via_debugger_data16(uint32_t addr);
	bool is_debugger_available()
	{
		return true;
	}
	void *get_debugger()
	{
		return d_debugger;
	}
	virtual bool get_debug_regs_info(_TCHAR *buffer, size_t buffer_len);
	virtual bool process_state(FILEIO* state_fio, bool loading);
	// unique functions
	void set_context_memory(DEVICE* device)
	{
		d_mem = device;
	}
	void set_context_ch0(DEVICE* device)
	{
		dma[0].dev = device;
	}
	void set_context_ch1(DEVICE* device)
	{
		dma[1].dev = device;
	}
	void set_context_ch2(DEVICE* device)
	{
		dma[2].dev = device;
	}
	void set_context_ch3(DEVICE* device)
	{
		dma[3].dev = device;
	}
//#ifdef SINGLE_MODE_DMA
	void set_context_child_dma(DEVICE* device)
	{
		d_dma = device;
	}
//#endif
	void set_context_debugger(DEBUGGER* device)
	{
		d_debugger = device;
	}
	void set_context_tc0(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_tc[0], device, id, _mask);
	}	
	void set_context_tc1(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_tc[1], device, id, _mask);
	}	
	void set_context_tc2(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_tc[2], device, id, _mask);
	}	
	void set_context_tc3(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_tc[3], device, id, _mask);
	}	
	void set_context_ube0(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_ube[0], device, id, _mask);
	}
	void set_context_ube1(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_ube[1], device, id, _mask);
	}
	void set_context_ube2(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_ube[2], device, id, _mask);
	}
	void set_context_ube3(DEVICE* device, int id, uint32_t _mask)
	{
		register_output_signal(&outputs_ube[3], device, id, _mask);
	}
};

#endif

