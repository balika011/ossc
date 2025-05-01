//
// Copyright (C) 2019-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
// Copyright (C) 2025       Balázs Triszka   <info@balika011.hu>
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

module osd_generator_top (
	// common
	input clk_i,
	input rst_i,

	// avalon slave
	input [31:0] avalon_s_writedata,
	output [31:0] avalon_s_readdata,
	input [7:0] avalon_s_address,
	input [3:0] avalon_s_byteenable,
	input avalon_s_write,
	input avalon_s_read,
	input avalon_s_chipselect,
	output avalon_s_waitrequest_n,

	// video clock
	input vclk,

	// avalon master
	output logic [31:0] avalon_m_address,
	input logic [31:0] avalon_m_readdata,
	output logic avalon_m_read,
	input logic avalon_m_readdatavalid,
	input logic avalon_m_waitrequest,
	output logic avalon_m_write,
	output logic [3:0] avalon_m_byteenable,
	output logic [8:0] avalon_m_writedata,
	input logic [1:0] avalon_m_response,
	input logic avalon_m_writeresponsevalid,

	input [11:0] x,
	input [10:0] y,

	output [7:0] osd_alpha,
	output [7:0] osd_red,
	output [7:0] osd_green,
	output [7:0] osd_blue
);

localparam OSD_CONFIG_REGNUM =	8'h0;
localparam OSD_POS_REGNUM =		8'h1;
localparam OSD_SIZE_REGNUM =	8'h2;
localparam OSD_COLORS0_REGNUM =	8'h3;
localparam OSD_COLORS1_REGNUM =	8'h4;
localparam OSD_COLORS2_REGNUM =	8'h5;
localparam OSD_COLORS3_REGNUM =	8'h6;

reg [31:0] osd_config;
reg [11:0] osd_xpos;
reg [10:0] osd_ypos;
reg [11:0] osd_xsize;
reg [10:0] osd_ysize;
reg [31:0] osd_colors[3:0];

wire render_enable = osd_config[0];

wire [20:0] pixel_idx = (y - osd_ypos) * osd_xsize + (x - osd_xpos);

reg [31:0] fb_address;
reg fb_read;
reg [31:0] fb_data_temp;
reg [31:0] fb_data;
wire [1:0] fb_pixel[15:0];
assign fb_pixel[0] = fb_data[1:0];
assign fb_pixel[1] = fb_data[3:2];
assign fb_pixel[2] = fb_data[5:4];
assign fb_pixel[3] = fb_data[7:6];
assign fb_pixel[4] = fb_data[9:8];
assign fb_pixel[5] = fb_data[11:10];
assign fb_pixel[6] = fb_data[13:12];
assign fb_pixel[7] = fb_data[15:14];
assign fb_pixel[8] = fb_data[17:16];
assign fb_pixel[9] = fb_data[19:18];
assign fb_pixel[10] = fb_data[21:20];
assign fb_pixel[11] = fb_data[23:22];
assign fb_pixel[12] = fb_data[25:24];
assign fb_pixel[13] = fb_data[27:26];
assign fb_pixel[14] = fb_data[29:28];
assign fb_pixel[15] = fb_data[31:30];

assign avalon_m_address = fb_address;
assign avalon_m_read = fb_read;
assign avalon_m_write = 1'b0;
assign avalon_m_byteenable = 4'b1111;
assign avalon_m_writedata = 32'b0;

reg offscreen_read;

