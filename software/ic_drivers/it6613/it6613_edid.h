#ifndef _EDID_H_
#define _EDID_H_

#include "it6613_hdmitx.h"

#ifdef SUPPORT_EDID

/////////////////////////////////////////
// RX Capability.
/////////////////////////////////////////
typedef struct {
    uint8_t b16bit:1 ;
    uint8_t b20bit:1 ;
    uint8_t b24bit:1 ;
    uint8_t Rsrv:5 ;
} LPCM_BitWidth ;

typedef enum {
    AUD_RESERVED_0 = 0 ,
    AUD_LPCM,
    AUD_AC3,
    AUD_MPEG1,
    AUD_MP3,
    AUD_MPEG2,
    AUD_AAC,
    AUD_DTS,
    AUD_ATRAC,
    AUD_ONE_BIT_AUDIO,
    AUD_DOLBY_DIGITAL_PLUS,
    AUD_DTS_HD,
    AUD_MAT_MLP,
    AUD_DST,
    AUD_WMA_PRO,
    AUD_RESERVED_15
} AUDIO_FORMAT_CODE ;

typedef union {
    struct {
        uint8_t channel:3 ;
        uint8_t AudioFormatCode:4 ;
        uint8_t Rsrv1:1 ;

        uint8_t b32KHz:1 ;
        uint8_t b44_1KHz:1 ;
        uint8_t b48KHz:1 ;
        uint8_t b88_2KHz:1 ;
        uint8_t b96KHz:1 ;
        uint8_t b176_4KHz:1 ;
        uint8_t b192KHz:1 ;
        uint8_t Rsrv2:1 ;
        uint8_t ucCode ;
    } s ;
    uint8_t uc[3] ;

} AUDDESCRIPTOR ;

typedef union {
    struct {
        uint8_t FL_FR:1 ;
        uint8_t LFE:1 ;
        uint8_t FC:1 ;
        uint8_t RL_RR:1 ;
        uint8_t RC:1 ;
        uint8_t FLC_FRC:1 ;
        uint8_t RLC_RRC:1 ;
        uint8_t Reserve:1 ;
        uint8_t Unuse[2] ;
    } s ;
    uint8_t uc[3] ;
} SPK_ALLOC ;

#define CEA_SUPPORT_UNDERSCAN (1<<7)
#define CEA_SUPPORT_AUDIO (1<<6)
#define CEA_SUPPORT_YUV444 (1<<5)
#define CEA_SUPPORT_YUV422 (1<<4)
#define CEA_NATIVE_MASK 0xF

typedef union _tag_DCSUPPORT {
    struct {
        uint8_t DVI_Dual:1 ;
        uint8_t Rsvd:2 ;
        uint8_t DC_Y444:1 ;
        uint8_t DC_30Bit:1 ;    
        uint8_t DC_36Bit:1 ;    
        uint8_t DC_48Bit:1 ;    
        uint8_t SUPPORT_AI:1 ;    
    } info ;
    uint8_t uc ;
} DCSUPPORT ;

typedef union _LATENCY_SUPPORT{
    struct {
        uint8_t Rsvd:6 ;
        uint8_t I_Latency_Present:1 ;
        uint8_t Latency_Present:1 ;
    } info ;
    uint8_t uc ;
} LATENCY_SUPPORT ;

#define HDMI_IEEEOUI 0x0c03

typedef struct _RX_CAP{
    uint8_t Valid;
    uint8_t VideoMode ;
    uint8_t VDOModeCount ;
    uint8_t idxNativeVDOMode ;
    uint8_t VDOMode[128] ;
    uint8_t AUDDesCount ;
    AUDDESCRIPTOR AUDDes[32] ;
    uint32_t  IEEEOUI ;
    DCSUPPORT dc ;
    uint8_t MaxTMDSClock ;
    LATENCY_SUPPORT lsupport ;
    uint8_t V_Latency ;
    uint8_t A_Latency ;
    uint8_t V_I_Latency ;
    uint8_t A_I_Latency ;
    SPK_ALLOC   SpeakerAllocBlk ;
    uint8_t ValidCEA:1 ;
    uint8_t ValidHDMI:1 ;
} RX_CAP ;

SYS_STATUS ParseVESAEDID(uint8_t *pEDID) ;
SYS_STATUS ParseCEAEDID(uint8_t *pCEAEDID, RX_CAP *pRxCap) ;



#endif // SUPPORT_EDID
#endif // _EDID_H_
