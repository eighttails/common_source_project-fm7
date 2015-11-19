/*
	Skelton for retropc emulator

	Author : Takeda.Toshiya
	Date   : 2006.09.16-

	[ d88 handler ]
*/

#include "disk.h"
#include "../config.h"
#include "../fileio.h"

#ifndef _ANY2D88
#define bios_path(x) emu->bios_path(x)
#else
#define bios_path(x) (x)
#endif


// crc table
static const uint16 crc_table[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static const int secsize[8] = {
	128, 256, 512, 1024, 2048, 4096, 8192, 16384
};

static uint8 tmp_buffer[DISK_BUFFER_SIZE];

// physical format table for solid image
typedef struct {
	int type;
	int ncyl, nside, nsec, size;
	bool mfm;
} fd_format_t;

#define FM	false
#define MFM	true

static const fd_format_t fd_formats[] = {
#if defined(_SC3000)
	{ MEDIA_TYPE_2D,  40, 1, 16,  256, MFM },	// 1D	160KB
#elif defined(_SMC70) || defined(_SMC777)
	{ MEDIA_TYPE_2DD, 70, 1, 16,  256, MFM },	// 1DD	280KB
#elif defined(_X1) || defined(_X1TWIN) || defined(_X1TURBO) || defined(_X1TURBOZ)
	{ MEDIA_TYPE_2D,  40, 2, 16,  256, MFM },	// 2D	320KB
#elif defined(_M5)
	{ MEDIA_TYPE_2D,  40, 2, 18,  256, MFM },	// 2D	360KB
#elif defined(_MZ80B) || defined(_MZ2000) || defined(_MZ2200) || defined(_MZ2500)
	{ MEDIA_TYPE_2DD, 80, 2, 16,  256, MFM },	// 2DD	640KB
#endif
	{ MEDIA_TYPE_2D,  35, 1, 16,  128, FM  },	// 1S	70KB
	{ MEDIA_TYPE_2D,  35, 2, 16,  128, FM  },	// 2S	140KB
	{ MEDIA_TYPE_2DD, 77, 1, 26,  128, FM  },	// 1S	250KB
	{ MEDIA_TYPE_2D,  40, 1,  8,  512, MFM },	// 1D	160KB
	{ MEDIA_TYPE_2D,  40, 1,  9,  512, MFM },	// 1D	180KB
	{ MEDIA_TYPE_2D,  40, 1, 10,  512, MFM },	// 1D	200KB
//#if defined(SUPPORT_MEDIA_TYPE_1DD)
//	{ MEDIA_TYPE_2DD, 70, 1,  8,  512, MFM },	// 1DD	280KB
//	{ MEDIA_TYPE_2DD, 70, 1,  9,  512, MFM },	// 1DD	315KB
//	{ MEDIA_TYPE_2DD, 70, 1, 10,  512, MFM },	// 1DD	350KB
//	{ MEDIA_TYPE_2DD, 80, 1,  8,  512, MFM },	// 1DD	320KB
//	{ MEDIA_TYPE_2DD, 80, 1,  9,  512, MFM },	// 1DD	360KB
//	{ MEDIA_TYPE_2DD, 80, 1, 10,  512, MFM },	// 1DD	400KB
//#else
	{ MEDIA_TYPE_2D,  35, 2,  8,  512, MFM },	// 2D	280KB
	{ MEDIA_TYPE_2D,  35, 2,  9,  512, MFM },	// 2D	315KB
	{ MEDIA_TYPE_2D,  35, 2, 10,  512, MFM },	// 2D	350KB
	{ MEDIA_TYPE_2D,  40, 2,  8,  512, MFM },	// 2D	320KB
	{ MEDIA_TYPE_2D,  40, 2,  9,  512, MFM },	// 2D	360KB
	{ MEDIA_TYPE_2D,  40, 2, 10,  512, MFM },	// 2D	400KB
//#endif
	{ MEDIA_TYPE_2DD, 80, 2,  8,  512, MFM },	// 2DD	640KB
	{ MEDIA_TYPE_2DD, 80, 2,  9,  512, MFM },	// 2DD	720KB
	{ MEDIA_TYPE_2DD, 81, 2,  9,  512, MFM },	// 2DD	729KB, ASCII MSX
	{ MEDIA_TYPE_2DD, 80, 2, 10,  512, MFM },	// 2DD	800KB
	{ MEDIA_TYPE_2HD, 77, 2, 26,  256, MFM },	// 2HD	1001KB, MITSUBISHI/IBM
	{ MEDIA_TYPE_2HD, 80, 2, 15,  512, MFM },	// 2HC	1200KB, TOSHIBA/IBM
	{ MEDIA_TYPE_2HD, 77, 2,  8, 1024, MFM },	// 2HD	1232KB, NEC
	{ MEDIA_TYPE_144, 80, 2, 18,  512, MFM },	// 2HD	1440KB
	{ MEDIA_TYPE_144, 80, 2, 21,  512, MFM },	// 2HD	1680KB
	{ MEDIA_TYPE_144, 82, 2, 21,  512, MFM },	// 2HD	1722KB
	{ MEDIA_TYPE_144, 80, 2, 36,  512, MFM },	// 2ED	2880KB
	{ -1, 0, 0, 0, 0 },
};

#define IS_VALID_TRACK(offset) ((offset) >= 0x20 && (offset) < sizeof(buffer))

void DISK::open(const _TCHAR* file_path, int bank)
{
	// check current disk image
	if(inserted) {
		if(_tcsicmp(orig_path, file_path) == 0 && file_bank == bank) {
			return;
		}
		close();
	}
	if(bank < 0) {
		return;
	}
	memset(buffer, 0, sizeof(buffer));
	file_bank = 0;
	write_protected = false;
	media_type = MEDIA_TYPE_UNK;
	is_solid_image = is_fdi_image = is_1dd_image = false;
	trim_required = false;
	track_mfm = drive_mfm;
	
	// open disk image
	FILEIO *fio = new FILEIO();
	if(fio->Fopen(file_path, FILEIO_READ_BINARY)) {
		_tcscpy_s(orig_path, _MAX_PATH, file_path);
		_tcscpy_s(dest_path, _MAX_PATH, file_path);
		
		file_size.d = fio->FileLength();
		fio->Fseek(0, FILEIO_SEEK_SET);
		
		if(check_file_extension(file_path, _T(".d88")) || check_file_extension(file_path, _T(".d77")) || check_file_extension(file_path, _T(".1dd"))) {
			// d88 image
			uint32 offset = 0;
			for(int i = 0; i < bank; i++) {
				fio->Fseek(offset + 0x1c, SEEK_SET);
				offset += fio->FgetUint32_LE();
			}
			fio->Fseek(offset + 0x1c, FILEIO_SEEK_SET);
			file_size.d = fio->FgetUint32_LE();
			fio->Fseek(offset, FILEIO_SEEK_SET);
			fio->Fread(buffer, file_size.d, 1);
			file_bank = bank;
			if(check_file_extension(file_path, _T(".1dd"))) {
				is_1dd_image = true;
				media_type = MEDIA_TYPE_2DD;
			}
			inserted = changed = true;
//			trim_required = true;
			
			// fix sector number from big endian to little endian
			for(int trkside = 0; trkside < 164; trkside++) {
				pair offset;
				offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
				
				if(!IS_VALID_TRACK(offset.d)) {
					break;
				}
				uint8* t = buffer + offset.d;
				pair sector_num, data_size;
				sector_num.read_2bytes_le_from(t + 4);
				bool is_be = (sector_num.b.l == 0 && sector_num.b.h >= 4);
				if(is_be) {
					sector_num.read_2bytes_be_from(t + 4);
					sector_num.write_2bytes_le_to(t + 4);
				}
				for(int i = 0; i < sector_num.sd; i++) {
					if(is_be) {
						sector_num.write_2bytes_le_to(t + 4);
					}
					data_size.read_2bytes_le_from(t + 14);
					t += data_size.sd + 0x10;
				}
			}
		} else if(check_file_extension(file_path, _T(".td0"))) {
			// teledisk image
			try {
				inserted = changed = teledisk_to_d88(fio);
				_stprintf_s(dest_path, _MAX_PATH, _T("%s.D88"), file_path);
			} catch(...) {
				// failed to convert the disk image
			}
		} else if(check_file_extension(file_path, _T(".imd"))) {
			// imagedisk image
			try {
				inserted = changed = imagedisk_to_d88(fio);
				_stprintf_s(dest_path, _MAX_PATH, _T("%s.D88"), file_path);
			} catch(...) {
				// failed to convert the disk image
			}
		} else if(check_file_extension(file_path, _T(".dsk"))) {
			// cpdread image
			try {
				inserted = changed = cpdread_to_d88(fio);
				_stprintf_s(dest_path, _MAX_PATH, _T("%s.D88"), file_path);
			} catch(...) {
				// failed to convert the disk image
			}
		} else if(check_file_extension(file_path, _T(".2d"))  && file_size.d == 40 * 2 * 16 * 256) {
			// 2d image for SHARP X1 series
			inserted = changed = is_solid_image = solid_to_d88(fio, MEDIA_TYPE_2D, 40, 2, 16, 256, true);
		} else if(check_file_extension(file_path, _T(".img"))  && file_size.d == 70 * 1 * 16 * 256) {
			// img image for SONY SMC-70/777 series
			inserted = changed = is_solid_image = solid_to_d88(fio, MEDIA_TYPE_2DD, 70, 1, 16, 256, true);
		} else if(check_file_extension(file_path, _T(".sf7")) && file_size.d == 40 * 1 * 16 * 256) {
			// sf7 image for SEGA SC-3000 + SF-7000
			inserted = changed = is_solid_image = solid_to_d88(fio, MEDIA_TYPE_2D, 40, 1, 16, 256, true);
		}
		if(!inserted) {
			// check solid image file format
			bool is_fdi_tmp = check_file_extension(file_path, _T(".fdi"));
			for(int i = 0;; i++) {
				const fd_format_t *p = &fd_formats[i];
				if(p->type == -1) {
					break;
				}
				int len = p->ncyl * p->nside * p->nsec * p->size;
				// 4096 bytes: FDI header ???
				if(file_size.d == len + (is_fdi_tmp ? 4096 : 0)) {
					fio->Fseek(0, FILEIO_SEEK_SET);
					if(is_fdi_tmp) {
						is_fdi_image = true;
						fio->Fread(fdi_header, 4096, 1);
					}
					int type = p->type;
					int ncyl = p->ncyl;
					int nside = p->nside;
					int nsec = p->nsec;
					int size = p->size;
#if defined(SUPPORT_MEDIA_TYPE_1DD)
					if(type == MEDIA_TYPE_2D && nside == 2 && p->mfm) {
						type = MEDIA_TYPE_2DD;
						nside = 1;
						ncyl *= 2;
					}
#elif defined(_ANY2D88)
					if(open_as_1dd && type == MEDIA_TYPE_2D && nside == 2 && p->mfm) {
						type = MEDIA_TYPE_2DD;
						nside = 1;
						ncyl *= 2;
					}
					if(open_as_256 && (size == 512 || size == 1024)) {
						nsec *= size / 256;
						size = 256;
					}
#endif
//					if(solid_to_d88(fio, p->type, p->ncyl, p->nside, p->nsec, p->size, p->mfm)) {
					if(solid_to_d88(fio, type, ncyl, nside, nsec, size, p->mfm)) {
						inserted = changed = is_solid_image = true;
						break;
					}
				}
			}
		}
		if(fio->IsOpened()) {
			fio->Fclose();
		}
		if(inserted) {
			if(buffer[0x1a] != 0) {
				buffer[0x1a] = 0x10;
				write_protected = true;
			}
			crc32 = getcrc32(buffer, file_size.d);
		}
		if(media_type == MEDIA_TYPE_UNK) {
			if((media_type = buffer[0x1b]) == MEDIA_TYPE_2HD) {
				for(int trkside = 0; trkside < 164; trkside++) {
					pair offset;
					offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
					
					if(!IS_VALID_TRACK(offset.d)) {
						continue;
					}
					// track found
					uint8 *t = buffer + offset.d;
					pair sector_num, data_size;
					sector_num.read_2bytes_le_from(t + 4);
					data_size.read_2bytes_le_from(t + 14);
					
					if(sector_num.sd >= 18 && data_size.sd == 512) {
						media_type = MEDIA_TYPE_144;
					}
					break;
				}
			}
		}
		is_special_disk = 0;
#if defined(_FM7) || defined(_FM8) || defined(_FM77_VARIANTS) || defined(_FM77AV_VARIANTS)
		// FIXME: ugly patch for FM-7 Gambler Jiko Chuushin Ha, DEATH FORCE and Psy-O-Blade
		if(media_type == MEDIA_TYPE_2D) {
			// check first track
			pair offset, sector_num, data_size;
			offset.read_4bytes_le_from(buffer + 0x20);
			if(IS_VALID_TRACK(offset.d)) {
				// check the sector (c,h,r,n) = (0,0,7,1) or (0,0,f7,2)
				uint8* t = buffer + offset.d;
				sector_num.read_2bytes_le_from(t + 4);
				for(int i = 0; i < sector_num.sd; i++) {
					data_size.read_2bytes_le_from(t + 14);
					if(data_size.sd == 0x100 && t[0] == 0 && t[1] == 0 && t[2] == 7 && t[3] == 1) {
						static const uint8 gambler[] = {0xb7, 0xde, 0xad, 0xdc, 0xdd, 0xcc, 0xde, 0xd7, 0xb1, 0x20, 0xbc, 0xde, 0xba, 0xc1, 0xad, 0xb3, 0xbc, 0xdd, 0xca};
						if(memcmp((void *)(t + 0x30), gambler, sizeof(gambler)) == 0) {
							is_special_disk = SPECIAL_DISK_FM7_GAMBLER;
							break;
						}
					} else if(data_size.sd == 0x200 && t[0] == 0 && t[1] == 0 && t[2] == 0xf7 && t[3] == 2) {
						//"DEATHFORCE/77AV" + $f7*17 + $00 + $00
						static const uint8 deathforce[] ={
							0x44, 0x45, 0x41, 0x54, 0x48, 0x46, 0x4f, 0x52,
							0x43, 0x45, 0x2f, 0x37, 0x37, 0x41, 0x56, 0xf7,
							0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
							0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
							0x00, 0x00
						};
						if(memcmp((void *)(t + 0x10), deathforce, sizeof(deathforce)) == 0) {
							is_special_disk = SPECIAL_DISK_FM7_DEATHFORCE;
							break;
						}
					} else if(data_size.sd == 0x100 && t[0] == 0 && t[1] == 0 && t[2] == 1 && t[3] == 1) {
						//$03 + $2D + "PSY-O-BLADE   Copyright 1988 by T&E SOFT Inc" + $B6 + $FD + $05
						static const uint8 psyoblade_ipl1[] ={
							0x03, 0x2d, 0x50, 0x53, 0x59, 0xa5, 0x4f, 0xa5,
							0x42, 0x4c, 0x41, 0x44, 0x45, 0x20, 0x20, 0x20,
							0x43, 0x6f, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68,
							0x74, 0x20, 0x31, 0x39, 0x38, 0x38, 0x20, 0x62,
							0x79, 0x20, 0x54, 0x26, 0x45, 0x20, 0x53, 0x4f,
							0x46, 0x54, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0xb6,
							0xfd, 0x05
						};
						//IPL Signature1
						static const uint8 psyoblade_disk_1[] ={
							0xc3, 0x00, 0x01, 0x00, 0x1a, 0x50, 0x86, 0xff,
							0xb7, 0xfd, 0x10, 0xb7, 0xfd, 0x0f, 0x30, 0x8c,
							0x0e, 0x8d, 0x35, 0x30, 0x8c, 0x14, 0x8d, 0x30,
							0x30, 0x8c, 0x14, 0x8d, 0x2b, 0x20, 0xfe, 0x0a,
						};
						//$00 + $00 + $03 + $14 + "PSY-O-BLADE  DISK" + $B6 + $FD + $05
						static const uint8 psyoblade_disk_2[] ={
							0x00, 0x00, 0x03, 0x14, 0x50, 0x53, 0x59, 0xa5,
							0x4f, 0xa5, 0x42, 0x4c, 0x41, 0x44, 0x45, 0x20,
							0x20, 0x20, 0x44, 0x49, 0x53, 0x4B, 0x20
						};
						if(memcmp((void *)(t + 0x58), psyoblade_ipl1, sizeof(psyoblade_ipl1)) == 0) {
							is_special_disk = SPECIAL_DISK_FM77AV_PSYOBLADE;
							break;
						} else if(memcmp((void *)(t + 0x10), psyoblade_disk_1, sizeof(psyoblade_disk_1)) == 0) {
							if(memcmp((void *)(t + 0x40), psyoblade_disk_2, sizeof(psyoblade_disk_2)) == 0) {
								is_special_disk = SPECIAL_DISK_FM77AV_PSYOBLADE;
								break;
							}
						}
					}
					t += data_size.sd + 0x10;
				}
				if(is_special_disk == 0) {
					t = buffer + offset.d;
					sector_num.read_2bytes_le_from(t + 4);
				
					for(int i = 0; i < sector_num.sd; i++) {
						data_size.read_2bytes_le_from(t + 14);
						// FIXME : Check DEATH FORCE
						if(data_size.sd == 0x200 && t[0] == 0 && t[1] == 0 && t[2] == 0xf7 && t[3] == 0x02) {
							static const uint8 deathforce[] ={
								0x44, 0x45, 0x41, 0x54, 0x48, 0x46, 0x4f, 0x52,
								0x43, 0x45, 0x2f, 0x37, 0x37, 0x41, 0x56, 0xf7,
								0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
								0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7, 0xf7,
								0x00, 0x00};	//"DEATHFORCE/77AV" + $f7*17 + $00 + $00
							if(memcmp((void *)(t + 0x10), deathforce, sizeof(deathforce)) == 0) {
								is_special_disk = SPECIAL_DISK_FM7_DEATHFORCE;
							}
							break;
						}
						t += data_size.sd + 0x10;
					}
				}
				if(is_special_disk == 0) {
					t = buffer + offset.d;
					sector_num.read_2bytes_le_from(t + 4);
				
					for(int i = 0; i < sector_num.sd; i++) {
						data_size.read_2bytes_le_from(t + 14);
						// FIXME : Check Psy-O-Blade (Program)
						if(data_size.sd == 0x100 && t[0] == 0 && t[1] == 0 && t[2] == 0x01 && t[3] == 0x01) {
							static const uint8 psyoblade_ipl1[] ={
								0x03, 0x2d, 0x50, 0x53, 0x59, 0xa5, 0x4f, 0xa5,
								0x42, 0x4c, 0x41, 0x44, 0x45, 0x20, 0x20, 0x20,
								0x43, 0x6f, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68,
								0x74, 0x20, 0x31, 0x39, 0x38, 0x38, 0x20, 0x62,
								0x79, 0x20, 0x54, 0x26, 0x45, 0x20, 0x53, 0x4f,
								0x46, 0x54, 0x20, 0x49, 0x6e, 0x63, 0x2e, 0xb6,
								0xfd, 0x05};
							//$03 + $2D + "PSY-O-BLADE   Copyright 1988 by T&E SOFT Inc" + $B6 + $FD + $05
							if(memcmp((void *)(t + 0x58), psyoblade_ipl1, sizeof(psyoblade_ipl1)) == 0) {
								is_special_disk = SPECIAL_DISK_FM77AV_PSYOBLADE;
								//printf("Disk: PSY-O-BLADE\n");
							}
							break;
						}
						t += data_size.sd + 0x10;
					}
				}
				if(is_special_disk == 0) {
					t = buffer + offset.d;
					sector_num.read_2bytes_le_from(t + 4);
				
					for(int i = 0; i < sector_num.sd; i++) {
						data_size.read_2bytes_le_from(t + 14);
						// FIXME : Check Psy-O-Blade (Program)
						if(data_size.sd == 0x100 && t[0] == 0 && t[1] == 0 && t[2] == 0x01 && t[3] == 0x01) {
							//IPL Signature1
							static const uint8 psyoblade_disk_1[] ={
								0xc3, 0x00, 0x01, 0x00, 0x1a, 0x50, 0x86, 0xff,
								0xb7, 0xfd, 0x10, 0xb7, 0xfd, 0x0f, 0x30, 0x8c,
								0x0e, 0x8d, 0x35, 0x30, 0x8c, 0x14, 0x8d, 0x30,
								0x30, 0x8c, 0x14, 0x8d, 0x2b, 0x20, 0xfe, 0x0a,
							};
							//$00 + $00 + $03 + $14 + "PSY-O-BLADE  DISK" + $B6 + $FD + $05
							static const uint8 psyoblade_disk_2[] ={
								0x00, 0x00, 0x03, 0x14, 0x50, 0x53, 0x59, 0xa5,
								0x4f, 0xa5, 0x42, 0x4c, 0x41, 0x44, 0x45, 0x20,
								0x20, 0x20, 0x44, 0x49, 0x53, 0x4B, 0x20};
							if(memcmp((void *)(t + 0x10), psyoblade_disk_1, sizeof(psyoblade_disk_1)) == 0) {
								if(memcmp((void *)(t + 0x40), psyoblade_disk_2, sizeof(psyoblade_disk_2)) == 0) {
									is_special_disk = SPECIAL_DISK_FM77AV_PSYOBLADE;
									//printf("Disk: PSY-O-BLADE\n");
								}
							}
							break;
						}
						t += data_size.sd + 0x10;
					}
				}
			}
		}
#elif defined(_X1) || defined(_X1TWIN) || defined(_X1TURBO) || defined(_X1TURBOZ)
		// FIXME: ugly patch for X1turbo ALPHA and X1 Batten Tanuki
		if(media_type == MEDIA_TYPE_2D) {
			// check first track
			pair offset;
			offset.read_4bytes_le_from(buffer + 0x20);
			if(IS_VALID_TRACK(offset.d)) {
				// check first sector
				static const uint8 batten[] = {0xca, 0xde, 0xaf, 0xc3, 0xdd, 0x20, 0xc0, 0xc7, 0xb7};
				uint8 *t = buffer + offset.d;
#if defined(_X1TURBO) || defined(_X1TURBOZ)
//				if(strncmp((char *)(t + 0x11), "turbo ALPHA", 11) == 0) {
//					is_special_disk = SPECIAL_DISK_X1TURBO_ALPHA;
//				} else
#endif
				if(memcmp((void *)(t + 0x11), batten, sizeof(batten)) == 0) {
					is_special_disk = SPECIAL_DISK_X1_BATTEN;
				}
			}
		}
#endif
	}
	delete fio;
}

void DISK::close()
{
	// write disk image
	if(inserted) {
		if(trim_required) {
			trim_buffer();
			trim_required = false;
		}
		buffer[0x1a] = write_protected ? 0x10 : 0; // mey be changed
		
		if(/*!write_protected &&*/ file_size.d && getcrc32(buffer, file_size.d) != crc32) {
			// write image
			FILEIO* fio = new FILEIO();
			int pre_size = 0, post_size = 0;
			uint8 *pre_buffer = NULL, *post_buffer = NULL;
			
			// is this d88 format ?
			if(check_file_extension(dest_path, _T(".d88")) || check_file_extension(dest_path, _T(".d77")) || check_file_extension(dest_path, _T(".1dd"))) {
				if(fio->Fopen(dest_path, FILEIO_READ_BINARY)) {
					fio->Fseek(0, FILEIO_SEEK_END);
					uint32 total_size = fio->Ftell(), offset = 0;
					for(int i = 0; i < file_bank; i++) {
						fio->Fseek(offset + 0x1c, SEEK_SET);
						offset += fio->FgetUint32_LE();
					}
					if((pre_size = offset) > 0) {
						pre_buffer = (uint8 *)malloc(pre_size);
						fio->Fseek(0, FILEIO_SEEK_SET);
						fio->Fread(pre_buffer, pre_size, 1);
					}
					fio->Fseek(offset + 0x1c, SEEK_SET);
					offset += fio->FgetUint32_LE();
					if((post_size = total_size - offset) > 0) {
						post_buffer = (uint8 *)malloc(post_size);
						fio->Fseek(offset, FILEIO_SEEK_SET);
						fio->Fread(post_buffer, post_size, 1);
					}
					fio->Fclose();
				}
			}
			
			// is this solid image and was physical formatted ?
			if(is_solid_image) {
				bool formatted = false;
				int tracks = 0;
				
				for(int trkside = 0; trkside < 164; trkside++) {
					pair offset;
					offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
					
					if(!IS_VALID_TRACK(offset.d)) {
						continue;
					}
					if(solid_nside == 1 && (trkside & 1) == 1) {
						formatted = true;
					}
					tracks++;
					
					uint8* t = buffer + offset.d;
					pair sector_num, data_size;
					sector_num.read_2bytes_le_from(t + 4);
					
					if(sector_num.sd != solid_nsec) {
						formatted = true;
					}
					for(int i = 0; i < sector_num.sd; i++) {
						data_size.read_2bytes_le_from(t + 14);
						if(data_size.sd != solid_size) {
							formatted = true;
						}
						if(t[6] != (solid_mfm ? 0 : 0x40)) {
							formatted = true;
						}
						t += data_size.sd + 0x10;
					}
				}
				if(tracks != (solid_ncyl * solid_nside)) {
					formatted = true;
				}
				if(formatted) {
					_stprintf_s(dest_path, _MAX_PATH, _T("%s.D88"), orig_path);
					is_solid_image = false;
				}
			}
			
			if((FILEIO::IsFileExists(dest_path) && FILEIO::IsFileProtected(dest_path)) || !fio->Fopen(dest_path, FILEIO_WRITE_BINARY)) {
				_TCHAR tmp_path[_MAX_PATH];
				_stprintf_s(tmp_path, _MAX_PATH, _T("temporary_saved_floppy_disk_#%d.d88"), drive_num);
				fio->Fopen(bios_path(tmp_path), FILEIO_WRITE_BINARY);
			}
			if(fio->IsOpened()) {
				if(pre_buffer) {
					fio->Fwrite(pre_buffer, pre_size, 1);
				}
				if(is_solid_image) {
					if(is_fdi_image) {
						fio->Fwrite(fdi_header, 4096, 1);
					}
					for(int trkside = 0; trkside < 164; trkside++) {
						pair offset;
						offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
						
						if(!IS_VALID_TRACK(offset.d)) {
							continue;
						}
						uint8* t = buffer + offset.d;
						pair sector_num, data_size;
						sector_num.read_2bytes_le_from(t + 4);
						
						for(int i = 0; i < sector_num.sd; i++) {
							data_size.read_2bytes_le_from(t + 14);
							fio->Fwrite(t + 0x10, data_size.sd, 1);
							t += data_size.sd + 0x10;
						}
					}
				} else {
					fio->Fwrite(buffer, file_size.d, 1);
				}
				if(post_buffer) {
					fio->Fwrite(post_buffer, post_size, 1);
				}
				fio->Fclose();
			}
			if(pre_buffer) {
				free(pre_buffer);
			}
			if(post_buffer) {
				free(post_buffer);
			}
			delete fio;
		}
		ejected = true;
	}
	inserted = write_protected = false;
	file_size.d = 0;
	sector_size.sd = sector_num.sd = 0;
	sector = NULL;
}

#ifdef _ANY2D88
void DISK::save_as_d88(const _TCHAR* file_path)
{
	if(inserted) {
		FILEIO* fio = new FILEIO();
		if(fio->Fopen(file_path, FILEIO_WRITE_BINARY)) {
			if(is_1dd_image) {
				memcpy(tmp_buffer, buffer + 0x20, 4 * 82);
				for(int trk = 0; trk < 82; trk++) {
					memcpy(buffer + 0x20 + (trk * 2 + 0) * 4, tmp_buffer + trk * 4, 4);
					memset(buffer + 0x20 + (trk * 2 + 1) * 4, 0, 4);
				}
				buffer[0x1b] = MEDIA_TYPE_2DD;
			}
			fio->Fwrite(buffer, file_size.d, 1);
			fio->Fclose();
		}
		delete fio;
	}
}
#endif

bool DISK::get_track(int trk, int side)
{
	sector_size.sd = sector_num.sd = 0;
	invalid_format = false;
	//no_skew = true;

	// disk not inserted or invalid media type
	if(!(inserted && check_media_type())) {
		return false;
	}
	// search track
	int trkside = is_1dd_image ? trk : (trk * 2 + (side & 1));
	if(!(0 <= trkside && trkside < 164)) {
		return false;
	}
	cur_track = trk;
	cur_side = side;
	
	pair offset;
	offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
	
	if(!IS_VALID_TRACK(offset.d)) {
		return false;
	}
	
	// track found
	sector = buffer + offset.d;
	sector_num.read_2bytes_le_from(sector + 4);
	pair data_size;
	data_size.read_2bytes_le_from(sector + 14);
	
	// create each sector position in track
	track_mfm = false;
	if(sector_num.sd == 0) {
		track_mfm = drive_mfm;
	} else {
		uint8* t = sector;
		for(int i = 0; i < sector_num.sd; i++) {
			data_size.read_2bytes_le_from(t + 14);
			// t[6]: 0x00 = double-density, 0x40 = single-density
			if(t[6] == 0x00) {
				track_mfm = true;
				break;
			}
			t += data_size.sd + 0x10;
		}
	}
	int sync_size  = track_mfm ? 12 : 6;
	int am_size = track_mfm ? 3 : 0;
	int gap0_size = track_mfm ? 80 : 40;
	int gap1_size = track_mfm ? 50 : 26;
	int gap2_size = track_mfm ? 22 : 11;
	int gap3_size = 0, gap4_size;
	
	if(media_type == MEDIA_TYPE_144 || media_type == MEDIA_TYPE_2HD) {
		if(track_mfm) {
			if(data_size.sd ==  256 && sector_num.sd == 26) gap3_size =  54;
			if(data_size.sd ==  512 && sector_num.sd == 15) gap3_size =  84;
			if(data_size.sd == 1024 && sector_num.sd ==  8) gap3_size = 116;
		} else {
			if(data_size.sd ==  128 && sector_num.sd == 26) gap3_size =  27;
			if(data_size.sd ==  256 && sector_num.sd == 15) gap3_size =  42;
			if(data_size.sd ==  512 && sector_num.sd ==  8) gap3_size =  58;
		}
	} else {
		if(track_mfm) {
			if(data_size.sd ==  256 && sector_num.sd == 16) gap3_size =  51;
			if(data_size.sd ==  512 && sector_num.sd ==  9) gap3_size =  80;
			if(data_size.sd == 1024 && sector_num.sd ==  5) gap3_size = 116;
		} else {
			if(data_size.sd ==  128 && sector_num.sd == 16) gap3_size =  27;
			if(data_size.sd ==  256 && sector_num.sd ==  9) gap3_size =  42;
			if(data_size.sd ==  512 && sector_num.sd ==  5) gap3_size =  58;
		}
	}
	
	uint8* t = sector;
	int total = sync_size + (am_size + 1);
	
	for(int i = 0; i < sector_num.sd; i++) {
		data_size.read_2bytes_le_from(t + 14);
		total += sync_size + (am_size + 1) + (4 + 2) + gap2_size;
		if(data_size.sd > 0) {
			total += sync_size + (am_size + 1);
			total += data_size.sd + 2;
		}
//		if(t[2] != i + 1) {
//			no_skew = false;
//		}
		t += data_size.sd + 0x10;
	}
	if(gap3_size == 0) {
		gap3_size = (get_track_size() - total - gap0_size - gap1_size) / (sector_num.sd + 1);
	}
	gap4_size = get_track_size() - total - gap0_size - gap1_size - gap3_size * sector_num.sd;
	
	if(gap3_size < 8 || gap4_size < 8) {
		gap0_size = gap1_size = gap3_size = (get_track_size() - total) / (2 + sector_num.sd + 1);
		gap4_size = get_track_size() - total - gap0_size - gap1_size - gap3_size * sector_num.sd;
	}
	if(gap3_size < 8 || gap4_size < 8) {
		gap0_size = gap1_size = gap3_size = gap4_size = 32;
		invalid_format = true;
	}
	int preamble_size = gap0_size + sync_size + (am_size + 1) + gap1_size;
	
	t = sector;
	total = preamble_size;
	sync_position[array_length(sync_position) - 1] = gap0_size; // sync position in preamble
	
	for(int i = 0; i < sector_num.sd; i++) {
		data_size.read_2bytes_le_from(t + 14);
		if(invalid_format) {
			total = preamble_size + (get_track_size() - preamble_size - gap4_size) * i / sector_num.sd;
		}
		sync_position[i] = total;
		total += sync_size;
		am1_position[i] = total;
		total += am_size + 1;
		id_position[i] = total;
		total += (4 + 2) + gap2_size;
		if(data_size.sd > 0) {
			total += sync_size + (am_size + 1);
			data_position[i] = total;
			total += data_size.sd + 2;
		} else {
			data_position[i] = total; // FIXME
		}
		total += gap3_size;
		t += data_size.sd + 0x10;
	}
	return true;
}

bool DISK::make_track(int trk, int side)
{
	int track_size = get_track_size();
	
	if(!get_track(trk, side)) {
		// create a dummy track
		for(int i = 0; i < track_size; i++) {
			track[i] = rand();
		}
		return false;
	}
	
	// make track image
	int sync_size  = track_mfm ? 12 : 6;
	int am_size = track_mfm ? 3 : 0;
	int gap2_size = track_mfm ? 22 : 11;
	uint8 gap_data = track_mfm ? 0x4e : 0xff;
	
	// preamble
	memset(track, gap_data, track_size);
	int q = sync_position[array_length(sync_position) - 1];
	
	// sync
	for(int i = 0; i < sync_size; i++) {
		track[q++] = 0x00;
	}
	// index mark
	for(int i = 0; i < am_size; i++) {
		track[q++] = 0xc2;
	}
	track[q++] = 0xfc;
	
	// sectors
	uint8 *t = sector;
	
	for(int i = 0; i < sector_num.sd; i++) {
		pair data_size;
		data_size.read_2bytes_le_from(t + 14);
		int p = sync_position[i];
		
		// sync
		for(int j = 0; j < sync_size; j++) {
			if(p < track_size) track[p++] = 0x00;
		}
		// am1
		for(int j = 0; j < am_size; j++) {
			if(p < track_size) track[p++] = 0xa1;
		}
		if(p < track_size) track[p++] = 0xfe;
		// id
		if(p < track_size) track[p++] = t[0];
		if(p < track_size) track[p++] = t[1];
		if(p < track_size) track[p++] = t[2];
		if(p < track_size) track[p++] = t[3];
		uint16 crc = 0;
		crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[0]]);
		crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[1]]);
		crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[2]]);
		crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[3]]);
		if(p < track_size) track[p++] = (crc >> 8) & 0xff;
		if(p < track_size) track[p++] = (crc >> 0) & 0xff;
		// gap2
		for(int j = 0; j < gap2_size; j++) {
			if(p < track_size) track[p++] = gap_data;
		}
		// data field
		if(data_size.sd > 0) {
			// sync
			for(int j = 0; j < sync_size; j++) {
				if(p < track_size) track[p++] = 0x00;
			}
			// am2
			for(int j = 0; j < am_size; j++) {
				if(p < track_size) track[p++] = 0xa1;
			}
			if(p < track_size) track[p++] = (t[7] != 0) ? 0xf8 : 0xfb;
			// data
			crc = 0;
			for(int j = 0; j < data_size.sd; j++) {
				if(p < track_size) track[p++] = t[0x10 + j];
				crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[0x10 + j]]);
			}
			if(p < track_size) track[p++] = (crc >> 8) & 0xff;
			if(p < track_size) track[p++] = (crc >> 0) & 0xff;
		}
		t += data_size.sd + 0x10;
	}
	return true;
}