always @(negedge vclk or posedge rst_i) begin
	if (rst_i) begin
		fb_read <= 0;
		offscreen_read <= 0;
	end else begin
		if (render_enable & x >= osd_xpos & x < (osd_xpos + osd_xsize) & y >= osd_ypos & y < (osd_ypos + osd_ysize)) begin
			offscreen_read <= 0;

			// Prepare for the next 16 pixels
			if (pixel_idx[3:0] == 0) begin
				fb_address <= { (pixel_idx[20:4] + 1), 2'd0 };
				fb_read <= 1;
			end else if (avalon_m_readdatavalid) begin
				if (pixel_idx[3:0] == 15)
					fb_data <= avalon_m_readdata;
				else
					fb_data_temp <= avalon_m_readdata;
				fb_read <= 0;
			end else if (pixel_idx[3:0] == 15) begin
				fb_data <= fb_data_temp;
			end

			{ osd_alpha, osd_red, osd_green, osd_blue } <= osd_colors[fb_pixel[pixel_idx]];
		end else begin
			{ osd_alpha, osd_red, osd_green, osd_blue } <= 32'h00000000;

			if (y < osd_ypos | y > (osd_ypos + osd_ysize)) begin
				if (offscreen_read == 0) begin
					offscreen_read <= 1;
					fb_address <= 0;
					fb_read <= 1;
				end else if (avalon_m_readdatavalid) begin
					fb_data <= avalon_m_readdata;
					fb_read <= 0;
				end
			end
		end
	end
end

// Avalon register interface

assign avalon_s_waitrequest_n = 1'b1;

always @(posedge clk_i or posedge rst_i) begin
	if (rst_i) begin
		osd_config <= 32'h0;
	end else begin
		if (avalon_s_chipselect && avalon_s_write) begin
			if (avalon_s_address == OSD_CONFIG_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_config[31:24] <= avalon_s_writedata[31:24];
				if (avalon_s_byteenable[2])
					osd_config[23:16] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_config[15:8] <= avalon_s_writedata[15:8];
				if (avalon_s_byteenable[0])
					osd_config[7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_POS_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_xpos[11:8] <= avalon_s_writedata[27:24];
				if (avalon_s_byteenable[2])
					osd_xpos[7:0] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_ypos[10:8] <= avalon_s_writedata[10:8];
				if (avalon_s_byteenable[0])
					osd_ypos[7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_SIZE_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_xsize[11:8] <= avalon_s_writedata[27:24];
				if (avalon_s_byteenable[2])
					osd_xsize[7:0] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_ysize[10:8] <= avalon_s_writedata[10:8];
				if (avalon_s_byteenable[0])
					osd_ysize[7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_COLORS0_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_colors[0][31:24] <= avalon_s_writedata[31:24];
				if (avalon_s_byteenable[2])
					osd_colors[0][23:16] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_colors[0][15:8] <= avalon_s_writedata[15:8];
				if (avalon_s_byteenable[0])
					osd_colors[0][7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_COLORS1_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_colors[1][31:24] <= avalon_s_writedata[31:24];
				if (avalon_s_byteenable[2])
					osd_colors[1][23:16] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_colors[1][15:8] <= avalon_s_writedata[15:8];
				if (avalon_s_byteenable[0])
					osd_colors[1][7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_COLORS2_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_colors[2][31:24] <= avalon_s_writedata[31:24];
				if (avalon_s_byteenable[2])
					osd_colors[2][23:16] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_colors[2][15:8] <= avalon_s_writedata[15:8];
				if (avalon_s_byteenable[0])
					osd_colors[2][7:0] <= avalon_s_writedata[7:0];
			end else if (avalon_s_address == OSD_COLORS3_REGNUM) begin
				if (avalon_s_byteenable[3])
					osd_colors[3][31:24] <= avalon_s_writedata[31:24];
				if (avalon_s_byteenable[2])
					osd_colors[3][23:16] <= avalon_s_writedata[23:16];
				if (avalon_s_byteenable[1])
					osd_colors[3][15:8] <= avalon_s_writedata[15:8];
				if (avalon_s_byteenable[0])
					osd_colors[3][7:0] <= avalon_s_writedata[7:0];
			end
		end
	end
end

always @(*) begin
	if (avalon_s_chipselect && avalon_s_read) begin
		if (avalon_s_address == OSD_CONFIG_REGNUM) begin
			avalon_s_readdata = osd_config;
		end else if (avalon_s_address == OSD_POS_REGNUM) begin
			avalon_s_readdata = { 4'd0, osd_xpos, 5'd0, osd_ypos };
		end else if (avalon_s_address == OSD_SIZE_REGNUM) begin
			avalon_s_readdata = { 4'd0, osd_xsize, 5'd0, osd_ysize };
		end else if (avalon_s_address == OSD_COLORS0_REGNUM) begin
			avalon_s_readdata = osd_colors[0];
		end else if (avalon_s_address == OSD_COLORS1_REGNUM) begin
			avalon_s_readdata = osd_colors[1];
		end else if (avalon_s_address == OSD_COLORS2_REGNUM) begin
			avalon_s_readdata = osd_colors[2];
		end else if (avalon_s_address == OSD_COLORS3_REGNUM) begin
			avalon_s_readdata = osd_colors[3];
		end
	end else begin
		avalon_s_readdata = 32'h00000000;
	end
end

endmodule