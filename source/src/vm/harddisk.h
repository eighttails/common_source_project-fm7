/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2006.09.16-

	[ d88 image handler ]
*/

#ifndef _HARDDISK_H_
#define _HARDDISK_H_

#include "../common.h"
//#include "vm.h"
//#include "../emu.h"

class FILEIO;
class EMU_TEMPLATE;
class  DLL_PREFIX HARDDISK
{
protected:
	EMU_TEMPLATE* emu;
private:
	FILEIO *fio;
	int header_size;
	
public:
	HARDDISK(EMU_TEMPLATE* parent_emu)
	{
		emu = parent_emu;
		fio = NULL;
		access = false;
		static int num = 0;
		drive_num = num++;
		set_device_name(_T("Hard Disk Drive #%d"), drive_num + 1);
	}
	~HARDDISK()
	{
		close();
	}
	
	void open(const _TCHAR* file_path, int default_sector_size);
	void close();
	bool mounted();
	bool accessed();
	bool read_buffer(long position, int length, uint8_t *buffer);
	bool write_buffer(long position, int length, uint8_t *buffer);
	
	int cylinders;
	int surfaces;
	int sectors;
	int sector_size;
	int sector_num;
	int drive_num;
	bool access;
	
	long get_cur_position();
	int get_sector_size();
	int get_cylinders();
	int get_headers();
	int get_sectors_per_cylinder();
	int get_sector_num();
	int get_drive_num();
		
	// device name
	void set_device_name(const _TCHAR* format, ...)
	{
		if(format != NULL) {
			va_list ap;
			_TCHAR buffer[1024];
			
			va_start(ap, format);
			my_vstprintf_s(buffer, 1024, format, ap);
			va_end(ap);
			
			my_tcscpy_s(this_device_name, 128, buffer);
		}
	}
	const _TCHAR *get_device_name()
	{
		return (const _TCHAR *)this_device_name;
	}
	_TCHAR this_device_name[128];
};

#endif

