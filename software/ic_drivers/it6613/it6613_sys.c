///////////////////////////////////////////////////////////////////////////////
// This is the sample program for CAT6611 driver usage.
///////////////////////////////////////////////////////////////////////////////

#include "it6613_hdmitx.h"
#include "it6613_sys.h"
#include "it6613_edid.h"

extern int TX_HDP;
int gEnableColorDepth = 1;

////////////////////////////////////////////////////////////////////////////////
// EDID
////////////////////////////////////////////////////////////////////////////////
static unsigned char EDID_Buf[128] ;
RX_CAP RxCapability ;
int bChangeMode = false ;
AVI_InfoFrame AviInfo;
Audio_InfoFrame AudioInfo ;

////////////////////////////////////////////////////////////////////////////////
// Program utility.
////////////////////////////////////////////////////////////////////////////////
void ConfigAVIInfoFrame(uint8_t VIC, uint8_t pixelrep) ;
void ConfigAudioInfoFrm() ;


#ifndef SUPPORT_SYNCEMB
uint8_t bInputColorMode = F_MODE_RGB444; //F_MODE_RGB444;
uint8_t bInputSignalType = 0 ;
#else
uint8_t bInputColorMode = F_MODE_YUV422 ;
uint8_t bInputSignalType = T_MODE_SYNCEMB | T_MODE_CCIR656 ; // for 16 bit sync embedded
#endif // SUPPORT_SYNCEMB

uint8_t iVideoModeSelect=0 ;

uint8_t bOutputColorMode = F_MODE_RGB444; //F_MODE_RGB444 ;
uint32_t  VideoPixelClock ; 
uint8_t VIC ; // 480p60
uint8_t pixelrep ; // no pixelrepeating
HDMI_Aspec aspec ;
HDMI_Colorimetry Colorimetry ;

int bHDMIMode, bAudioEnable ;

////////////////////////////////////////////////////////////////////////////////
// Function Body.
////////////////////////////////////////////////////////////////////////////////

void
HDMITX_SetOutput()
{
    VIDEOPCLKLEVEL level ;
    unsigned long TMDSClock = VideoPixelClock*(pixelrep+1);

    #ifdef SUPPORT_SYNCEMB
    ProgramSyncEmbeddedVideoMode(VIC, bInputSignalType) ; // inf CCIR656 input
    #endif
    
    if( TMDSClock>80000000 )
    {
        level = PCLK_HIGH ;
    }
    else if(TMDSClock>20000000)
    {
        level = PCLK_MEDIUM ;
    }
    else
    {
        level = PCLK_LOW ;
    }
    


    //int EnableVideoOutput(VIDEOPCLKLEVEL level,uint8_t inputColorMode,uint8_t outputColorMode,uint8_t bHDMI) ;
    //EnableVideoOutput(level,bInputColorMode, bInputSignalType, bOutputColorMode,bHDMIMode) ;
    EnableVideoOutput(level,bInputColorMode, bOutputColorMode,bHDMIMode) ;
    
    if( bHDMIMode )
    {
    	OS_PRINTF("ConfigAVIInfoFrame, VIC=%d\n", VIC);
        ConfigAVIInfoFrame(VIC, pixelrep) ;

        EnableHDCP(true) ;
		if( bAudioEnable )
		{
            //int EnableAudioOutput(uint32_t  VideoPixelClock,uint8_t bAudioSampleFreq,uint8_t ChannelNumber,uint8_t bAudSWL,uint8_t bSPDIF)
            //EnableAudioOutput(TMDSClock,48000, 2, false);
            bool bSPDIF = false;
            EnableAudioOutput(TMDSClock,AUDFS_48KHz, 2, 16, bSPDIF);
            ConfigAudioInfoFrm() ;
		}
    }
    SetAVMute(false) ;
    bChangeMode = false ;
}