bool DISK::get_sector(int trk, int side, int index)
{
	sector_size.sd = sector_num.sd = 0;
	sector = NULL;
	
	// disk not inserted or invalid media type
	if(!(inserted && check_media_type())) {
		return false;
	}

	// search track
	if(trk == -1 && side == -1) {
		trk = cur_track;
		side = cur_side;
	}
	int trkside = is_1dd_image ? trk : (trk * 2 + (side & 1));
	if(!(0 <= trkside && trkside < 164)) {
		return false;
	}
	pair offset;
	offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
	
	if(!IS_VALID_TRACK(offset.d)) {
		return false;
	}
	
	// track found
	uint8* t = buffer + offset.d;
	sector_num.read_2bytes_le_from(t + 4);
	
	if(index >= sector_num.sd) {
		return false;
	}
	
	// skip sector
	for(int i = 0; i < index; i++) {
		pair data_size;
		data_size.read_2bytes_le_from(t + 14);
		t += data_size.sd + 0x10;
	}
	set_sector_info(t);
	return true;
}

void DISK::set_sector_info(uint8 *t)
{
	// header info
	id[0] = t[0];
	id[1] = t[1];
	id[2] = t[2];
	id[3] = t[3];
	uint16 crc = 0;
	crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[0]]);
	crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[1]]);
	crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[2]]);
	crc = (uint16)((crc << 8) ^ crc_table[(uint8)(crc >> 8) ^ t[3]]);
	id[4] = (crc >> 8) & 0xff;
	id[5] = (crc >> 0) & 0xff;
	// http://www,gnu-darwin.or.jp/www001/src/ports/emulators/quasi88/work/quasi88-0.6.3/document/FORMAT.TXT
	// t[6]: 0x00 = double-density, 0x40 = single-density
	// t[7]: 0x00 = normal, 0x10 = deleted mark
	// t[8]: 0x00 = valid, 0x10 = valid (deleted data), 0xa0 = id crc error, 0xb0 = data crc error, 0xe0 = address mark missing, 0xf0 = data mark missing
	density = t[6];
	deleted = (t[7] != 0);
   
