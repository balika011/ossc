//
// Copyright (C) 2015-2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

//`define DEBUG

module ossc (
	input clk27,

	inout scl,
	inout sda,

	input ir_rx,
	input [1:0] btn,
	input [1:0] cfg,

	input TVP_PCLK,
	input [7:0] TVP_R,
	input [7:0] TVP_G,
	input [7:0] TVP_B,
	input TVP_HS,
	input TVP_SOG,
	input TVP_VS,

	output HDMI_TX_PCLK,
	output reg [7:0] HDMI_TX_RD,
	output reg [7:0] HDMI_TX_GD,
	output reg [7:0] HDMI_TX_BD,
	output reg HDMI_TX_DE,
	output reg HDMI_TX_HS,
	output reg HDMI_TX_VS,
	input HDMI_TX_INT_N,

	output av_reset_n,

	output LED_G,
	inout LED_R_OR_TVP_SOG,

	output LCD_RS,
	output LCD_CS_N,
	inout LCD_BL_OR_TVP_SOG,

	output SD_CLK,
	inout SD_CMD,
	inout [3:0] SD_DAT,

	input altera_reserved_tms,
	input altera_reserved_tck,
	input altera_reserved_tdi,
	output altera_reserved_tdo
);

assign HDMI_TX_PCLK = PCLK_sc;


wire LED_R_TVP_SOG_REMAPPED;
ALT_IOBUF led_r_iobuf (.i(LED_R), .oe(~remap_led_r_to_tvp_sog), .o(LED_R_TVP_SOG_REMAPPED), .io(LED_R_OR_TVP_SOG));
wire LCD_BL_TVP_SOG_REMAPPED;
ALT_IOBUF lcd_bl_iobuf (.i(LCD_BL), .oe(~remap_lcd_bl_to_tvp_sog), .o(LCD_BL_TVP_SOG_REMAPPED), .io(LCD_BL_OR_TVP_SOG));
wire TVP_SOG_SELECTED = remap_led_r_to_tvp_sog ? LED_R_TVP_SOG_REMAPPED : (remap_lcd_bl_to_tvp_sog ? LCD_BL_TVP_SOG_REMAPPED : TVP_SOG);

wire sd_cmd_oe_o, sd_cmd_out_o, sd_dat_oe_o;
wire [3:0] sd_dat_out_o;
assign SD_CMD = sd_cmd_oe_o ? sd_cmd_out_o : 1'bz;
assign SD_DAT = sd_dat_oe_o ? sd_dat_out_o : 4'bzzzz;




wire clk_reset_n;

wire [31:0] sys_ctrl;
wire remap_lcd_bl_to_tvp_sog = sys_ctrl[31];
wire remap_led_r_to_tvp_sog = sys_ctrl[30];
wire LED_R = sys_ctrl[8];
assign LED_G = sys_ctrl[7];
wire TVP_VS_type = sys_ctrl[6];
assign LCD_CS_N = sys_ctrl[5];
assign LCD_RS = sys_ctrl[4];
wire LCD_BL = sys_ctrl[3];
wire enable_sc = sys_ctrl[2];
assign av_reset_n = sys_ctrl[1];
wire hw_reset = sys_ctrl[0];

