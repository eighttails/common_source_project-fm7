/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2015.11.20-

	[ win32 console ]
*/

//#include "emu.h"
#include "fifo.h"
#include "./osd.h"
//#include "emu_thread.h"
#include <QString>

//extern EMU *emu;
//BOOL WINAPI ctrl_c_handler(DWORD type)
//{
//	return TRUE;
//}

void OSD_BASE::do_write_inputdata(QString s)
{
	//emit sig_console_input_string(s);
}

void OSD_BASE::do_set_input_string(QString s)
{
	if(!s.isEmpty()) {
		console_cmd_str.append(s);
		console_cmd_str.append(QString::fromUtf8("\n"));
	}
}

_TCHAR *OSD_BASE::console_input_string(void)
{
	//DebugSemaphore->acquire(1);
	if(console_cmd_str.isEmpty()) {
		//DebugSemaphore->release(1);
		return NULL;
	}
	_TCHAR *p = (_TCHAR *)console_cmd_str.toUtf8().constData();
	//DebugSemaphore->release();
	return p;
}

void OSD_BASE::clear_console_input_string(void)
{
	//DebugSemaphore->acquire(1);
	console_cmd_str.clear();
	//DebugSemaphore->release();
}

void OSD_BASE::open_console(_TCHAR* title)
{
	if(osd_console_opened) return;
	//DebugSemaphore->acquire(1);
	console_cmd_str.clear();
	osd_console_opened = true;
	//DebugSemaphore->release();

}

void OSD_BASE::close_console()
{
	//DebugSemaphore->release(DebugSemaphore->available());
	console_cmd_str.clear();
	osd_console_opened = false;
	emit sig_close_console();
}

unsigned int OSD_BASE::get_console_code_page()
{
	//return GetConsoleCP();
	return 0;
}

bool OSD_BASE::is_console_active()
{
	return 	osd_console_opened;
}

void OSD_BASE::set_console_text_attribute(unsigned short attr)
{
	QString attr_table[] = {
//		QString::fromUtf8("<FONT COLOR=black>"), // 0
		QString::fromUtf8("<FONT COLOR=white>"), // 0 : OK?
		QString::fromUtf8("<FONT COLOR=blue>"),  // 1
		QString::fromUtf8("<FONT COLOR=green>"), // 2
		QString::fromUtf8("<FONT COLOR=aqua>"),  // 3
		QString::fromUtf8("<FONT COLOR=red>"),   // 4
		QString::fromUtf8("<FONT COLOR=fuchsia>"),  // 5
		QString::fromUtf8("<FONT COLOR=yellow>"),   // 6
//		QString::fromUtf8("<FONT COLOR=gray>"),     // 7
		QString::fromUtf8("<FONT COLOR=black>"),     // 7
	};
	uint32_t new_color = 0;
	bool is_strong = false;
	if(attr & OSD_CONSOLE_BLUE)  new_color |= 1;
	if(attr & OSD_CONSOLE_GREEN) new_color |= 2;
	if(attr & OSD_CONSOLE_RED)   new_color |= 4;

	QString new_attr = attr_table[new_color];
	if(attr & OSD_CONSOLE_INTENSITY) {
		is_strong = true;
	}
	emit sig_set_attribute_debugger(new_attr, is_strong);
	//SetConsoleTextAttribute(hStdOut, new_attr);
}

void OSD_BASE::write_console(const _TCHAR* buffer, unsigned int length)
{
	QString s = QString::fromLocal8Bit((const char *)buffer, length);
	emit sig_put_string_debugger(s);
}

int OSD_BASE::read_console_input(_TCHAR* buffer, int length)
{
	int count = 0;
	QString tmps;
	//DebugSemaphore->acquire(1);
	tmps = console_cmd_str.left(16);
	//DebugSemaphore->release(1);
	if(buffer == NULL) return 0;
	memset(buffer, 0x00, 16);
	if(tmps.isEmpty()) {
		return 0;
	}
	int locallen = tmps.indexOf(QString::fromUtf8("\n"));
	if(locallen >= 16) locallen = 15;
	if(locallen >= 0) {
		tmps = tmps.left(locallen + 1);
		locallen = locallen + 1;
	}

	count = tmps.length();
	if(tmps.isEmpty() || (count <= 0)) return 0; 
	if(count > 16) count = 16;
	if(count > length) count = length;
	//DebugSemaphore->acquire(1);
	int l = console_cmd_str.length();
	
	console_cmd_str = console_cmd_str.right(l - count);	
	strncpy((char *)buffer, tmps.toLocal8Bit().constData(), count);
	//DebugSemaphore->release(1);

	return count;
}

// This is not recognise char code.
bool OSD_BASE::is_console_key_pressed(uint32_t ch)
{
	_TCHAR buf[17];
	if(read_console_input(buf, 16) > 0) return true;
	return false;
}
	
void OSD_BASE::close_debugger_console()
{
	emit sig_debugger_finished(); // It's dirty...
}

void OSD_BASE::do_close_debugger_thread()
{
	emit sig_debugger_finished();
}