//	if(ignore_crc()) {
//		crc_error = false;
//	} else {
//		crc_error = (((t[8] & 0xf0) != 0x00) && ((t[8] & 0xf0) != 0x10));
		addr_crc_error = ((t[8] & 0xf0) == 0xa0);
		data_crc_error = ((t[8] & 0xf0) == 0xb0);
//	}
	sector = t + 0x10;
	sector_size.read_2bytes_le_from(t + 14);
}

void DISK::set_deleted(bool value)
{
	if(sector != NULL) {
		uint8 *t = sector - 0x10;
		t[7] = value ? 0x10 : 0;
		if((t[8] & 0xf0) == 0x00 || (t[8] & 0xf0) == 0x10) {
			t[8] = (t[8] & 0x0f) | t[7];
 		}
	}
	deleted = value;
}

void DISK::set_data_crc_error(bool value)
{
	if(sector != NULL) {
		uint8 *t = sector - 0x10;
		t[8] = (t[8] & 0x0f) | (value ? 0xb0 : t[7]);
	}
	data_crc_error = value;
}

void DISK::set_data_mark_missing()
{
	if(sector != NULL) {
		uint8 *t = sector - 0x10;
		//t[8] = (t[8] & 0x0f) | (value ? 0xb0 : t[7]); // FIXME: always data crc error ?
		t[8] = (t[8] & 0x0f) | 0xf0;
		t[14] = t[15] = 0;

	}
	//addr_crc_error = value;
	data_crc_error = false;
}