void
HDMITX_ChangeDisplayOption(HDMI_Video_Type OutputVideoTiming, HDMI_OutputColorMode OutputColorMode)
{
   //HDMI_Video_Type  t=HDMI_480i60_16x9;
    switch(OutputVideoTiming)
	{
    case HDMI_640x480p60:
        VIC = 1 ;
        VideoPixelClock = 25000000 ;
        pixelrep = 0 ;
        aspec = HDMI_4x3 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_480p60:
        VIC = 2 ;
        VideoPixelClock = 27000000 ;
        pixelrep = 0 ;
        aspec = HDMI_4x3 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_480p60_16x9:
        VIC = 3 ;
        VideoPixelClock = 27000000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_720p60:
        VIC = 4 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_1080i60:
        VIC = 5 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_480i60:
        VIC = 6 ;
        VideoPixelClock = 13500000 ;
        pixelrep = 1 ;
        aspec = HDMI_4x3 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_480i60_16x9:
        VIC = 7 ;
        VideoPixelClock = 13500000 ;
        pixelrep = 1 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_1080p60:
        VIC = 16 ;
        VideoPixelClock = 148500000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_576p50:
        VIC = 17 ;
        VideoPixelClock = 27000000 ;
        pixelrep = 0 ;
        aspec = HDMI_4x3 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_576p50_16x9:
        VIC = 18 ;
        VideoPixelClock = 27000000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_720p50:
        VIC = 19 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_1080i50:
        VIC = 20 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_576i50:
        VIC = 21 ;
        VideoPixelClock = 13500000 ;
        pixelrep = 1 ;
        aspec = HDMI_4x3 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_576i50_16x9:
        VIC = 22 ;
        VideoPixelClock = 13500000 ;
        pixelrep = 1 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU601 ;
        break ;
    case HDMI_1080p50:
        VIC = 31 ;
        VideoPixelClock = 148500000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
/*    case HDMI_1080p24:
        VIC = 32 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_1080p25:
        VIC = 33 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_1080p30:
        VIC = 34 ;
        VideoPixelClock = 74250000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU709 ;
        break ;
    case HDMI_1080i120:
        VIC = 46 ;
        VideoPixelClock = 148500000 ;
        pixelrep = 0 ;
        aspec = HDMI_16x9 ;
        Colorimetry = HDMI_ITU601 ;
        break ;*/
        
        
    default:
        VIC = 0;
        bChangeMode = false ;                
        return ;
    }

    switch(OutputColorMode)
    {
    case HDMI_YUV444:
        bOutputColorMode = F_MODE_YUV444 ;
        break ;
    case HDMI_YUV422:
        bOutputColorMode = F_MODE_YUV422 ;
        break ;
    case HDMI_RGB444:
    default:
        bOutputColorMode = F_MODE_RGB444 ;
        break ;
    }

    if( Colorimetry == HDMI_ITU709 )
    {
        bInputColorMode |= F_VIDMODE_ITU709 ;
    }
    else
    {
        bInputColorMode &= ~F_VIDMODE_ITU709 ;
    }
    
    if( OutputVideoTiming != HDMI_640x480p60)
    {
        bInputColorMode |= F_VIDMODE_16_235 ;
    }
    else
    {
        bInputColorMode &= ~F_VIDMODE_16_235 ;
    }

    bChangeMode = true ;
}


void
ConfigAVIInfoFrame(uint8_t VIC, uint8_t pixelrep)
{
//     AVI_InfoFrame AviInfo;

    AviInfo.pktbyte.AVI_HB[0] = AVI_INFOFRAME_TYPE|0x80 ; 
    AviInfo.pktbyte.AVI_HB[1] = AVI_INFOFRAME_VER ; 
    AviInfo.pktbyte.AVI_HB[2] = AVI_INFOFRAME_LEN ; 
    
    switch(bOutputColorMode)
    {
    case F_MODE_YUV444:
        // AviInfo.info.ColorMode = 2 ;
        AviInfo.pktbyte.AVI_DB[0] = (2<<5)|(1<<4) ;
        break ;
    case F_MODE_YUV422:
        // AviInfo.info.ColorMode = 1 ;
        AviInfo.pktbyte.AVI_DB[0] = (1<<5)|(1<<4) ;
        break ;
    case F_MODE_RGB444:
    default:
        // AviInfo.info.ColorMode = 0 ;
        AviInfo.pktbyte.AVI_DB[0] = (0<<5)|(1<<4) ;
        break ;
    }
    AviInfo.pktbyte.AVI_DB[1] = 8 ;
    AviInfo.pktbyte.AVI_DB[1] |= (aspec != HDMI_16x9)?(1<<4):(2<<4) ; // 4:3 or 16:9
    AviInfo.pktbyte.AVI_DB[1] |= (Colorimetry != HDMI_ITU709)?(1<<6):(2<<6) ; // 4:3 or 16:9
    AviInfo.pktbyte.AVI_DB[2] = 0 ;
    AviInfo.pktbyte.AVI_DB[3] = VIC ;
    AviInfo.pktbyte.AVI_DB[4] =  pixelrep & 3 ;
    AviInfo.pktbyte.AVI_DB[5] = 0 ;
    AviInfo.pktbyte.AVI_DB[6] = 0 ;
    AviInfo.pktbyte.AVI_DB[7] = 0 ;
    AviInfo.pktbyte.AVI_DB[8] = 0 ;
    AviInfo.pktbyte.AVI_DB[9] = 0 ;
    AviInfo.pktbyte.AVI_DB[10] = 0 ;
    AviInfo.pktbyte.AVI_DB[11] = 0 ;
    AviInfo.pktbyte.AVI_DB[12] = 0 ;

    EnableAVIInfoFrame(true, (unsigned char *)&AviInfo) ;
}



