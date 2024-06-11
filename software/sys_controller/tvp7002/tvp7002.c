//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "i2c_opencores.h"
#include "tvp7002.h"

//#define SYNCBYPASS    // Bypass VGA syncs (for debug - needed for interlace?)
//#define ADCPOWERDOWN  // Power-down ADCs
//#define PLLPOSTDIV    // Double-rate PLL with div-by-2 (decrease jitter?)

/* Y'Pb'Pr' to R'G'B' CSC coefficients.
 *
 * Coefficients from "Colour Space Conversions" (http://www.poynton.com/PDFs/coloureq.pdf).
 */
const ypbpr_to_rgb_csc_t csc_coeffs[] = {
    { "Rec. 601", 0x2000, 0x0000, 0x2CE5, 0x2000, 0xF4FD, 0xE926, 0x2000, 0x38BC, 0x0000 },    // eq. 101
    { "Rec. 709", 0x2000, 0x0000, 0x323E, 0x2000, 0xFA04, 0xF113, 0x2000, 0x3B61, 0x0000 },    // eq. 105
};

static const alt_u8 Kvco[] = {75, 85, 150, 200};
static const char *Kvco_str[] = { "Ultra low", "Low", "Medium", "High" };

static void tvp_set_clamp_type(video_format fmt)
{
    alt_u8 status = tvp_readreg(TVP_SOGTHOLD) & 0xF8;

    switch (fmt) {
    case FORMAT_YPbPr:
        //select mid clamp for Pb & Pr
        status |= 0x5;
        break;
    case FORMAT_RGBS:
    case FORMAT_RGBHV:
    case FORMAT_RGsB:
    default:
        //select bottom clamp (RGB)
        break;
    }
    tvp_writereg(TVP_SOGTHOLD, status);
}

static void tvp_set_clamp_alc(video_type type, alt_u8 clamp_ref_offset, alt_8 clamp_user_offset, alt_u8 en_alc)
{
    alt_16 clamp_pos = clamp_ref_offset + clamp_user_offset;
    alt_u8 clamp_width, alc_offset;

    switch (type) {
    /*case VIDEO_LDTV:
        clamp_pos += 2;
        clamp_width = 6;
        alc_offset = 1;
        break;*/
    case VIDEO_HDTV:
        clamp_pos += 50;
        clamp_width = 32;
        alc_offset = 8;
        break;
    case VIDEO_SDTV:
    case VIDEO_EDTV:
    case VIDEO_PC:
    default:
        clamp_pos += 6;
        clamp_width = 16;
        alc_offset = 2;
        break;
    }

    // Make sure clamp and ALC positions are within 8bit range
    if (clamp_pos < 0)
        clamp_pos = 0;
    else if (clamp_pos + clamp_width + alc_offset > 255)
        clamp_pos = 255 - alc_offset - clamp_width;

    printf("Clamp pos: %u, width: %u (ref_offset: %u, user_offset %d)\n", clamp_pos, clamp_width, clamp_ref_offset, clamp_user_offset);

    tvp_writereg(TVP_CLAMPSTART, (alt_u8)clamp_pos);
    tvp_writereg(TVP_CLAMPWIDTH, clamp_width);

    if (en_alc) {
        tvp_writereg(TVP_ALCEN, 0x80); //enable ALC
        tvp_writereg(TVP_ALCPLACE, clamp_pos+clamp_width+alc_offset);
    } else {
        tvp_writereg(TVP_ALCEN, 0x00); //disable ALC
    }
}

static void tvp_sel_clk(tvp_refclk_t refclk, alt_u8 ext_pclk)
{
    alt_u8 status = tvp_readreg(TVP_INPMUX2) & 0xF5;

    //TODO: set SOG and CLP LPF based on mode
    if (refclk == REFCLK_EXT27) {
        status |= 0x8;

        if (!ext_pclk)
            status |= 0x2;
    } else {
        status |= 0x2;
    }

    tvp_writereg(TVP_INPMUX2, status);
}

inline alt_u32 tvp_readreg(alt_u32 regaddr)
{
    I2C_start(I2CA_BASE, TVP_BASE, 0);
    I2C_write(I2CA_BASE, regaddr, 1);   //don't use repeated start as it seems unreliable at 400kHz
    I2C_start(I2CA_BASE, TVP_BASE, 1);
    return I2C_read(I2CA_BASE,1);
}

inline void tvp_writereg(alt_u32 regaddr, alt_u8 data)
{
    I2C_start(I2CA_BASE, TVP_BASE, 0);
    I2C_write(I2CA_BASE, regaddr, 0);
    I2C_write(I2CA_BASE, data, 1);
}

inline void tvp_reset()
{
    /*usleep(10000);
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, 0x00);
    usleep(10000);
    IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, 0x01);
    usleep(10000);*/
}