bool DISK::format_track(int trk, int side)
{
	// disk not inserted or invalid media type
	if(!(inserted && check_media_type())) {
		return false;
	}
	
	// search track
	int trkside = is_1dd_image ? trk : (trk * 2 + (side & 1));
	if(!(0 <= trkside && trkside < 164)) {
		return false;
	}
	
	// create new empty track
	if(trim_required) {
		trim_buffer();
		trim_required = false;
	}
	memset(buffer + DISK_BUFFER_SIZE, 0, sizeof(buffer) - DISK_BUFFER_SIZE);
	pair offset;
	offset.d = DISK_BUFFER_SIZE;
	offset.write_4bytes_le_to(buffer + 0x20 + trkside * 4);
	
	trim_required = true;
	sector_num.sd = 0;
	track_mfm = drive_mfm;
	
	return true;
}

void DISK::insert_sector(uint8 c, uint8 h, uint8 r, uint8 n, bool deleted, bool data_crc_error, uint8 fill_data, int length)
{
	uint8* t = buffer + DISK_BUFFER_SIZE;
	
	sector_num.sd++;
	for(int i = 0; i < (sector_num.sd - 1); i++) {
		t[4] = sector_num.b.l;
		t[5] = sector_num.b.h;
		pair data_size;
		data_size.read_2bytes_le_from(t + 14);
		t += data_size.sd + 0x10;
	}
	t[0] = c;
	t[1] = h;
	t[2] = r;
	t[3] = n;
	t[4] = sector_num.b.l;
	t[5] = sector_num.b.h;
	t[6] = track_mfm ? 0 : 0x40;
	t[7] = deleted ? 0x10 : 0;
	t[8] = data_crc_error ? 0xb0 : t[7];
	t[14] = (length >> 0) & 0xff;
	t[15] = (length >> 8) & 0xff;
	memset(t + 16, fill_data, length);
	
	set_sector_info(t);
}

