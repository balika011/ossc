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

module videogen (
    input pclk,
    input [11:0] H_ACTIVE,
    input [10:0] V_ACTIVE,
    input [11:0] xpos,
    input [10:0] ypos,
    output reg [7:0] R_out,
    output reg [7:0] G_out,
    output reg [7:0] B_out
);

parameter   H_OVERSCAN      =   10'd40; //at both sides
parameter   V_OVERSCAN      =   10'd16; //top and bottom
parameter   H_GRADIENT      =   10'd512;
parameter   V_GRADIENT      =   10'd256;
parameter   V_GRAYRAMP      =   10'd84;

wire [11:0] H_BORDER        =   ((H_ACTIVE - (H_OVERSCAN << 1) - H_GRADIENT) >> 1);
wire [10:0] V_BORDER        =   ((V_ACTIVE - (V_OVERSCAN << 1) - V_GRADIENT) >> 1);

// Pattern gen
always @(posedge pclk)
begin
	if ((xpos < H_OVERSCAN) || (xpos >= H_ACTIVE - H_OVERSCAN) || (ypos < V_OVERSCAN) || (ypos >= V_ACTIVE - V_OVERSCAN))
		{R_out, G_out, B_out} <= {3{(xpos[0] ^ ypos[0]) ? 8'hff : 8'h00}};
	else if ((xpos < H_OVERSCAN + H_BORDER) || (xpos >= H_ACTIVE - H_OVERSCAN - H_BORDER) || (ypos < V_OVERSCAN + V_BORDER) || (ypos >= V_ACTIVE - V_OVERSCAN - V_BORDER))
		{R_out, G_out, B_out} <= {3{8'h50}};
	else if (ypos >= V_OVERSCAN + V_BORDER + V_GRADIENT - V_GRAYRAMP)
		{R_out, G_out, B_out} <= {3{8'((((xpos - (H_OVERSCAN + H_BORDER)) >> 4) << 3)  +  (xpos - (H_OVERSCAN + H_BORDER) >> 6))}};
	else
		{R_out, G_out, B_out} <= {3{8'((xpos - (H_OVERSCAN + H_BORDER)) >> 1)}};
end

endmodule