inline void tvp_disable_output()
{
    tvp_writereg(TVP_MISCCTRL2, 0x03);
}

inline void tvp_enable_output()
{
    tvp_writereg(TVP_MISCCTRL2, 0x00);
}

inline void tvp_powerdown()
{
    alt_u8 syncproc_rst = tvp_readreg(TVP_MISCCTRL4) | (1<<7);

    tvp_writereg(TVP_MISCCTRL4, syncproc_rst);
    usleep(1000);
    tvp_writereg(TVP_MISCCTRL4, syncproc_rst & 0x7F);
    usleep(1000);
    tvp_writereg(TVP_MISCCTRL1, 0x13);
    tvp_writereg(TVP_POWERCTRL, 0x07);
}

inline void tvp_powerup()
{
    tvp_writereg(TVP_MISCCTRL1, 0x11);
    tvp_writereg(TVP_POWERCTRL, 0x00);
}

inline void tvp_set_hpllcoast(alt_u8 pre, alt_u8 post)
{
    tvp_writereg(TVP_HPLLPRECOAST, pre);
    tvp_writereg(TVP_HPLLPOSTCOAST, post);
}

inline void tvp_set_linelen_tol(alt_u8 val) {
    tvp_writereg(TVP_LINELENTOL, val);
}

inline void tvp_set_ssthold(alt_u8 vsdetect_thold)
{
    tvp_writereg(TVP_SSTHOLD, vsdetect_thold);
}

void tvp_init()
{
    color_setup_t def_gain_offs = {
        .r_f_gain = DEFAULT_FINE_GAIN,
        .g_f_gain = DEFAULT_FINE_GAIN,
        .b_f_gain = DEFAULT_FINE_GAIN,
        .r_f_off = DEFAULT_FINE_OFFSET,
        .g_f_off = DEFAULT_FINE_OFFSET,
        .b_f_off = DEFAULT_FINE_OFFSET,
        .c_gain = DEFAULT_COARSE_GAIN,
    };

    // Set default configuration (skip those which match register reset values)

    // Configure external refclk, HPLL generated pclk
    tvp_sel_clk(REFCLK_EXT27, 0);

    // Hsync input->output delay (horizontal shift)
    // Default is 13, which maintains alignment of RGB and hsync at output
    //tvp_writereg(TVP_HSOUTSTART, 0);

    // Hsync edge->Vsync edge delay
    // NOTE: Value 1 syncs the edges!
    tvp_writereg(TVP_VSOUTALIGN, 1);

    // Bypass VSYNC processing
    tvp_writereg(TVP_MISCCTRL4, 0x09);

    // Set default CSC coeffs.
    tvp_sel_csc(&csc_coeffs[0]);

    // Set default phase
    //tvp_set_hpll_phase(DEFAULT_SAMPLER_PHASE, 1);

    // Set min video LPF, max sync LPF
    //tvp_set_lpf(0);
    tvp_set_sync_lpf(DEFAULT_SYNC_LPF);

    // Increase line length tolerance
    tvp_set_linelen_tol(DEFAULT_LINELEN_TOL);

    // Use HSYNC leading edge as fine clamp reference
    // Allows minimizing HSYNC window
    // Does not work properly for some reason, but is not critical as reference is at most LEADEDGE+MVSWIDTH
    //tvp_writereg(TVP_OUTFORMAT, 0x0C);

    // Minimize HSYNC window for best sync stability
    //tvp_writereg(TVP_MVSWIDTH, 0x03);

    // Common sync separator threshold
    // Some arcade games need more that the default 0x40
    tvp_set_ssthold(DEFAULT_VSYNC_THOLD);

    // Analog sync/SoG thresholf
    //tvp_set_sog_thold(DEFAULT_SYNC_VTH);

    // Default (3,3) coast may lead to PLL jitter and sync loss (e.g. SNES)
    tvp_set_hpllcoast(DEFAULT_PRE_COAST, DEFAULT_POST_COAST);

    //set analog (coarse) gain to max recommended value (-> 91% of the ADC range with 0.7Vpp input)
    //set rest of the gain digitally (fine) to utilize 100% of the range at the output (0.91*(1+(26/256)) = 1)
    tvp_set_gain_offset(&def_gain_offs);

    // Disable PLL and ADC but enable outputs to enable frontend mode detection
    tvp_enable_output();
    tvp_powerdown();
}

void tvp_set_gain_offset(color_setup_t *col) {
    tvp_writereg(TVP_BG_CGAIN, ((col->c_gain << 4) | col->c_gain));
    tvp_writereg(TVP_R_CGAIN, col->c_gain);
    tvp_writereg(TVP_R_FGAIN, col->r_f_gain);
    tvp_writereg(TVP_G_FGAIN, col->g_f_gain);
    tvp_writereg(TVP_B_FGAIN, col->b_f_gain);
    tvp_writereg(TVP_R_FOFFSET_MSB, col->r_f_off);
    tvp_writereg(TVP_G_FOFFSET_MSB, col->g_f_off);
    tvp_writereg(TVP_B_FOFFSET_MSB, col->b_f_off);
}

