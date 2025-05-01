#ifndef _IT6613_TYPES_H_
#define _IT6613_TYPES_H_

#include <stdint.h>
#include <stdbool.h>

//////////////////////////////////////////////////
// data type
//////////////////////////////////////////////////

#define SUCCESS 0
#define FAIL -1

#define ON 1
#define OFF 0

typedef enum _SYS_STATUS
{
	ER_SUCCESS = 0,
	ER_FAIL,
	ER_RESERVED
} SYS_STATUS;

#define abs(x) (((x) >= 0) ? (x) : (-(x)))

typedef enum _Video_State_Type
{
	VSTATE_PwrOff = 0,
	VSTATE_SyncWait,
	VSTATE_SWReset,
	VSTATE_SyncChecking,
	VSTATE_HDCPSet,
	VSTATE_HDCP_Reset,
	VSTATE_ModeDetecting,
	VSTATE_VideoOn,
	VSTATE_Reserved
} Video_State_Type;

typedef enum _Audio_State_Type
{
	ASTATE_AudioOff = 0,
	ASTATE_RequestAudio,
	ASTATE_ResetAudio,
	ASTATE_WaitForReady,
	ASTATE_AudioOn,
	ASTATE_Reserved
} Audio_State_Type;

typedef enum _TXVideo_State_Type
{
	TXVSTATE_Unplug = 0,
	TXVSTATE_HPD,
	TXVSTATE_WaitForMode,
	TXVSTATE_WaitForVStable,
	TXVSTATE_VideoInit,
	TXVSTATE_VideoSetup,
	TXVSTATE_VideoOn,
	TXVSTATE_Reserved
} TXVideo_State_Type;

typedef enum _TXAudio_State_Type
{
	TXASTATE_AudioOff = 0,
	TXASTATE_AudioPrepare,
	TXASTATE_AudioOn,
	TXASTATE_AudioFIFOFail,
	TXASTATE_Reserved
} TXAudio_State_Type;

typedef enum
{
	PCLK_LOW = 0,
	PCLK_MEDIUM,
	PCLK_HIGH
} VIDEOPCLKLEVEL;

///////////////////////////////////////////////////////////////////////
// Video Data Type
///////////////////////////////////////////////////////////////////////
#define F_MODE_RGB24 0
#define F_MODE_RGB444 0
#define F_MODE_YUV422 1
#define F_MODE_YUV444 2
#define F_MODE_CLRMOD_MASK 3

#define F_MODE_INTERLACE 1

#define F_MODE_ITU709 (1 << 4)
#define F_MODE_ITU601 0

#define F_MODE_0_255 0
#define F_MODE_16_235 (1 << 5)

#define F_MODE_EN_UDFILT (1 << 6) // output mode only,and loaded from EEPROM
#define F_MODE_EN_DITHER (1 << 7) // output mode only,and loaded from EEPROM

#define F_VIDMODE_ITU709 F_MODE_ITU709
#define F_VIDMODE_16_235 F_MODE_16_235

typedef union _VideoFormatCode
{
	struct _VFC
	{
		uint8_t colorfmt : 2;
		uint8_t interlace : 1;
		uint8_t Colorimetry : 1;
		uint8_t Quantization : 1;
		uint8_t UpDownFilter : 1;
		uint8_t Dither : 1;
	} VFCCode;
	unsigned char VFCByte;
} VideoFormatCode;

#define T_MODE_CCIR656 (1 << 0)
#define T_MODE_SYNCEMB (1 << 1)
#define T_MODE_INDDR (1 << 2)
#define T_MODE_PCLKDIV2 (1 << 3)
#define T_MODE_DEGEN (1 << 4)
#define T_MODE_SYNCGEN (1 << 5)
//////////////////////////////////////////////////////////////////
// Audio relate definition and macro.
//////////////////////////////////////////////////////////////////

// for sample clock
#define AUDFS_22p05KHz 4
#define AUDFS_44p1KHz 0
#define AUDFS_88p2KHz 8
#define AUDFS_176p4KHz 12

#define AUDFS_24KHz 6
#define AUDFS_48KHz 2
#define AUDFS_96KHz 10
#define AUDFS_192KHz 14

#define AUDFS_32KHz 3
#define AUDFS_OTHER 1

// Audio Enable
#define ENABLE_SPDIF (1 << 4)
#define ENABLE_I2S_SRC3 (1 << 3)
#define ENABLE_I2S_SRC2 (1 << 2)
#define ENABLE_I2S_SRC1 (1 << 1)
#define ENABLE_I2S_SRC0 (1 << 0)

#define AUD_SWL_NOINDICATE 0x0
#define AUD_SWL_16 0x2
#define AUD_SWL_17 0xC
#define AUD_SWL_18 0x4
#define AUD_SWL_20 0xA // for maximum 20 bit
#define AUD_SWL_21 0xD
#define AUD_SWL_22 0x5
#define AUD_SWL_23 0x9
#define AUD_SWL_24 0xB

/////////////////////////////////////////////////////////////////////
// Packet and Info Frame definition and datastructure.
/////////////////////////////////////////////////////////////////////

