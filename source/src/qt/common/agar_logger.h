/*
 * Log functions
 * (C) 2014-06-30 K.Ohta
 * 
 * History:
 *  Dec 30, 2014 Move from XM7/SDL, this was Ohta's original code.
 * Licence : GPLv2
 */
#ifndef _CSP_LOGGER_H
#define _CSP_LOGGER_H

#include <stdarg.h>
#include <stdio.h>
#include <QWidget>
#if !defined(Q_OS_WIN32)
#  include <syslog.h>
#endif
#include <time.h>
#include <sys/time.h>

#include "simd_types.h"

#ifdef __cplusplus
extern "C" {
#endif
   extern void AGAR_OpenLog(int syslog, int cons);
   extern void AGAR_DebugLog(int level, const char *fmt, ...);
   extern void AGAR_CloseLog(void);
   extern void AGAR_SetLogStatus(int sw);
   extern void AGAR_SetLogSysLog(int sw);
   extern void AGAR_SetLogStdOut(int sw);
   extern bool AGAR_LogGetStatus(void);

#define AGAR_LOG_ON 1
#define AGAR_LOG_OFF 0
   
#define AGAR_LOG_DEBUG 0
#define AGAR_LOG_INFO 1
#define AGAR_LOG_WARN 2

   
#ifndef FALSE
#define FALSE                   0
#endif
#ifndef TRUE
#define TRUE                    (!FALSE)
#endif

#ifdef __cplusplus
}
#endif
 
#endif