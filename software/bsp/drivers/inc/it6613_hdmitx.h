#ifndef _HDMITX_H_
#define _HDMITX_H_

#include "it6613_config.h"
#include "it6613_types.h"
#include "it6613_HDMI_COMMON.h"
#include "it6613_drv.h"

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
#define MS(x) (((x) + (TIMER_LOOP_LEN - 1)) / TIMER_LOOP_LEN)

#endif // _HDMITX_H_