#define VENDORSPEC_INFOFRAME_TYPE 0x01
#define AVI_INFOFRAME_TYPE 0x02
#define SPD_INFOFRAME_TYPE 0x03
#define AUDIO_INFOFRAME_TYPE 0x04
#define MPEG_INFOFRAME_TYPE 0x05
#define HDR_INFOFRAME_TYPE 0x07

#define VENDORSPEC_INFOFRAME_VER 0x01
#define AVI_INFOFRAME_VER 0x02
#define SPD_INFOFRAME_VER 0x01
#define AUDIO_INFOFRAME_VER 0x01
#define MPEG_INFOFRAME_VER 0x01
#define HDR_INFOFRAME_VER 0x01

#define VENDORSPEC_INFOFRAME_LEN 8
#define AVI_INFOFRAME_LEN 13
#define SPD_INFOFRAME_LEN 25
#define AUDIO_INFOFRAME_LEN 10
#define MPEG_INFOFRAME_LEN 10
#define HDR_INFOFRAME_LEN 26

#define ACP_PKT_LEN 9
#define ISRC1_PKT_LEN 16
#define ISRC2_PKT_LEN 16

typedef union _AVI_InfoFrame
{
	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		uint8_t Scan : 2;
		uint8_t BarInfo : 2;
		uint8_t ActiveFmtInfoPresent : 1;
		uint8_t ColorMode : 2;
		uint8_t FU1 : 1;

		uint8_t ActiveFormatAspectRatio : 4;
		uint8_t PictureAspectRatio : 2;
		uint8_t Colorimetry : 2;

		uint8_t Scaling : 2;
		uint8_t FU2 : 6;

		uint8_t VIC : 7;
		uint8_t FU3 : 1;

		uint8_t PixelRepetition : 4;
		uint8_t FU4 : 4;

		uint16_t Ln_End_Top;
		uint16_t Ln_Start_Bottom;
		uint16_t Pix_End_Left;
		uint16_t Pix_Start_Right;
	} info;
	struct
	{
		uint8_t AVI_HB[3];
		uint8_t AVI_DB[AVI_INFOFRAME_LEN];
	} pktbyte;
} AVI_InfoFrame;

typedef union _Freesync_InfoFrame
{

	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		uint8_t Enable;

		uint8_t Rsvd1;
		uint8_t Rsvd2;
		uint8_t Rsvd3;
		uint8_t Rsvd4;

		uint8_t Mode2;

		uint8_t min_rate;
		uint8_t max_rate;
	} info;

	struct
	{
		uint8_t FS_HB[3];
		uint8_t FS_DB[VENDORSPEC_INFOFRAME_LEN];
	} pktbyte;

} Freesync_InfoFrame;

typedef union _Audio_InfoFrame
{

	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		uint8_t AudioChannelCount : 3;
		uint8_t RSVD1 : 1;
		uint8_t AudioCodingType : 4;

		uint8_t SampleSize : 2;
		uint8_t SampleFreq : 3;
		uint8_t Rsvd2 : 3;

		uint8_t FmtCoding;

		uint8_t SpeakerPlacement;

		uint8_t Rsvd3 : 3;
		uint8_t LevelShiftValue : 4;
		uint8_t DM_INH : 1;
	} info;

	struct
	{
		uint8_t AUD_HB[3];
		uint8_t AUD_DB[AUDIO_INFOFRAME_LEN];
	} pktbyte;

} Audio_InfoFrame;

typedef union _MPEG_InfoFrame
{
	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		uint32_t MpegBitRate;

		uint8_t MpegFrame : 2;
		uint8_t Rvsd1 : 2;
		uint8_t FieldRepeat : 1;
		uint8_t Rvsd2 : 3;
	} info;
	struct
	{
		uint8_t MPG_HB[3];
		uint8_t MPG_DB[MPEG_INFOFRAME_LEN];
	} pktbyte;
} MPEG_InfoFrame;

// Source Product Description
typedef union _SPD_InfoFrame
{
	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		char VN[8];	 // vendor name character in 7bit ascii characters
		char PD[16]; // product description character in 7bit ascii characters
		uint8_t SourceDeviceInfomation;
	} info;
	struct
	{
		uint8_t SPD_HB[3];
		uint8_t SPD_DB[SPD_INFOFRAME_LEN];
	} pktbyte;
} SPD_InfoFrame;

// HDR
typedef union _HDR_InfoFrame
{
	struct
	{
		uint8_t Type;
		uint8_t Ver;
		uint8_t Len;

		uint8_t TF;
		uint8_t DESC_ID;
		uint8_t DESC[24];
	} info;
	struct
	{
		uint8_t HDR_HB[3];
		uint8_t HDR_DB[HDR_INFOFRAME_LEN];
	} pktbyte;
} HDR_InfoFrame;

///////////////////////////////////////////////////////////////////////////
// Using for interface.
///////////////////////////////////////////////////////////////////////////
struct VideoTiming
{
	uint32_t VideoPixelClock;
	uint8_t VIC;
	uint8_t pixelrep;
	uint8_t outputVideoMode;
};

#endif // _TYPEDEF_H_