// Configure H-PLL (sampling rate, VCO gain and charge pump current)
void tvp_setup_hpll(alt_u16 h_samplerate, alt_u16 pixs_per_line, alt_u16 refclks_per_line, alt_u8 plldivby2)
{
    alt_u32 pclk_est;
    alt_u8 vco_range;
    alt_u8 cp_current;

    alt_u8 status = tvp_readreg(TVP_HPLLPHASE) & 0xF8;

    // Enable PLL post-div-by-2 with double samplerate
    if (plldivby2 && (h_samplerate < 2048)) {
        tvp_writereg(TVP_HPLLPHASE, status|1);
        h_samplerate = 2*h_samplerate;
    } else {
        tvp_writereg(TVP_HPLLPHASE, status);
    }

    tvp_writereg(TVP_HPLLDIV_MSB, (h_samplerate >> 4));
    tvp_writereg(TVP_HPLLDIV_LSB, ((h_samplerate & 0xf) << 4));

    printf("Horizontal samplerate set to %u\n", h_samplerate);

    pclk_est = ((alt_u32)h_samplerate * (TVP_EXTCLK_HZ/(alt_u32)refclks_per_line)) / 1000; //in kHz

    printf("Estimated PCLK_HPLL: %lu.%.3lu MHz\n", pclk_est/1000, pclk_est%1000);

    if (pclk_est < 36000) {
        vco_range = 0;
    } else if (pclk_est < 70000) {
        vco_range = 1;
    } else if (pclk_est < 135000) {
        vco_range = 2;
    } else {
        vco_range = 3;
    }

    cp_current = (40*Kvco[vco_range]+pixs_per_line/2) / pixs_per_line; //"+pixs_per_line/2" for fast rounding
    if (cp_current > 6)
        cp_current = 6;

    printf("VCO range: %s\nCPC: %u\n", Kvco_str[vco_range], cp_current);
    tvp_writereg(TVP_HPLLCTRL, ((vco_range << 6) | (cp_current << 3)));
}

void tvp_sel_csc(const ypbpr_to_rgb_csc_t *csc)
{
    tvp_writereg(TVP_CSC1HI, (csc->G_Y >> 8));
    tvp_writereg(TVP_CSC1LO, (csc->G_Y & 0xff));
    tvp_writereg(TVP_CSC2HI, (csc->G_Pb >> 8));
    tvp_writereg(TVP_CSC2LO, (csc->G_Pb & 0xff));
    tvp_writereg(TVP_CSC3HI, (csc->G_Pr >> 8));
    tvp_writereg(TVP_CSC3LO, (csc->G_Pr & 0xff));

    tvp_writereg(TVP_CSC4HI, (csc->R_Y >> 8));
    tvp_writereg(TVP_CSC4LO, (csc->R_Y & 0xff));
    tvp_writereg(TVP_CSC5HI, (csc->R_Pb >> 8));
    tvp_writereg(TVP_CSC5LO, (csc->R_Pb & 0xff));
    tvp_writereg(TVP_CSC6HI, (csc->R_Pr >> 8));
    tvp_writereg(TVP_CSC6LO, (csc->R_Pr & 0xff));

    tvp_writereg(TVP_CSC7HI, (csc->B_Y >> 8));
    tvp_writereg(TVP_CSC7LO, (csc->B_Y & 0xff));
    tvp_writereg(TVP_CSC8HI, (csc->B_Pb >> 8));
    tvp_writereg(TVP_CSC8LO, (csc->B_Pb & 0xff));
    tvp_writereg(TVP_CSC9HI, (csc->B_Pr >> 8));
    tvp_writereg(TVP_CSC9LO, (csc->B_Pr & 0xff));
}

void tvp_set_lpf(alt_u8 val)
{
    alt_u8 status = tvp_readreg(TVP_VIDEOBWLIM) & 0xF0;
    tvp_writereg(TVP_VIDEOBWLIM, status|val);
    printf("TVP LPF value set to 0x%x\n", val);
}

void tvp_set_sync_lpf(alt_u8 val)
{
    alt_u8 status = tvp_readreg(TVP_INPMUX2) & 0x3F;
    tvp_writereg(TVP_INPMUX2, status|(val<<6));
    printf("Sync LPF value set to 0x%x\n", val);
}

void tvp_set_clp_lpf(alt_u8 val)
{
    alt_u8 status = tvp_readreg(TVP_INPMUX2) & 0xCF;
    tvp_writereg(TVP_INPMUX2, status|(val<<4));
    printf("CLP LPF value set to 0x%x\n", val);
}