wire resync_indicator = (warn_pll_lock_lost != 0) | (resync_led_ctr != 0);
wire [31:0] controls = { 1'b0, resync_indicator, N_VSYNC_sc_LL, pll_activeclock, cfg_L, ~btn_LL, ir_code_cnt, ir_code };

wire osd_enable;
wire [1:0] osd_color;

sys sys_inst(
	.reset_reset_n(1'b1),
	.clk_clk(clk27),
	.clk_27_clk_reset_reset_n(clk_reset_n),
	.reset_bridge_0_in_reset_reset_n(clk_reset_n),
	.ibex_0_config_boot_addr_i(32'h10080000),
	.ibex_0_tck_clk(tck),
	.ibex_0_jtag_tdi(tdi),
	.ibex_0_jtag_tms(tms),
	.ibex_0_jtag_tdo(tdo),
	.ibex_0_jtag_trstn(),
	.i2c_opencores_0_export_scl_pad_io(scl),
	.i2c_opencores_0_export_sda_pad_io(sda),
	.i2c_opencores_0_export_spi_miso_pad_i(1'b0),
	.sdc_controller_0_sd_clk_o_clk(SD_CLK),
	.sdc_controller_0_sd_sd_cmd_dat_i(SD_CMD),
	.sdc_controller_0_sd_sd_cmd_out_o(sd_cmd_out_o),
	.sdc_controller_0_sd_sd_cmd_oe_o(sd_cmd_oe_o),
	.sdc_controller_0_sd_sd_dat_dat_i(SD_DAT),
	.sdc_controller_0_sd_sd_dat_out_o(sd_dat_out_o),
	.sdc_controller_0_sd_sd_dat_oe_o(sd_dat_oe_o),
	.sc_config_0_sc_in_fe_status_i({ 19'h0, TVP_sync_active, TVP_fe_interlace, TVP_fe_vtotal }),
	.sc_config_0_sc_in_fe_status2_i({ 4'h0, TVP_hsync_width, TVP_fe_pcnt_field }),
	.sc_config_0_sc_in_lt_status_i(32'h00000000),
	.sc_config_0_sc_in_controls_i(controls),
	.sc_config_0_sc_out_hv_in_config_o(hv_in_config),
	.sc_config_0_sc_out_hv_in_config2_o(hv_in_config2),
	.sc_config_0_sc_out_hv_in_config3_o(hv_in_config3),
	.sc_config_0_sc_out_hv_out_config_o(hv_out_config),
	.sc_config_0_sc_out_hv_out_config2_o(hv_out_config2),
	.sc_config_0_sc_out_hv_out_config3_o(hv_out_config3),
	.sc_config_0_sc_out_xy_out_config_o(xy_out_config),
	.sc_config_0_sc_out_xy_out_config2_o(xy_out_config2),
	.sc_config_0_sc_out_misc_config_o(misc_config),
	.sc_config_0_sc_out_sl_config_o(sl_config),
	.sc_config_0_sc_out_sl_config2_o(sl_config2),
	.sc_config_0_sc_out_sl_config3_o(sl_config3),
	.sc_config_0_sc_out_sys_ctrl_o(sys_ctrl),
	.osd_generator_0_osd_if_vclk(PCLK_sc),
	.osd_generator_0_osd_if_xpos(xpos_sc),
	.osd_generator_0_osd_if_ypos(ypos_sc),
	.osd_generator_0_osd_if_osd_enable(osd_enable),
	.osd_generator_0_osd_if_osd_color(osd_color),
	.pll_reconfig_0_pll_reconfig_if_areset(pll_areset),
	.pll_reconfig_0_pll_reconfig_if_scanclk(pll_scanclk),
	.pll_reconfig_0_pll_reconfig_if_scanclkena(pll_scanclkena),
	.pll_reconfig_0_pll_reconfig_if_configupdate(pll_configupdate),
	.pll_reconfig_0_pll_reconfig_if_scandata(pll_scandata),
	.pll_reconfig_0_pll_reconfig_if_scandone(pll_scandone)
);




logic tms, tck, tdi, tdo;

cycloneive_jtag jtag_inst(
	.tms(altera_reserved_tms),
	.tck(altera_reserved_tck),
	.tdi(altera_reserved_tdi),
	.tdo(altera_reserved_tdo),
	.tmsutap(tms),
	.tckutap(tck),
	.tdiutap(tdi),
	.tdouser(tdo)
);





cycloneive_rublock rublock_inst(
	.clk(clk27),
	.shiftnld(1'b0),
	.captnupdt(1'b0),
	.regin(1'b0),
	.rsttimer(1'b0),
	.rconfig(hw_reset)
);





wire pll_clkout, pll_clkswitch, pll_locked;

pll_2x pll_pclk (
	.areset(pll_areset),
	.clkswitch(pll_clkswitch),
	.configupdate(pll_configupdate),
	.inclk0(clk27), // set videogen clock to primary (power-on default) since both reference clocks must be running during switchover
	.inclk1(TVP_PCLK), // is the secondary input clock fully compensated?
	.scanclk(pll_scanclk),
	.scanclkena(pll_scanclkena),
	.scandata(pll_scandata),
	.activeclock(pll_activeclock),
	.c0(pll_clkout),
	.locked(pll_locked),
	.scandataout(),
	.scandone(pll_scandone)
);





reg [7:0] TVP_R_L, TVP_G_L, TVP_B_L;
reg TVP_HS_L, TVP_VS_L, TVP_FID;
reg TVP_VS_PCLK_L, TVP_VS_PCLK_LL;
reg TVP_SOG_SELECTED_PCLK_L, TVP_SOG_SELECTED_PCLK_LL, TVP_SOG_SELECTED_PCLK_LLL;
reg TVP_SOG_SELECTED_CLK27_L, TVP_SOG_SELECTED_CLK27_LL;
reg TVP_VS_CLK27_L, TVP_VS_CLK27_LL;
wire [7:0] TVP_R_L_post, TVP_G_L_post, TVP_B_L_post;
wire TVP_HS_post, TVP_VS_post, TVP_DE_post, TVP_FID_post, TVP_datavalid_post;
wire TVP_fe_interlace, TVP_fe_frame_change, TVP_sof_scaler, TVP_sync_active;
wire [19:0] TVP_fe_pcnt_field;
wire [7:0] TVP_hsync_width;
wire [10:0] TVP_fe_vtotal, TVP_fe_xpos, TVP_fe_ypos;
wire [31:0] hv_in_config, hv_in_config2, hv_in_config3, hv_out_config, hv_out_config2, hv_out_config3, xy_out_config, xy_out_config2;
wire [31:0] misc_config, sl_config, sl_config2, sl_config3;

tvp7002_frontend u_tvp_frontend ( 
	.PCLK_i(TVP_PCLK),
	.CLK_MEAS_i(clk27),
	.reset_n(av_reset_n),
	.R_i(TVP_R_L),
	.G_i(TVP_G_L),
	.B_i(TVP_B_L),
	.HS_i(TVP_HS_L),
	.VS_i(TVP_VS_PCLK_LL),
	.HSYNC_i(TVP_SOG_SELECTED_CLK27_LL),
	.VSYNC_i(TVP_VS_CLK27_LL),
	.DE_i(1'b0),
	.FID_i(1'b0),
	.sogref_update_i(TVP_SOG_SELECTED_PCLK_LLL & ~TVP_SOG_SELECTED_PCLK_LL),
	.vsync_i_type(TVP_VS_type),
	.hv_in_config(hv_in_config),
	.hv_in_config2(hv_in_config2),
	.hv_in_config3(hv_in_config3),
	.misc_config(misc_config),
	.R_o(TVP_R_L_post),
	.G_o(TVP_G_L_post),
	.B_o(TVP_B_L_post),
	.HSYNC_o(TVP_HS_post),
	.VSYNC_o(TVP_VS_post),
	.DE_o(TVP_DE_post),
	.FID_o(TVP_FID_post),
	.interlace_flag(TVP_fe_interlace),
	.datavalid_o(TVP_datavalid_post),
	.xpos_o(TVP_fe_xpos),
	.ypos_o(TVP_fe_ypos),
	.vtotal(TVP_fe_vtotal),
	.frame_change(TVP_fe_frame_change),
	.sof_scaler(TVP_sof_scaler),
	.pcnt_field(TVP_fe_pcnt_field),
	.hsync_width(TVP_hsync_width),
	.sync_active(TVP_sync_active)
);



wire [7:0] R_sc, G_sc, B_sc;
wire HSYNC_sc, VSYNC_sc, DE_sc;
wire pll_areset, pll_scanclk, pll_scanclkena, pll_configupdate, pll_scandata, pll_scandone, pll_activeclock;
wire PCLK_sc;
wire [10:0] xpos_sc;
wire [10:0] ypos_sc;
wire resync_strobe;

scanconverter #(
	.EMIF_ENABLE(0),
	.NUM_LINE_BUFFERS(2)
  ) scanconverter_inst (
	.PCLK_CAP_i(TVP_PCLK),
	.PCLK_OUT_i(pll_clkout),
	.reset_n(av_reset_n),
	.R_i(TVP_R_L_post),
	.G_i(TVP_G_L_post),
	.B_i(TVP_B_L_post),
	.HSYNC_i(TVP_HS_post),
	.VSYNC_i(TVP_VS_post),
	.DE_i(TVP_DE_post),
	.FID_i(TVP_FID_post),
	.datavalid_i(TVP_datavalid_post),
	.interlaced_in_i(TVP_fe_interlace),
	.frame_change_i(TVP_fe_frame_change),
	.xpos_i(TVP_fe_xpos),
	.ypos_i(TVP_fe_ypos),
	.h_in_active(hv_in_config[23:12]),
	.hv_out_config(hv_out_config),
	.hv_out_config2(hv_out_config2),
	.hv_out_config3(hv_out_config3),
	.xy_out_config(xy_out_config),
	.xy_out_config2(xy_out_config2),
	.misc_config(misc_config),
	.sl_config(sl_config),
	.sl_config2(sl_config2),
	.sl_config3(sl_config3),
	.testpattern_enable(~enable_sc),
	.lb_enable(enable_sc),
	.ext_sync_mode(1'b0),
	.ext_frame_change_i(1'b0),
	.ext_R_i(8'h00),
	.ext_G_i(8'h00),
	.ext_B_i(8'h00),
	.PCLK_o(PCLK_sc),
	.R_o(R_sc),
	.G_o(G_sc),
	.B_o(B_sc),
	.HSYNC_o(HSYNC_sc),
	.VSYNC_o(VSYNC_sc),
	.DE_o(DE_sc),
	.xpos_o(xpos_sc),
	.ypos_o(ypos_sc),
	.resync_strobe(resync_strobe),
	.emif_br_clk(1'b0),
	.emif_br_reset(1'b0),
	.emif_rd_addr(),
	.emif_rd_read(),
	.emif_rd_rdata(0),
	.emif_rd_waitrequest(0),
	.emif_rd_readdatavalid(0),
	.emif_rd_burstcount(),
	.emif_wr_addr(),
	.emif_wr_write(),
	.emif_wr_wdata(),
	.emif_wr_waitrequest(0),
	.emif_wr_burstcount()
);




wire [15:0] ir_code;
wire [7:0] ir_code_cnt;
reg ir_rx_LL;
ir_rcv ir0 (
	.clk27(clk27),
	.reset_n(clk_reset_n),
	.ir_rx(ir_rx_LL),
	.ir_code(ir_code),
	.ir_code_ack(),
	.ir_code_cnt(ir_code_cnt)
);




// Control PLL reference clock switchover
always @(posedge clk27)
begin
	pll_clkswitch <= (pll_activeclock != enable_sc);
end




reg [1:0] btn_L, btn_LL;
reg ir_rx_L;
reg HDMI_TX_INT_N_L, HDMI_TX_INT_N_LL;
reg cfg_stored;
reg [1:0] cfg_L;

// Insert synchronizers to async inputs (synchronize to CPU clock)
always @(posedge clk27 or negedge clk_reset_n)
begin
	if (!clk_reset_n) begin
		{btn_L, btn_LL} <= '0;

		{ir_rx_L, ir_rx_LL} <= '0;

		{HDMI_TX_INT_N_L, HDMI_TX_INT_N_LL} <= '0;

		{cfg_stored, cfg_L} <= '0;
	end else begin
		{btn_L, btn_LL} <= {btn, btn_L};

		{ir_rx_L, ir_rx_LL} <=  {ir_rx, ir_rx_L};

		{HDMI_TX_INT_N_L, HDMI_TX_INT_N_LL} <= {HDMI_TX_INT_N, HDMI_TX_INT_N_L};

		if (!cfg_stored)
			cfg_L <= cfg;

		cfg_stored <= 1'b1;
	end
end




reg N_VSYNC_sc_L, N_VSYNC_sc_LL;

// Sync vsync flag to CPU clock
always @(posedge clk27) begin
	{N_VSYNC_sc_L, N_VSYNC_sc_LL} <= {~VSYNC_sc, N_VSYNC_sc_L};
end





// TVP7002 RGB digitizer
always @(posedge TVP_PCLK) begin
	TVP_R_L <= TVP_R;
	TVP_G_L <= TVP_G;
	TVP_B_L <= TVP_B;
	TVP_HS_L <= TVP_HS;
	TVP_VS_L <= TVP_VS;

	// sync to pclk
	TVP_SOG_SELECTED_PCLK_L <= TVP_SOG_SELECTED;
	TVP_SOG_SELECTED_PCLK_LL <= TVP_SOG_SELECTED_PCLK_L;
	TVP_SOG_SELECTED_PCLK_LLL <= TVP_SOG_SELECTED_PCLK_LL;

	TVP_VS_PCLK_L <= TVP_VS;
	TVP_VS_PCLK_LL <= TVP_VS_PCLK_L;
end

always @(posedge clk27) begin
	// sync to always-running fixed meas clk
	TVP_SOG_SELECTED_CLK27_L <= TVP_SOG_SELECTED;
	TVP_SOG_SELECTED_CLK27_LL <= TVP_SOG_SELECTED_CLK27_L;

	TVP_VS_CLK27_L <= TVP_VS;
	TVP_VS_CLK27_LL <= TVP_VS_CLK27_L;
end




// Output registers
always @(posedge PCLK_sc) begin
	if (osd_enable) begin
		if (osd_color == 2'h0) begin
			{HDMI_TX_RD, HDMI_TX_GD, HDMI_TX_BD} <= 24'h000000;
		end else if (osd_color == 2'h1) begin
			{HDMI_TX_RD, HDMI_TX_GD, HDMI_TX_BD} <= 24'h0000ff;
		end else if (osd_color == 2'h2) begin
			{HDMI_TX_RD, HDMI_TX_GD, HDMI_TX_BD} <= 24'hffff00;
		end else begin
			{HDMI_TX_RD, HDMI_TX_GD, HDMI_TX_BD} <= 24'hffffff;
		end
	end else begin
		{HDMI_TX_RD, HDMI_TX_GD, HDMI_TX_BD} <= {R_sc, G_sc, B_sc};
	end

	HDMI_TX_HS <= HSYNC_sc;
	HDMI_TX_VS <= VSYNC_sc;
	HDMI_TX_DE <= DE_sc;
end




reg [23:0] resync_led_ctr;
reg resync_strobe_L, resync_strobe_LL, resync_strobe_LLL;

// Generate a warning signal from sync lock loss
always @(posedge clk27) begin
	if (enable_sc) begin
		if (~resync_strobe_LLL & resync_strobe_LL) begin
			resync_led_ctr <= {24{1'b1}};
		end else if (resync_led_ctr > 0) begin
			resync_led_ctr <= resync_led_ctr - 1'b1;
		end
	end

	resync_strobe_L <= resync_strobe;
	resync_strobe_LL <= resync_strobe_L;
	resync_strobe_LLL <= resync_strobe_LL;
end




reg warn_pll_lock_lost;

// Generate a warning signal from PLL lock loss
always @(posedge clk27 or negedge clk_reset_n)
begin
	if (!clk_reset_n) begin
		warn_pll_lock_lost <= 1'b0;
	end else begin
		if (~pll_areset & ~pll_locked)
			warn_pll_lock_lost <= 1;
		else if (warn_pll_lock_lost != 0)
			warn_pll_lock_lost <= warn_pll_lock_lost + 1'b1;
	end
end

endmodule