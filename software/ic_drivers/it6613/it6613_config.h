#ifndef _IT6613_CONFIG_H
#define _IT6613_CONFIG_H

// #define IT6613_DEBUG

#ifdef EXTERN_HDCPROM
#pragma message("Defined EXTERN_HDCPROM")
#endif // EXTERN_HDCPROM

#define SUPPORT_EDID
// #define SUPPORT_HDCP
#define SUPPORT_INPUTRGB
// #define SUPPORT_INPUTYUV444
// #define SUPPORT_INPUTYUV422
// #define SUPPORT_SYNCEMBEDDED
// #define SUPPORT_DEGEN
// #define SUPPORT_INPUTYUV
// #define INVERT_VID_LATCHEDGE //latch at falling edge

#ifdef SUPPORT_SYNCEMBEDDED
#pragma message("defined SUPPORT_SYNCEMBEDDED for Sync Embedded timing input or CCIR656 input.")
#endif

#ifndef _MCU_ // DSSSHA need large computation data rather than 8051 supported.
#define SUPPORT_DSSSHA
#endif

#if defined(SUPPORT_INPUTYUV444) || defined(SUPPORT_INPUTYUV422)
#define SUPPORT_INPUTYUV
#endif

#ifndef IT6613_DEBUG
#define OS_PRINTF(...)
#define ErrorF(...)
#define printf(...)
#else
#include <stdio.h>
#include "utils.h"
#define OS_PRINTF printf
#define ErrorF printf
#define printf dd_printf
#endif

#endif