void DISK::sync_buffer()
{
	if(trim_required) {
		trim_buffer();
		trim_required = false;
	}
}

void DISK::trim_buffer()
{
	int max_tracks = 164;
	uint32 dest_offset = 0x2b0;
	
	// copy header
	memset(tmp_buffer, 0, sizeof(tmp_buffer));
	memcpy(tmp_buffer, buffer, 0x20);
	
	// check max tracks
	for(int trkside = 0; trkside < 164; trkside++) {
		pair src_trk_offset;
		src_trk_offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
		if(src_trk_offset.d != 0) {
#if 1
			if(src_trk_offset.d < 0x2b0) {
				max_tracks = (src_trk_offset.d - 0x20) >> 2;
			}
#else
			if(src_trk_offset.d != 0x2b0) {
				max_tracks = (src_trk_offset.d - 0x20) >> 2;
				if(max_tracks > 164) {
					dest_offset = 0x20 + max_tracks * 4);
				}
			}
#endif
			break;
		}
	}
	
	// copy tracks
	for(int trkside = 0; trkside < max_tracks; trkside++) {
		pair src_trk_offset;
		src_trk_offset.read_4bytes_le_from(buffer + 0x20 + trkside * 4);
		
		pair dest_trk_offset;
		dest_trk_offset.d = 0;
		
		if(IS_VALID_TRACK(src_trk_offset.d)) {
			uint8* t = buffer + src_trk_offset.d;
			pair sector_num, data_size;
			sector_num.read_2bytes_le_from(t + 4);
			if(sector_num.sd != 0) {
				dest_trk_offset.d = dest_offset;
				for(int i = 0; i < sector_num.sd; i++) {
					data_size.read_2bytes_le_from(t + 14);
					memcpy(tmp_buffer + dest_offset, t, data_size.sd + 0x10);
					dest_offset += data_size.sd + 0x10;
					t += data_size.sd + 0x10;
				}
			}
		}
		dest_trk_offset.write_4bytes_le_to(tmp_buffer + 0x20 + trkside * 4);
	}
	
	// update file size
	file_size.d = dest_offset;
	file_size.write_4bytes_le_to(tmp_buffer + 0x1c);
	
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, tmp_buffer, min(sizeof(buffer), file_size.d));
	//memcpy(buffer, tmp_buffer, (file_size.d > sizeof(buffer)) ? sizeof(buffer) : file_size.d);
}

int DISK::get_rpm()
{
	if(drive_rpm != 0) {
		return drive_rpm;
	} else if(inserted) {
		return (media_type == MEDIA_TYPE_2HD) ? 360 : 300;
	} else {
		return (drive_type == DRIVE_TYPE_2HD) ? 360 : 300;
	}
}

int DISK::get_track_size()
{
	if(inserted) {
#if defined(_FM7) || defined(_FM8) || defined(_FM77_VARIANTS) || defined(_FM77AV_VARIANTS)
		if(is_special_disk == SPECIAL_DISK_FM7_DEATHFORCE) {
			return media_type == MEDIA_TYPE_144 ? 12500 : media_type == MEDIA_TYPE_2HD ? 10410 : drive_mfm ? 6300 : 3100;
		}
#endif
		return media_type == MEDIA_TYPE_144 ? 12500 : media_type == MEDIA_TYPE_2HD ? 10410 : track_mfm ? 6250 : 3100;
	} else {
#if defined(_FM7) || defined(_FM8) || defined(_FM77_VARIANTS) || defined(_FM77AV_VARIANTS)
		if(is_special_disk == SPECIAL_DISK_FM7_DEATHFORCE) {
			return media_type == MEDIA_TYPE_144 ? 12500 : media_type == MEDIA_TYPE_2HD ? 10410 : drive_mfm ? 6300 : 3100;
		}
#endif
		return drive_type == DRIVE_TYPE_144 ? 12500 : drive_type == DRIVE_TYPE_2HD ? 10410 : drive_mfm ? 6250 : 3100;
	}
}

double DISK::get_usec_per_track()
{
	return 1000000.0 / (get_rpm() / 60.0);
}

double DISK::get_usec_per_bytes(int bytes)
{
#if defined(_FM77AV_VARIANTS)
	if(is_special_disk == SPECIAL_DISK_FM77AV_PSYOBLADE) {
		return 1000000.0 / (get_track_size() * (get_rpm() / 60.0) * 2.2) * bytes;
	}
#endif	
	return 1000000.0 / (get_track_size() * (get_rpm() / 60.0)) * bytes;
}

int DISK::get_bytes_per_usec(double usec)
{
	return (int)(usec / get_usec_per_bytes(1) + 0.5);
}

bool DISK::check_media_type()
{
	switch(drive_type) {
	case DRIVE_TYPE_2D:
#if defined(_FM77AV40) || defined(_FM77AV40EX) || defined(_FM77AV40SX) || \
	defined(_FM77AV20) || defined(_FM77AV20EX) || defined(_FM77AV20SX)
		return (media_type == MEDIA_TYPE_2D || media_type == MEDIA_TYPE_2DD);
#else		
		return (media_type == MEDIA_TYPE_2D);
#endif		
	case DRIVE_TYPE_2DD:
		return (media_type == MEDIA_TYPE_2D || media_type == MEDIA_TYPE_2DD);
	case DRIVE_TYPE_2HD:
		return (media_type == MEDIA_TYPE_2HD);
	case DRIVE_TYPE_144:
		return (media_type == MEDIA_TYPE_144);
	case DRIVE_TYPE_UNK:
		return true; // always okay
	}
	return false;
}

// image decoder

#define COPYBUFFER(src, size) { \
	if(file_size.d + (size) > DISK_BUFFER_SIZE) { \
		return false; \
	} \
	memcpy(buffer + file_size.d, (src), (size)); \
	file_size.d += (size); \
}

typedef struct {
	char title[17];
	uint8 rsrv[9];
	uint8 protect;
	uint8 type;
	uint32 size;
	uint32 trkptr[164];
} d88_hdr_t;

typedef struct {
	uint8 c, h, r, n;
	uint16 nsec;
	uint8 dens, del, stat;
	uint8 rsrv[5];
	uint16 size;
} d88_sct_t;

// teledisk image decoder

/*
	this teledisk image decoder is based on:
	
		LZHUF.C English version 1.0 based on Japanese version 29-NOV-1988
		LZSS coded by Haruhiko OKUMURA
		Adaptive Huffman Coding coded by Haruyasu YOSHIZAKI
		Edited and translated to English by Kenji RIKITAKE
		TDLZHUF.C by WTK
*/

#define STRING_BUFFER_SIZE	4096
#define LOOKAHEAD_BUFFER_SIZE	60
#define THRESHOLD		2
#define N_CHAR			(256 - THRESHOLD + LOOKAHEAD_BUFFER_SIZE)
#define TABLE_SIZE		(N_CHAR * 2 - 1)
#define ROOT_POSITION		(TABLE_SIZE - 1)
#define MAX_FREQ		0x8000

static uint8 td_text_buf[STRING_BUFFER_SIZE + LOOKAHEAD_BUFFER_SIZE - 1];
static uint16 td_ptr;
static uint16 td_bufcnt, td_bufndx, td_bufpos;
static uint16 td_ibufcnt, td_ibufndx;
static uint8 td_inbuf[512];
static uint16 td_freq[TABLE_SIZE + 1];
static short td_prnt[TABLE_SIZE + N_CHAR];
static short td_son[TABLE_SIZE];
static uint16 td_getbuf;
static uint8 td_getlen;

