
#ifndef _CSP_EMU_UTILS_H
#define _CSP_EMU_UTILS_H
#include "common.h"
#include "emu.h"
#include "qt_main.h"

#if !defined(Q_OS_WIN32)
extern "C" {
	extern uint32_t timeGetTime(void);
}
#endif

#if defined(USE_FD1) || defined(USE_FD2) || defined(USE_FD3) || defined(USE_FD4) || \
    defined(USE_FD5) || defined(USE_FD6) || defined(USE_FD7) || defined(USE_FD8)

extern void open_disk(int drv, _TCHAR* path, int bank);
extern void close_disk(int drv);
#endif

#endif
