/*
	Skelton for retropc emulator

	Origin : PockEmul
	Author : Takeda.Toshiya
	Date   : 2016.03.18-

	[ uPD16434 ]
*/

//#include "vm.h"
//#include "emu.h"
#include "upd16434.h"

/*
STOP        Set Stop Mode                       01
DISP OFF    Display Off                         08
DISP ON     Display On                          09
SFF         Set Frame Frequency                 10 to 14
SMM         Set Multiplexing Mode               18 to 1F
BRESET      Bit Reset                           20 to 3F
BSET        Bit set                             40 to 5F
SRM         Set Read Mode                       60 to 63
SWM         Set Write Mode                      64 to 67
SORM        Set OR Mode                         68 to 6B
SANDM       Set AND Mode                        6C to 6F
SCML        Set Character Mode with Left entry  71
SCMR        Set Character Mode with Right entry 72
CLCURS      Clear Cursor                        7C
WRCURS      Write Cursor                        7D
LDPI        Load Data Pointer with Immediate    80 to B1, C0 to F1
*/

#define MASK_STOP	0x01
#define MASK_DISPOFF	0x08
#define MASK_DISPON	0x09
#define MASK_SFF	0x10
#define MASK_SMM	0x18
#define MASK_BRESET	0x20
#define MASK_BSET	0x40
#define MASK_SRM	0x60
#define MASK_SWM	0x64
#define MASK_SORM	0x68
#define MASK_SANDM	0x6C
#define MASK_SCML	0x71
#define MASK_SCMR	0x72
#define MASK_CLCURS	0x7C
#define MASK_WRCURS	0x7D
#define MASK_LDPI	0x80

#define SWM		0x64
#define SCMR		0x72
#define SCML		0x71

