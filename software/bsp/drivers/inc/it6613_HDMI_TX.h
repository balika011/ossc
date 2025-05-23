#ifndef HDMI_TX_H_
#define HDMI_TX_H_

#include "it6613_HDMI_COMMON.h"
#include "it6613_hdmitx.h"

bool HDMITX_Init(void);
bool HDMITX_ChipVerify(void);
bool HDMITX_HPD(void);
void HDMITX_ChangeVideoTiming(int VIC);
void HDMITX_ChangeVideoTimingAndColor(int VIC, COLOR_TYPE Color);
void HDMITX_SetAVIInfoFrame(uint8_t VIC, uint8_t OutputColorMode, bool b16x9, bool ITU709, bool ITC, uint8_t pixelrep);

void HDMITX_DisableVideoOutput(void);
void HDMITX_EnableVideoOutput(void);
void HDMITX_SetColorSpace(COLOR_TYPE InputColor, COLOR_TYPE OutputColor);
bool HDMITX_DevLoopProc(void);

bool HDMITX_IsSinkSupportYUV444(void);
bool HDMITX_IsSinkSupportYUV422(void);

bool HDMITX_IsSinkSupportColorDepth36(void);
bool HDMITX_IsSinkSupportColorDepth30(void);
void HDMITX_SetOutputColorDepth(int ColorDepth);

void HDMITX_SetAudioInfoFrame(uint8_t bAudioDwSampling);
void HDMITX_SetHDRInfoFrame(uint8_t enableHDR);
void HDMITX_SetVRRInfoFrame(uint8_t enable);

#endif /*HDMI_TX_H_*/
