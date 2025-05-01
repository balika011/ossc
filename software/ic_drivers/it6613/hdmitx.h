#ifndef _HDMITX_H_
#define _HDMITX_H_

#ifdef EXTERN_HDCPROM
#pragma message("Defined EXTERN_HDCPROM")
#endif // EXTERN_HDCPROM

#define SUPPORT_EDID
//#define SUPPORT_HDCP
#define SUPPORT_INPUTRGB
//#define SUPPORT_INPUTYUV444
//#define SUPPORT_INPUTYUV422
//#define SUPPORT_SYNCEMBEDDED
//#define SUPPORT_DEGEN
//#define SUPPORT_INPUTYUV
//#define INVERT_VID_LATCHEDGE //latch at falling edge


#ifdef SUPPORT_SYNCEMBEDDED
#pragma message("defined SUPPORT_SYNCEMBEDDED for Sync Embedded timing input or CCIR656 input.") 
#endif

#ifndef _MCU_ // DSSSHA need large computation data rather than 8051 supported.
#define SUPPORT_DSSSHA
#endif

#if defined(SUPPORT_INPUTYUV444) || defined(SUPPORT_INPUTYUV422)
#define SUPPORT_INPUTYUV
#endif

#include "it6613_types.h"
#include "HDMI_COMMON.h"
#include "sysconfig.h"
#include "it6613_drv.h"

// Hardwired to CPU reset
#define HDMITX_Reset(x)

#ifndef SUPPORT_HDCP
static SYS_STATUS
HDCP_Authenticate()
{
	return ER_SUCCESS;
}

static void
HDCP_ResetAuth()
{
	return;
}

static void
HDCP_ResumeAuthentication()
{
	return;
}
#endif

#define HDMITX_INSTANCE_MAX 1

#define SIZEOF_CSCMTX 18
#define SIZEOF_CSCGAIN 6
#define SIZEOF_CSCOFFSET 3

///////////////////////////////////////////////////////////////////////
// Output Mode Type
///////////////////////////////////////////////////////////////////////

#define RES_ASPEC_4x3 0
#define RES_ASPEC_16x9 1
#define F_MODE_REPT_NO 0
#define F_MODE_REPT_TWICE 1
#define F_MODE_REPT_QUATRO 3
#define F_MODE_CSC_ITU601 0
#define F_MODE_CSC_ITU709 1

///////////////////////////////////////////////////////////////////////
// ROM OFFSET
///////////////////////////////////////////////////////////////////////
#define ROMOFF_INT_TYPE 0
#define ROMOFF_INPUT_VIDEO_TYPE 1
#define ROMOFF_OUTPUT_AUDIO_MODE 8
#define ROMOFF_AUDIO_CH_SWAP 9



#define TIMER_LOOP_LEN 10
#define MS(x) (((x)+(TIMER_LOOP_LEN-1))/TIMER_LOOP_LEN) ; // for timer loop



#endif // _HDMITX_H_