static const uint8_t font[5 * 256] = {
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x4F, 0x00, 0x00,
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x2C, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x55, 0x22, 0x50,
	0x00, 0x05, 0x03, 0x00, 0x00,
	0x00, 0x00, 0x1C, 0x22, 0x41,
	0x41, 0x22, 0x1C, 0x00, 0x00,
	0x14, 0x08, 0x3E, 0x08, 0x14,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x50, 0x30, 0x00, 0x00, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x60, 0x60, 0x00, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,

	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x42, 0x61, 0x51, 0x49, 0x46,
	0x21, 0x41, 0x45, 0x4B, 0x31,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x30,
	0x01, 0x71, 0x09, 0x05, 0x03,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x06, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x36, 0x36, 0x00, 0x00,
	0x00, 0x56, 0x36, 0x00, 0x00,
	0x08, 0x14, 0x22, 0x41, 0x00,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x51, 0x09, 0x06,

	0x32, 0x49, 0x79, 0x41, 0x3E,
	0x7E, 0x11, 0x11, 0x11, 0x7E,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x22, 0x1C,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x49, 0x49, 0x78,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x0C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,

	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x46, 0x49, 0x49, 0x49, 0x31,
	0x01, 0x01, 0x7F, 0x01, 0x01,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x07, 0x08, 0x70, 0x08, 0x07,
	0x61, 0x51, 0x49, 0x45, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x00,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x7F, 0x00,
	0x04, 0x02, 0x01, 0x02, 0x04,
	0x40, 0x40, 0x40, 0x40, 0x40,

	0x00, 0x01, 0x02, 0x04, 0x00,
	0x20, 0x54, 0x54, 0x54, 0x78,
	0x00, 0x7F, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x48,
	0x00, 0x38, 0x44, 0x44, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x58,
	0x00, 0x04, 0x7E, 0x05, 0x01,
	0x08, 0x54, 0x54, 0x54, 0x3C,
	0x00, 0x7F, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x00, 0x7F, 0x10, 0x28, 0x44,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x00, 0x7C, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,

	0x7C, 0x14, 0x14, 0x14, 0x08,
	0x08, 0x14, 0x14, 0x14, 0x7C,
	0x00, 0x7C, 0x08, 0x04, 0x04,
	0x58, 0x54, 0x54, 0x54, 0x30,
	0x00, 0x04, 0x3E, 0x44, 0x40,
	0x00, 0x3C, 0x40, 0x40, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x00, 0x0C, 0x50, 0x50, 0x3C,
	0x44, 0x64, 0x54, 0x4C, 0x44,
	0x00, 0x08, 0x36, 0x41, 0x00,
	0x00, 0x00, 0x7F, 0x00, 0x00,
	0x00, 0x41, 0x36, 0x08, 0x00,
	0x04, 0x02, 0x04, 0x08, 0x04,
	0x7F, 0x7F, 0x7F, 0x7F, 0x7F,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x70, 0x50, 0x70, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x01, 0x01,
	0x40, 0x40, 0x78, 0x00, 0x00,
	0x10, 0x20, 0x40, 0x00, 0x00,
	0x00, 0x0C, 0x0C, 0x00, 0x00,
	0x0A, 0x0A, 0x4A, 0x2A, 0x1E,
	0x04, 0x44, 0x34, 0x14, 0x0C,
	0x20, 0x10, 0x78, 0x04, 0x00,
	0x18, 0x08, 0x4C, 0x48, 0x38,
	0x48, 0x48, 0x78, 0x48, 0x48,
	0x48, 0x28, 0x18, 0x7C, 0x08,
	0x08, 0x7C, 0x08, 0x28, 0x18,
	0x40, 0x48, 0x48, 0x78, 0x40,
	0x54, 0x54, 0x54, 0x7C, 0x00,
	0x18, 0x00, 0x58, 0x40, 0x38,

	0x08, 0x08, 0x08, 0x08, 0x08,
	0x01, 0x41, 0x3D, 0x09, 0x07,
	0x10, 0x08, 0x7C, 0x02, 0x01,
	0x0E, 0x02, 0x43, 0x22, 0x1E,
	0x42, 0x42, 0x7E, 0x42, 0x42,
	0x22, 0x12, 0x0A, 0x7F, 0x02,
	0x42, 0x3F, 0x02, 0x42, 0x3E,
	0x0A, 0x0A, 0x7F, 0x0A, 0x0A,
	0x08, 0x46, 0x42, 0x22, 0x1E,
	0x04, 0x03, 0x42, 0x3E, 0x02,
	0x42, 0x42, 0x42, 0x42, 0x7E,
	0x02, 0x4F, 0x22, 0x1F, 0x02,
	0x4A, 0x4A, 0x40, 0x20, 0x1C,
	0x42, 0x22, 0x12, 0x2A, 0x46,
	0x02, 0x3F, 0x42, 0x4A, 0x46,
	0x06, 0x48, 0x40, 0x20, 0x1E,

	0x08, 0x46, 0x4A, 0x32, 0x1E,
	0x0A, 0x4A, 0x3E, 0x09, 0x08,
	0x0E, 0x00, 0x4E, 0x20, 0x1E,
	0x04, 0x45, 0x3D, 0x05, 0x04,
	0x00, 0x7F, 0x08, 0x10, 0x00,
	0x44, 0x24, 0x1F, 0x04, 0x04,
	0x40, 0x42, 0x42, 0x42, 0x40,
	0x42, 0x2A, 0x12, 0x2A, 0x06,
	0x22, 0x12, 0x7B, 0x16, 0x22,
	0x00, 0x40, 0x20, 0x1F, 0x00,
	0x78, 0x00, 0x02, 0x04, 0x78,
	0x3F, 0x44, 0x44, 0x44, 0x44,
	0x02, 0x42, 0x42, 0x22, 0x1E,
	0x04, 0x02, 0x04, 0x08, 0x30,
	0x32, 0x02, 0x7F, 0x02, 0x32,
	0x02, 0x12, 0x22, 0x52, 0x0E,

	0x20, 0x2A, 0x2A, 0x2A, 0x40,
	0x38, 0x24, 0x22, 0x20, 0x70,
	0x40, 0x28, 0x10, 0x28, 0x06,
	0x0A, 0x3E, 0x4A, 0x4A, 0x4A,
	0x04, 0x7F, 0x04, 0x14, 0x0C,	// YA
	0x40, 0x42, 0x42, 0x7E, 0x40,
	0x4A, 0x4A, 0x4A, 0x4A, 0x7E,
	0x04, 0x05, 0x45, 0x25, 0x1C,
	0x0F, 0x40, 0x20, 0x1F, 0x00,
	0x7E, 0x00, 0x7E, 0x40, 0x30,	// RU
	0x7E, 0x40, 0x20, 0x10, 0x08,
	0x7E, 0x42, 0x42, 0x42, 0x7E,
	0x0E, 0x02, 0x42, 0x22, 0x1E,
	0x42, 0x42, 0x40, 0x20, 0x18,
	0x04, 0x08, 0x02, 0x04, 0x00,
	0x07, 0x05, 0x07, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00,
};

void UPD16434::initialize()
{
   DEVICE::initialize();
}

void UPD16434::reset()
{
	pointer = 0;
	mode = SWM;
	memset(imem, 0, sizeof(imem));
}

void UPD16434::draw_char(uint8_t c, bool right)
{
	for(int i = 0; i < 5;i++) {
		imem[pointer] = font[c * 5 + 4 - i];
		if(right) {
			pointer++;
		} else {
			pointer--;
		}
		pointer &= 0x7f;
	}
}