////////////////////////////////////////////////////////////////////////////////
// Function: ConfigAudioInfoFrm
// Parameter: NumChannel, number from 1 to 8
// Return: ER_SUCCESS for successfull.
// Remark: Evaluate. The speakerplacement is only for reference.
//         For production, the caller of SetAudioInfoFrame should program
//         Speaker placement by actual status.
// Side-Effect:
////////////////////////////////////////////////////////////////////////////////

void
ConfigAudioInfoFrm()
{
    int i ;
    ErrorF("ConfigAudioInfoFrm(%d)\n",2) ;

    AudioInfo.pktbyte.AUD_HB[0] = AUDIO_INFOFRAME_TYPE ;
    AudioInfo.pktbyte.AUD_HB[1] = 1 ;
    AudioInfo.pktbyte.AUD_HB[2] = AUDIO_INFOFRAME_LEN ;
    AudioInfo.pktbyte.AUD_DB[0] = 1 ;
    for( i = 1 ;i < AUDIO_INFOFRAME_LEN ; i++ )
    {
        AudioInfo.pktbyte.AUD_DB[i] = 0 ;
    }
    EnableAudioInfoFrame(true, (unsigned char *)&AudioInfo) ;
}



/////////////////////////////////////////////////////////////////////
// ParseEDID()
// Check EDID check sum and EDID 1.3 extended segment.
/////////////////////////////////////////////////////////////////////

int
ParseEDID()
{
    // collect the EDID ucdata of segment 0
    uint8_t CheckSum ;
    uint8_t BlockCount ;
    int err = false ;
    int bValidCEA = false ;
    int i ;

    RxCapability.ValidCEA = false ;
	
    if (!GetEDIDData(0, EDID_Buf))
        return false;


    for( i = 0, CheckSum = 0 ; i < 128 ; i++ )
    {
        CheckSum += EDID_Buf[i] ; CheckSum &= 0xFF ;
    }
	
			//Eep_Write(0x80, 0x80, EDID_Buf) ;
	if( CheckSum != 0 )	// 128-byte EDID sum shall equal zero
	{
		return false ;
	}
	
	// check EDID Header
	if( EDID_Buf[0] != 0x00 ||
	    EDID_Buf[1] != 0xFF ||
	    EDID_Buf[2] != 0xFF ||
	    EDID_Buf[3] != 0xFF ||
	    EDID_Buf[4] != 0xFF ||
	    EDID_Buf[5] != 0xFF ||
	    EDID_Buf[6] != 0xFF ||
	    EDID_Buf[7] != 0x00)
    {
        return false ;
    }


    BlockCount = EDID_Buf[0x7E] ;  // Extention Flash: Number of 128-byte EDID extesion blocks to follow

    if( BlockCount == 0 )
    {
        return true ; // do nothing.
    }
    else if ( BlockCount > 4 )
    {
        BlockCount = 4 ;
    }
        	
     // read all segment for test
    for( i = 1 ; i <= BlockCount ; i++ )
    {
        err = GetEDIDData(i, EDID_Buf) ;

        if( err )
        {  
           if( !bValidCEA && EDID_Buf[0] == 0x2 && EDID_Buf[1] == 0x3 )  //EDID_Buf[0] == 0x2  ==> Additional timing data type 2
            {
                err = ParseCEAEDID(EDID_Buf, &RxCapability);
                if( err )
                {
 
				    if(RxCapability.IEEEOUI==0x0c03)
				    {
				    	RxCapability.ValidHDMI = true ;
				    	bValidCEA = true ;
					}
				    else
				    {
				    	RxCapability.ValidHDMI = false ;
				    }
				                   
                }
            }
        }
    }

    return err?false:true ;

}