void tvp_set_hpll_phase(alt_u8 val)
{
    alt_u8 status = tvp_readreg(TVP_HPLLPHASE) & 0x07;

    tvp_writereg(TVP_HPLLPHASE, (val<<3)|status);
    printf("TVP Phase set to %u/32 (%u deg)\n", val, (val*11250)/1000);
}

void tvp_set_sog_thold(alt_u8 val)
{
    alt_u8 status = tvp_readreg(TVP_SOGTHOLD) & 0x07;
    tvp_writereg(TVP_SOGTHOLD, (val<<3)|status);
    printf("SOG thold set to 0x%x\n", val);
}

void tvp_set_alcfilt(alt_u8 nsv, alt_u8 nsh) {
    tvp_writereg(TVP_ALCFILT, (nsv<<3)|nsh);
}

void tvp_source_setup(video_type type, alt_u16 h_samplerate, alt_u16 pixs_per_line, alt_u16 refclks_per_line, alt_u8 plldivby2, alt_u8 h_synclen_px, alt_8 clamp_user_offset)
{
    // Due to short MVS width, clamp reference starts prematurely (at the end of MVS window). Adjust offset so that reference moves back to hsync trailing edge.
    alt_u8 clamp_ref_offset = h_synclen_px - (((30*h_samplerate)/refclks_per_line)+5)/10;

    // Clamp and ALC
    tvp_set_clamp_alc(type, clamp_ref_offset, clamp_user_offset, 1);

    // Setup Macrovision stripper and H-PLL coast signal.
    // Coast needs to be enabled when HSYNC is missing during VSYNC. RGBHV mode cannot use it, so turn off the internal signal for this mode.
    // Macrovision stripper filters out glitches and serration pulses that may occur outside of sync window (HSYNC_lead +- TVP_MVSWIDTH*37ns). Enabled for all inputs.
    switch (type) {
    case VIDEO_PC:
        tvp_writereg(TVP_MISCCTRL4, 0x0D);
        break;
    //case VIDEO_LDTV:
    case VIDEO_SDTV:
    case VIDEO_EDTV:
    case VIDEO_HDTV:
    default:
        tvp_writereg(TVP_MISCCTRL4, 0x09);
        break;
    }

    tvp_setup_hpll(h_samplerate, pixs_per_line, refclks_per_line, plldivby2);
}

void tvp_source_sel(tvp_input_t input, tvp_sync_input_t syncinput, video_format fmt)
{
    alt_u8 sync_status;

    // RGB+SOG input select
    tvp_writereg(TVP_INPMUX1, (((syncinput <= TVP_SOG3) ? syncinput : 0)<<6) | (input<<4) | (input<<2) | input);

    // Clamp setup
    tvp_set_clamp_type(fmt);

    // HV/SOG sync select
    if (syncinput > TVP_SOG3) {
        if (syncinput < TVP_CS_A)
            tvp_writereg(TVP_SYNCCTRL1, 0x52);
        else // RGBS
            tvp_writereg(TVP_SYNCCTRL1, 0x53);

        usleep(100000);
        sync_status = tvp_readreg(TVP_SYNCSTAT);
        if (sync_status & (1<<7))
            printf("%s detected, %s polarity\n", (sync_status & (1<<3)) ? "Csync" : "Hsync", (sync_status & (1<<5)) ? "pos" : "neg");
        if (sync_status & (1<<4))
            printf("Vsync detected, %s polarity\n", (sync_status & (1<<2)) ? "pos" : "neg");
    } else {
        tvp_writereg(TVP_SYNCCTRL1, 0x5B);
        usleep(1000);
        sync_status = tvp_readreg(TVP_SYNCSTAT);
        if (sync_status & (1<<1))
            printf("SOG detected\n");
        else
            printf("SOG not detected\n");
    }

    // Enable CSC for YPbPr
    if (fmt == FORMAT_YPbPr)
        tvp_writereg(TVP_MISCCTRL3, 0x10);
    else
        tvp_writereg(TVP_MISCCTRL3, 0x00);

#ifdef SYNCBYPASS
    tvp_writereg(TVP_SYNCBYPASS, 0x03);
#else
    tvp_writereg(TVP_SYNCBYPASS, 0x00);
#endif

    //TODO:
    //TVP_ADCSETUP

    printf("\n");
}

alt_u8 tvp_check_sync(tvp_sync_input_t syncinput)
{
    alt_u8 sync_status;

    sync_status = tvp_readreg(TVP_SYNCSTAT);

    if (syncinput >= TVP_CS_A)
        return ((sync_status & 0x88) == 0x88);
    else if (syncinput >= TVP_HV_A)
        return ((sync_status & 0x90) == 0x90);
    else
        return !!(sync_status & (1<<1));
}
