/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2015.11.27-

	[ Qt printer ]
*/

#include "../emu.h"

void OSD::initialize_printer()
{
	prn_fio = new FILEIO();
	prn_data = -1;
	prn_strobe = false;
}

void OSD::release_printer()
{
	close_printer_file();
	delete prn_fio;
}



void OSD::printer_strobe(bool value)
{
	bool falling = (prn_strobe && !value);
	prn_strobe = value;
	
	if(falling) {
		if(!prn_fio->IsOpened()) {
			if(prn_data == -1) {
				return;
			}
			open_printer_file();
		}
		prn_fio->Fputc(prn_data);
		// wait 10sec
#ifdef SUPPORT_VARIABLE_TIMING
		prn_wait_frames = (int)(vm->get_frame_rate() * 10.0 + 0.5);
#else
		prn_wait_frames = (int)(FRAMES_PER_SEC * 10.0 + 0.5);
#endif
	}
}