void UPD16434::data(uint8_t cmd)
{
	switch(mode) {
	case SCMR:
		draw_char(cmd, true);
		break;
	case SCML:
		draw_char(cmd, false);
		break;
	case SWM + 0:
	case SWM + 1:
	case SWM + 2:
	case SWM + 3:
		imem[pointer] = cmd;
		update_pointer(mode);
		break;
	}
}

void UPD16434::instruction(uint8_t cmd)
{
	if((cmd & MASK_LDPI) == MASK_LDPI) {
		pointer = cmd & 0x7F;
		if((mode >= 0x60) && (mode <= 0x63)) {
			update_pointer(mode);
		}
//	} else if((cmd & MASK_WRCURS) == MASK_WRCURS) {
//	} else if((cmd & MASK_CLCURS) == MASK_CLCURS) {
	} else if((cmd & MASK_SCMR) == MASK_SCMR) {
		cmd_mode(cmd);
	} else if((cmd & MASK_SCML) == MASK_SCML) {
		cmd_mode(cmd);
	} else if((cmd & MASK_SANDM) == MASK_SANDM) {
		cmd_mode(cmd);
	} else if((cmd & MASK_SORM) == MASK_SORM) {
		cmd_mode(cmd);
	} else if((cmd & MASK_SWM) == MASK_SWM) {
		cmd_mode(cmd);
	} else if((cmd & MASK_SRM) == MASK_SRM) {
		cmd_mode(cmd);
	} else if((cmd & MASK_BSET) == MASK_BSET) {
		cmd_bset(cmd, true);
	} else if((cmd & MASK_BRESET) == MASK_BRESET) {
		cmd_bset(cmd, false);
//	} else if((cmd & MASK_SMM) == MASK_SMM) {
//	} else if((cmd & MASK_SFF) == MASK_SFF) {
//	} else if((cmd & MASK_DISPON) == MASK_DISPON) {
//	} else if((cmd & MASK_DISPOFF) == MASK_DISPOFF) {
//	} else if((cmd & MASK_STOP) == MASK_STOP) {
	}
}

void UPD16434::cmd_mode(uint8_t cmd)
{
	mode = cmd;
	
	if((cmd >= 0x60) && (cmd <= 0x63)) {
		update_pointer(mode);
	}
}

void UPD16434::cmd_bset(uint8_t cmd, bool set)
{
	uint8_t bit = (cmd >> 2) & 0x07;
	
	if(set){
		imem[pointer] |= (0x01 << bit);
	} else {
		imem[pointer] &= ~(0x01 << bit);
	}
	update_pointer(cmd);
}

void UPD16434::update_pointer(uint8_t mode)
{
	if((mode & 0x03) == 0) {
		pointer++;
		pointer &= 0x7f;
	} else if((mode & 0x03) == 1) {
		pointer--;
		pointer &= 0x7f;
	}
}

void UPD16434::draw(int xoffset)
{
	scrntype_t color_on   = RGB_COLOR( 48,  56,  16);	// dot on
	scrntype_t color_off  = RGB_COLOR(144, 150, 144);	// dot off
	scrntype_t color_back = RGB_COLOR(160, 168, 160);	// back
	
	for(int y = 0; y < 24; y++) {
		scrntype_t* dest = osd->get_vm_screen_buffer(y) + xoffset;
		for(int x = 0; x < 60; x++) {
			dest[x] = color_back;
		}
	}
	for(int b = 0; b < 8; b++) {
		scrntype_t* dest_u = osd->get_vm_screen_buffer(b + (b == 7) +  2) + xoffset;
		scrntype_t* dest_l = osd->get_vm_screen_buffer(b + (b == 7) + 13) + xoffset;
		uint8_t bit = 0x01 << b;
		
		for(int j = 0; j < 0x32; j++) {
			dest_u[j + j / 5] = (imem[0x31 - j] & bit) ? color_on : color_off;
			dest_l[j + j / 5] = (imem[0x71 - j] & bit) ? color_on : color_off;
		}
	}
}

#define STATE_VERSION	1

void UPD16434::save_state(FILEIO* state_fio)
{
	state_fio->FputUint32(STATE_VERSION);
	state_fio->FputInt32(this_device_id);
	
	state_fio->FputUint8(pointer);
	state_fio->FputUint8(mode);
	state_fio->Fwrite(imem, sizeof(imem), 1);
}

bool UPD16434::load_state(FILEIO* state_fio)
{
	if(state_fio->FgetUint32() != STATE_VERSION) {
		return false;
	}
	if(state_fio->FgetInt32() != this_device_id) {
		return false;
	}
	pointer = state_fio->FgetUint8();
	mode = state_fio->FgetUint8();
	state_fio->Fread(imem, sizeof(imem), 1);
	return true;
}