static const uint8 td_d_code[256] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0d, 0x0d, 0x0d, 0x0d, 0x0e, 0x0e, 0x0e, 0x0e, 0x0f, 0x0f, 0x0f, 0x0f,
	0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
	0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15, 0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
	0x18, 0x18, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1b, 0x1c, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f, 0x1f,
	0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23, 0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
	0x28, 0x28, 0x29, 0x29, 0x2a, 0x2a, 0x2b, 0x2b, 0x2c, 0x2c, 0x2d, 0x2d, 0x2e, 0x2e, 0x2f, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
};
static const uint8 td_d_len[256] = {
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

static int td_next_word(FILEIO* fio)
{
	if(td_ibufndx >= td_ibufcnt) {
		td_ibufndx = td_ibufcnt = 0;
		memset(td_inbuf, 0, 512);
		for(int i = 0; i < 512; i++) {
			int d = fio->Fgetc();
			if(d == EOF) {
				if(i) {
					break;
				}
				return(-1);
			}
			td_inbuf[i] = d;
			td_ibufcnt = i + 1;
		}
	}
	while(td_getlen <= 8) {
		td_getbuf |= td_inbuf[td_ibufndx++] << (8 - td_getlen);
		td_getlen += 8;
	}
	return 0;
}

static int td_get_bit(FILEIO* fio)
{
	if(td_next_word(fio) < 0) {
		return -1;
	}
	short i = td_getbuf;
	td_getbuf <<= 1;
	td_getlen--;
	return (i < 0) ? 1 : 0;
}

static int td_get_byte(FILEIO* fio)
{
	if(td_next_word(fio) != 0) {
		return -1;
	}
	uint16 i = td_getbuf;
	td_getbuf <<= 8;
	td_getlen -= 8;
	i >>= 8;
	return (int)i;
}

static void td_start_huff()
{
	int i, j;
	for(i = 0; i < N_CHAR; i++) {
		td_freq[i] = 1;
		td_son[i] = i + TABLE_SIZE;
		td_prnt[i + TABLE_SIZE] = i;
	}
	i = 0; j = N_CHAR;
	while(j <= ROOT_POSITION) {
		td_freq[j] = td_freq[i] + td_freq[i + 1];
		td_son[j] = i;
		td_prnt[i] = td_prnt[i + 1] = j;
		i += 2; j++;
	}
	td_freq[TABLE_SIZE] = 0xffff;
	td_prnt[ROOT_POSITION] = 0;
}

static void td_reconst()
{
	short i, j = 0, k;
	uint16 f, l;
	for(i = 0; i < TABLE_SIZE; i++) {
		if(td_son[i] >= TABLE_SIZE) {
			td_freq[j] = (td_freq[i] + 1) / 2;
			td_son[j] = td_son[i];
			j++;
		}
	}
	for(i = 0, j = N_CHAR; j < TABLE_SIZE; i += 2, j++) {
		k = i + 1;
		f = td_freq[j] = td_freq[i] + td_freq[k];
		for(k = j - 1; f < td_freq[k]; k--);
		k++;
		l = (j - k) * 2;
		memmove(&td_freq[k + 1], &td_freq[k], l);
		td_freq[k] = f;
		memmove(&td_son[k + 1], &td_son[k], l);
		td_son[k] = i;
	}
	for(i = 0; i < TABLE_SIZE; i++) {
		if((k = td_son[i]) >= TABLE_SIZE) {
			td_prnt[k] = i;
		} else {
			td_prnt[k] = td_prnt[k + 1] = i;
		}
	}
}

static void td_update(int c)
{
	int i, j, k, l;
	if(td_freq[ROOT_POSITION] == MAX_FREQ) {
		td_reconst();
	}
	c = td_prnt[c + TABLE_SIZE];
	do {
		k = ++td_freq[c];
		if(k > td_freq[l = c + 1]) {
			while(k > td_freq[++l]);
			l--;
			td_freq[c] = td_freq[l];
			td_freq[l] = k;
			i = td_son[c];
			td_prnt[i] = l;
			if(i < TABLE_SIZE) {
				td_prnt[i + 1] = l;
			}
			j = td_son[l];
			td_son[l] = i;
			td_prnt[j] = c;
			if(j < TABLE_SIZE) {
				td_prnt[j + 1] = c;
			}
			td_son[c] = j;
			c = l;
		}
	}
	while((c = td_prnt[c]) != 0);
}

static short td_decode_char(FILEIO* fio)
{
	int ret;
	uint16 c = td_son[ROOT_POSITION];
	while(c < TABLE_SIZE) {
		if((ret = td_get_bit(fio)) < 0) {
			return -1;
		}
		c += (unsigned)ret;
		c = td_son[c];
	}
	c -= TABLE_SIZE;
	td_update(c);
	return c;
}

static short td_decode_position(FILEIO* fio)
{
	short bit;
	uint16 i, j, c;
	if((bit = td_get_byte(fio)) < 0) {
		return -1;
	}
	i = (uint16)bit;
	c = (uint16)td_d_code[i] << 6;
	j = td_d_len[i] - 2;
	while(j--) {
		if((bit = td_get_bit(fio)) < 0) {
			 return -1;
		}
		i = (i << 1) + bit;
	}
	return (c | i & 0x3f);
}

static void td_init_decode()
{
	td_ibufcnt= td_ibufndx = td_bufcnt = td_getbuf = 0;
	td_getlen = 0;
	td_start_huff();
	for(int i = 0; i < STRING_BUFFER_SIZE - LOOKAHEAD_BUFFER_SIZE; i++) {
		td_text_buf[i] = ' ';
	}
	td_ptr = STRING_BUFFER_SIZE - LOOKAHEAD_BUFFER_SIZE;
}

static int td_decode(FILEIO* fio, uint8 *buf, int len)
{
	short c, pos;
	int  count;
	for(count = 0; count < len;) {
		if(td_bufcnt == 0) {
			if((c = td_decode_char(fio)) < 0) {
				return count;
			}
			if(c < 256) {
				*(buf++) = (uint8)c;
				td_text_buf[td_ptr++] = (uint8)c;
				td_ptr &= (STRING_BUFFER_SIZE - 1);
				count++;
			} else {
				if((pos = td_decode_position(fio)) < 0) {
					return count;
				}
				td_bufpos = (td_ptr - pos - 1) & (STRING_BUFFER_SIZE - 1);
				td_bufcnt = c - 255 + THRESHOLD;
				td_bufndx = 0;
			}
		} else {
			while(td_bufndx < td_bufcnt && count < len) {
				c = td_text_buf[(td_bufpos + td_bufndx) & (STRING_BUFFER_SIZE - 1)];
				*(buf++) = (uint8)c;
				td_bufndx++;
				td_text_buf[td_ptr++] = (uint8)c;
				td_ptr &= (STRING_BUFFER_SIZE - 1);
				count++;
			}
			if(td_bufndx >= td_bufcnt) {
				td_bufndx = td_bufcnt = 0;
			}
		}
	}
	return count;
}

typedef struct {
	char sig[3];
	uint8 unknown;
	uint8 ver;
	uint8 dens;
	uint8 type;
	uint8 flag;
	uint8 dos;
	uint8 sides;
	uint16 crc;
} td_hdr_t;

typedef struct {
	uint16 crc;
	uint16 len;
	uint8 ymd[3];
	uint8 hms[3];
} td_cmt_t;

typedef struct {
	uint8 nsec, trk, head;
	uint8 crc;
} td_trk_t;

typedef struct {
	uint8 c, h, r, n;
	uint8 ctrl, crc;
} td_sct_t;

bool DISK::teledisk_to_d88(FILEIO *fio)
{
	td_hdr_t hdr;
	td_cmt_t cmt;
	td_trk_t trk;
	td_sct_t sct;
	uint8 obuf[512];
	bool temporary = false;
	
	// check teledisk header
	fio->Fseek(0, FILEIO_SEEK_SET);
	fio->Fread(&hdr, sizeof(td_hdr_t), 1);
	if(hdr.sig[0] == 't' && hdr.sig[1] == 'd') {
		// this image is compressed
		// decompress to the temporary file
		FILEIO* fio_tmp = new FILEIO();
		if(!fio_tmp->Fopen(bios_path(_T("teledisk.$$$")), FILEIO_WRITE_BINARY)) {
			delete fio_tmp;
			return false;
		}
		int rd = 1;
		td_init_decode();
		do {
			if((rd = td_decode(fio, obuf, 512)) > 0) {
				fio_tmp->Fwrite(obuf, rd, 1);
			}
		}
		while(rd > 0);
		fio_tmp->Fclose();
		delete fio_tmp;
		temporary = true;
		
		// reopen the temporary file
		fio->Fclose();
		if(!fio->Fopen(_T("teledisk.$$$"), FILEIO_READ_BINARY)) {
			return false;
		}
	} else if(hdr.sig[0] == 'T' && hdr.sig[1] == 'D') {
		// this image is not compressed
	} else {
		return false;
	}
	if(hdr.flag & 0x80) {
		// skip comment
		fio->Fread(&cmt, sizeof(td_cmt_t), 1);
		fio->Fseek(cmt.len, FILEIO_SEEK_CUR);
	}
	
	// create d88 header
	d88_hdr_t d88_hdr;
	d88_sct_t d88_sct;
	
	memset(&d88_hdr, 0, sizeof(d88_hdr_t));
	//_strcpy_s(d88_hdr.title, sizeof(d88_hdr.title), "TELEDISK");
	strncpy(d88_hdr.title, "TELEDISK", sizeof(d88_hdr.title));
	d88_hdr.protect = 0; // non-protected
	
	file_size.d = 0;
	COPYBUFFER(&d88_hdr, sizeof(d88_hdr_t));
	
	// create tracks
	int trkcnt = 0, trkptr = sizeof(d88_hdr_t);
	fio->Fread(&trk, sizeof(td_trk_t), 1);
	while(trk.nsec != 0xff) {
		d88_hdr.trkptr[trkcnt++] = trkptr;
		if(hdr.sides == 1) {
			trkcnt++;
		}
		
		// read sectors in this track
		for(int i = 0; i < trk.nsec; i++) {
			uint8 buf[2048], dst[2048];
			memset(buf, 0, sizeof(buf));
			memset(dst, 0, sizeof(dst));
			
			// read sector header
			fio->Fread(&sct, sizeof(td_sct_t), 1);
			
			// create d88 sector header
			memset(&d88_sct, 0, sizeof(d88_sct_t));
			d88_sct.c = sct.c;
			d88_sct.h = sct.h;
			d88_sct.r = sct.r;
			d88_sct.n = sct.n;
			d88_sct.nsec = trk.nsec;
			d88_sct.dens = (hdr.dens & 0x80) ? 0x40 : 0;
			d88_sct.del = (sct.ctrl & 4) ? 0x10 : 0;
			d88_sct.stat = (sct.ctrl & 2) ? 0xb0 : d88_sct.del;
			d88_sct.size = secsize[sct.n & 3];
			
			// create sector image
			if(sct.ctrl & 0x30) {
				d88_sct.stat = 0xf0; // data mark missing
				d88_sct.size = 0;
			} else {
				// read sector source
				int len = fio->Fgetc();
				len += fio->Fgetc() * 256 - 1;
				int flag = fio->Fgetc(), d = 0;
				fio->Fread(buf, len, 1);
				
				// convert
				if(flag == 0) {
					memcpy(dst, buf, len);
				} else if(flag == 1) {
					pair len2;
					len2.read_2bytes_le_from(buf);
					while(len2.sd--) {
						dst[d++] = buf[2];
						dst[d++] = buf[3];
					}
				} else if(flag == 2) {
					for(int s = 0; s < len;) {
						int type = buf[s++];
						int len2 = buf[s++];
						if(type == 0) {
							while(len2--) {
								dst[d++] = buf[s++];
							}
						} else if(type < 5) {
							uint8 pat[256];
							int n = 2;
							while(type-- > 1) {
								n *= 2;
							}
							for(int j = 0; j < n; j++) {
								pat[j] = buf[s++];
							}
							while(len2--) {
								for(int j = 0; j < n; j++) {
									dst[d++] = pat[j];
								}
							}
						} else {
							break; // unknown type
						}
					}
				} else {
					break; // unknown flag
				}
			}
			
			// copy to d88
			COPYBUFFER(&d88_sct, sizeof(d88_sct_t));
			COPYBUFFER(dst, d88_sct.size);
			trkptr += sizeof(d88_sct_t) + d88_sct.size;
		}
		// read next track
		fio->Fread(&trk, sizeof(td_trk_t), 1);
	}
	d88_hdr.type = ((hdr.dens & 3) == 2) ? MEDIA_TYPE_2HD : ((trkcnt >> 1) > 45) ? MEDIA_TYPE_2DD : MEDIA_TYPE_2D;
	d88_hdr.size = trkptr;
	memcpy(buffer, &d88_hdr, sizeof(d88_hdr_t));
	
	if(temporary) {
		FILEIO::RemoveFile(_T("teledisk.$$$"));
	}
	return true;
}

// imagedisk image decoder (from MESS formats/imd_dsk.c by Mr.Miodrag Milanovic)

bool DISK::imagedisk_to_d88(FILEIO *fio)
{
	//d88_hdr_t d88_hdr;
	//d88_sct_t d88_sct;
	int size = fio->FileLength();
	fio->Fseek(0, FILEIO_SEEK_SET);
	fio->Fread(tmp_buffer, size, 1);
 	
	if(memcmp(tmp_buffer, "IMD ", 4) != 0) {
		return false;
	}
 	
	int pos;
	for(pos = 0; pos < size && tmp_buffer[pos] != 0x1a; pos++);
	pos++;
	
	if(pos >= size) {
		return false;
	}
	
	// create d88 header
	d88_hdr_t d88_hdr;
	d88_sct_t d88_sct;
	
	memset(&d88_hdr, 0, sizeof(d88_hdr_t));
	strncpy(d88_hdr.title, "IMAGEDISK", sizeof(d88_hdr.title));
	//_strcpy_s(d88_hdr.title, sizeof(d88_hdr.title), "IMAGEDISK");
	d88_hdr.protect = 0; // non-protected
	
	file_size.d = 0;
	COPYBUFFER(&d88_hdr, sizeof(d88_hdr_t));
	
	// create tracks
	int trkcnt = 0, trkptr = sizeof(d88_hdr_t);
	int img_mode = -1;
	uint8 dst[8192];
	
	//int pos;
	//for(pos = 0; pos < size && tmp_buffer[pos] != 0x1a; pos++);
	//pos++;
	
	while(pos < size) {
		// check track header
		uint8 mode = tmp_buffer[pos++];
		uint8 track = tmp_buffer[pos++];
		uint8 head = tmp_buffer[pos++];
		uint8 sector_count = tmp_buffer[pos++];
		uint8 ssize = tmp_buffer[pos++];
		
		if(sector_count == 0) {
			continue;
		}
		if(ssize == 0xff) {
			return false;
		}
		uint32 actual_size = ssize < 7 ? 128 << ssize : 8192;
		
		// setup sector id
		const uint8 *snum = &tmp_buffer[pos];
		pos += sector_count;
		const uint8 *tnum = head & 0x80 ? &tmp_buffer[pos] : NULL;
		if(tnum)
			pos += sector_count;
		const uint8 *hnum = head & 0x40 ? &tmp_buffer[pos] : NULL;
		if(hnum)
			pos += sector_count;
		head &= 0x3f;
		
		// create new track
		int trkside = track * 2 + (head & 1);
		if(trkside < 164) {
			if(trkcnt < trkside) {
				trkcnt = trkside;
			}
			d88_hdr.trkptr[trkside] = trkptr;
		}
		if(img_mode == -1) {
			img_mode = mode & 3;
		}
		
		// read sectors in this track
		for(int i = 0; i < sector_count; i++) {
			// create d88 sector header
			uint8 stype = tmp_buffer[pos++];
			memset(&d88_sct, 0, sizeof(d88_sct_t));
			d88_sct.c = tnum ? tnum[i] : track;
			d88_sct.h = hnum ? hnum[i] : head;
			d88_sct.r = snum[i];
			d88_sct.n = ssize;
			d88_sct.nsec = sector_count;
			d88_sct.dens = (mode < 3) ? 0x40 : 0;
			
			if(stype == 0 || stype > 8) {
				d88_sct.stat = 0xf0; // data mark missing
				d88_sct.size = 0;
			} else {
				d88_sct.del  = (stype == 3 || stype == 4 || stype == 7 || stype == 8) ? 0x10 : 0;
				d88_sct.stat = (stype == 5 || stype == 6 || stype == 7 || stype == 8) ? 0xb0 : d88_sct.del;
				d88_sct.size = actual_size;
				
				// create sector image
				if(stype == 2 || stype == 4 || stype == 6 || stype == 8) {
					memset(dst, tmp_buffer[pos++], actual_size);
				} else {
					memcpy(dst, &tmp_buffer[pos], actual_size);
					pos += d88_sct.size;
				}
			}
			
			// copy to d88
			if(trkside < 164) {
				COPYBUFFER(&d88_sct, sizeof(d88_sct_t));
				COPYBUFFER(dst, d88_sct.size);
				trkptr += sizeof(d88_sct_t) + d88_sct.size;
			}
		}
	}
	d88_hdr.type = (img_mode == 0) ? MEDIA_TYPE_2HD : (((trkcnt + 1) >> 1) > 60) ? MEDIA_TYPE_2DD : MEDIA_TYPE_2D;
	d88_hdr.size = trkptr;
	memcpy(buffer, &d88_hdr, sizeof(d88_hdr_t));
	return true;
}

// cpdread image decoder (from MESS formats/dsk_dsk.c by Mr.Olivier Galibert)

#define DSK_FORMAT_HEADER	"MV - CPC"
#define EXT_FORMAT_HEADER	"EXTENDED CPC DSK"

#pragma pack(1)
struct track_header {
	uint8 headertag[13];
	uint16 unused1;
	uint8 unused1b;
	uint8 track_number;
	uint8 side_number;
	uint8 datarate;
	uint8 rec_mode;
	uint8 sector_size_code;
	uint8 number_of_sector;
	uint8 gap3_length;
	uint8 filler_byte;
};
struct sector_header {
	uint8 track;
	uint8 side;
	uint8 sector_id;
	uint8 sector_size_code;
	uint8 fdc_status_reg1;
	uint8 fdc_status_reg2;
	uint16 data_length;
};
#pragma pack()

bool DISK::cpdread_to_d88(FILEIO *fio)
{
	bool extendformat = false;
	int image_size = fio->FileLength();
	
	fio->Fseek(0, FILEIO_SEEK_SET);
	fio->Fread(tmp_buffer, image_size, 1);
	
	if(memcmp(tmp_buffer, EXT_FORMAT_HEADER, 16) == 0) {
		extendformat = true;
	} else if(memcmp(tmp_buffer, DSK_FORMAT_HEADER, 8) == 0) {
		extendformat = false;
	} else {
		return false;
	}
	
	int heads = tmp_buffer[0x31];
	int skip = 1;
	if(heads == 1) {
		skip = 2;
	}
	int tracks = tmp_buffer[0x30];
	int track_offsets[84 * 2];
	bool track_offsets_error = false;
	if(!extendformat) {
		int cnt = 0, tmp = 0x100;
		for(int i = 0; i < tracks * heads; i++) {
			if(track_offsets_error = (memcmp(tmp_buffer + tmp, "Track-Info", 10) != 0)) {
				break;
			}
			track_offsets[cnt] = tmp;
			tmp += tmp_buffer[0x32] + tmp_buffer[0x33] * 256;
			cnt += skip;
		}
	} else  {
		int cnt = 0, tmp = 0x100;
		for(int i = 0; i < tracks * heads; i++) {
			int length = tmp_buffer[0x34 + i] << 8;
			if(length != 0) {
				if(track_offsets_error = (memcmp(tmp_buffer + tmp, "Track-Info", 10) != 0)) {
					break;
				}
				track_offsets[cnt] = tmp;
				tmp += length;
			} else {
				track_offsets[cnt] = image_size;
			}
			cnt += skip;
		}
	}
	if(track_offsets_error) {
		// I found the dsk image that the track size in table is 1100h, but the actual track size is 900h,
		// so I modified this code to search "Track-Info" at the top of track information block
		int cnt = 0, tmp = 0x100;
		for(int i = 0; i < tracks * heads; i++) {
			bool found = false;
			for(; tmp < image_size; tmp += 0x10) {
				if(found = (memcmp(tmp_buffer + tmp, "Track-Info", 10) == 0)) {
					break;
				}
			}
			if(found) {
				track_offsets[cnt] = tmp;
				tmp += 0x10;
			} else {
				track_offsets[cnt] = image_size;
			}
			cnt += skip;
		}
	}
	
	// create d88 header
	d88_hdr_t d88_hdr;
	d88_sct_t d88_sct;
	
	memset(&d88_hdr, 0, sizeof(d88_hdr_t));
	strncpy(d88_hdr.title, "CPDREAD", sizeof(d88_hdr.title));
	//_strcpy_s(d88_hdr.title, sizeof(d88_hdr.title), "CPDREAD");
	d88_hdr.protect = 0; // non-protected
	
	file_size.d = 0;
	COPYBUFFER(&d88_hdr, sizeof(d88_hdr_t));
	
	// create tracks
	int total = 0, trkptr = sizeof(d88_hdr_t);
	
	for(int track = 0; track < tracks; track++) {
		for(int side = 0; side < heads; side++) {
			if(track_offsets[(track << 1) + side] >= image_size) {
				continue;
			}
			if((track << 1) + side < 164) {
				d88_hdr.trkptr[(track << 1) + side] = trkptr;
			}
			
			track_header tr;
			memcpy(&tr, tmp_buffer + track_offsets[(track << 1) + side], sizeof(tr));
			int pos = track_offsets[(track << 1) + side] + 0x100;
			for(int j = 0; j < tr.number_of_sector; j++) {
				sector_header sector;
				memcpy(&sector, tmp_buffer + track_offsets[(track << 1) + side] + sizeof(tr) + (sizeof(sector) * j), sizeof(sector));
				
				// create d88 sector header
				memset(&d88_sct, 0, sizeof(d88_sct_t));
				d88_sct.c = sector.track;
				d88_sct.h = sector.side;
				d88_sct.r = sector.sector_id;
				d88_sct.n = sector.sector_size_code;
				d88_sct.nsec = tr.number_of_sector;
				if(extendformat) {
					d88_sct.size = sector.data_length;
					d88_sct.dens = (tr.rec_mode == 1) ? 0x40 : 0;
				} else {
					d88_sct.size = 128 << tr.sector_size_code;
					d88_sct.dens = (tr.sector_size_code == 0) ? 0x40 : 0; // FIXME
				}
				d88_sct.del = (sector.fdc_status_reg1 == 0xb2) ? 0x10 : 0;
				d88_sct.stat = (d88_sct.size == 0) ? 0xf0 : (sector.fdc_status_reg1 == 0xb5) ? 0xb0 : d88_sct.del;
				
				// copy to d88
				if((track << 1) + side < 164) {
					COPYBUFFER(&d88_sct, sizeof(d88_sct_t));
					COPYBUFFER(tmp_buffer + pos, d88_sct.size);
					trkptr += sizeof(d88_sct_t) + d88_sct.size;
				}
				total += d88_sct.size;
				
				if(extendformat) {
					pos += sector.data_length;
				} else {
					pos += 128 << tr.sector_size_code;
				}
			}
		}
	}
	d88_hdr.type = (total < (368640 + 655360) / 2) ? MEDIA_TYPE_2D : (total < (737280 + 1228800) / 2) ? MEDIA_TYPE_2DD : MEDIA_TYPE_2HD;
	d88_hdr.size = trkptr;
	memcpy(buffer, &d88_hdr, sizeof(d88_hdr_t));
	return true;
}

// solid image decoder

bool DISK::solid_to_d88(FILEIO *fio, int type, int ncyl, int nside, int nsec, int size, bool mfm)
{
	int n = 0, t = 0;
	
	media_type = type;
	solid_ncyl = ncyl;
	solid_nside = nside;
	solid_nsec = nsec;
	solid_size = size;
	solid_mfm = mfm;
	
	// create d88 header
	d88_hdr_t d88_hdr;
	d88_sct_t d88_sct;
	
	memset(&d88_hdr, 0, sizeof(d88_hdr_t));
	strncpy(d88_hdr.title, "SOLID", sizeof(d88_hdr.title));
	//_strcpy_s(d88_hdr.title, sizeof(d88_hdr.title), "SOLID");
	d88_hdr.protect = 0; // non-protected
	
	file_size.d = 0;
	COPYBUFFER(&d88_hdr, sizeof(d88_hdr_t));
	
	// sector length
	for(int i = 0; i < 8; i++) {
		if(size == (128 << i)) {
			n = i;
			break;
		}
	}
	
	// create tracks
	int trkptr = sizeof(d88_hdr_t);
	
	for(int c = 0; c < ncyl; c++) {
		for(int h = 0; h < nside; h++) {
			d88_hdr.trkptr[t++] = trkptr;
			if(nside == 1) {
				t++;
			}
			
			// read sectors in this track
			for(int s = 0; s < nsec; s++) {
				// create d88 sector header
				memset(&d88_sct, 0, sizeof(d88_sct_t));
				d88_sct.c = c;
				d88_sct.h = h;
				d88_sct.r = s + 1;
				d88_sct.n = n;
				d88_sct.nsec = nsec;
				d88_sct.dens = 0;
				d88_sct.del = 0;
				d88_sct.stat = 0;
				d88_sct.size = size;
				
				// create sector image
				uint8 dst[16384];
				memset(dst, 0xe5, sizeof(dst));
				fio->Fread(dst, size, 1);
				
				// copy to d88
				COPYBUFFER(&d88_sct, sizeof(d88_sct_t));
				COPYBUFFER(dst, size);
				trkptr += sizeof(d88_sct_t) + size;
			}
		}
	}
	d88_hdr.type = (type == MEDIA_TYPE_144) ? MEDIA_TYPE_2HD : type;
	d88_hdr.size = trkptr;
	memcpy(buffer, &d88_hdr, sizeof(d88_hdr_t));
	return true;
}

#define STATE_VERSION	10

void DISK::save_state(FILEIO* state_fio)
{
	state_fio->FputUint32(STATE_VERSION);
	
	state_fio->Fwrite(buffer, sizeof(buffer), 1);
	state_fio->Fwrite(orig_path, sizeof(orig_path), 1);
	state_fio->Fwrite(dest_path, sizeof(dest_path), 1);
	state_fio->FputUint32(file_size.d);
	state_fio->FputInt32(file_bank);
	state_fio->FputUint32(crc32);
	state_fio->FputBool(trim_required);
	state_fio->FputBool(is_1dd_image);
	state_fio->FputBool(is_solid_image);
	state_fio->FputBool(is_fdi_image);
	state_fio->Fwrite(fdi_header, sizeof(fdi_header), 1);
	state_fio->FputInt32(solid_ncyl);
	state_fio->FputInt32(solid_nside);
	state_fio->FputInt32(solid_nsec);
	state_fio->FputInt32(solid_size);
	state_fio->FputBool(solid_mfm);
	state_fio->FputBool(inserted);
	state_fio->FputBool(ejected);
	state_fio->FputBool(write_protected);
	state_fio->FputBool(changed);
	state_fio->FputUint8(media_type);
	state_fio->FputInt32(is_special_disk);
	state_fio->Fwrite(track, sizeof(track), 1);
	state_fio->FputInt32(sector_num.sd);
	state_fio->FputBool(track_mfm);
	state_fio->FputBool(invalid_format);
//	state_fio->FputBool(no_skew);
	state_fio->Fwrite(sync_position, sizeof(sync_position), 1);
	state_fio->Fwrite(am1_position, sizeof(am1_position), 1);
	state_fio->Fwrite(id_position, sizeof(id_position), 1);
	state_fio->Fwrite(data_position, sizeof(data_position), 1);
	state_fio->FputInt32(sector ? (int)(sector - buffer) : -1);
	state_fio->FputInt32(sector_size.sd);
	state_fio->Fwrite(id, sizeof(id), 1);
	state_fio->FputUint8(density);
	state_fio->FputBool(deleted);
	state_fio->FputBool(addr_crc_error);
	state_fio->FputBool(data_crc_error);
	state_fio->FputUint8(drive_type);
	state_fio->FputInt32(drive_rpm);
	state_fio->FputBool(drive_mfm);
}

bool DISK::load_state(FILEIO* state_fio)
{
	if(state_fio->FgetUint32() != STATE_VERSION) {
		return false;
	}
	state_fio->Fread(buffer, sizeof(buffer), 1);
	state_fio->Fread(orig_path, sizeof(orig_path), 1);
	state_fio->Fread(dest_path, sizeof(dest_path), 1);
	file_size.d = state_fio->FgetUint32();
	file_bank = state_fio->FgetInt32();
	crc32 = state_fio->FgetUint32();
	trim_required = state_fio->FgetBool();
	is_1dd_image = state_fio->FgetBool();
	is_solid_image = state_fio->FgetBool();
	is_fdi_image = state_fio->FgetBool();
	state_fio->Fread(fdi_header, sizeof(fdi_header), 1);
	solid_ncyl = state_fio->FgetInt32();
	solid_nside = state_fio->FgetInt32();
	solid_nsec = state_fio->FgetInt32();
	solid_size = state_fio->FgetInt32();
	solid_mfm = state_fio->FgetBool();
	inserted = state_fio->FgetBool();
	ejected = state_fio->FgetBool();
	write_protected = state_fio->FgetBool();
	changed = state_fio->FgetBool();
	media_type = state_fio->FgetUint8();
	is_special_disk = state_fio->FgetInt32();
	state_fio->Fread(track, sizeof(track), 1);
	sector_num.sd = state_fio->FgetInt32();
	track_mfm = state_fio->FgetBool();
	invalid_format = state_fio->FgetBool();
//	no_skew = state_fio->FgetBool();
	state_fio->Fread(sync_position, sizeof(sync_position), 1);
	state_fio->Fread(am1_position, sizeof(am1_position), 1);
	state_fio->Fread(id_position, sizeof(id_position), 1);
	state_fio->Fread(data_position, sizeof(data_position), 1);
	int offset = state_fio->FgetInt32();
	sector = (offset != -1) ? buffer + offset : NULL;
	sector_size.sd = state_fio->FgetInt32();
	state_fio->Fread(id, sizeof(id), 1);
	density = state_fio->FgetUint8();
	deleted = state_fio->FgetBool();
	addr_crc_error = state_fio->FgetBool();
	data_crc_error = state_fio->FgetBool();
	drive_type = state_fio->FgetUint8();
	drive_rpm = state_fio->FgetInt32();
	drive_mfm = state_fio->FgetBool();
	return true;
}

