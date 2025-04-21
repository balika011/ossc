// (C) 2001-2025 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions and other 
// software and tools, and its AMPP partner logic functions, and any output 
// files from any of the foregoing (including device programming or simulation 
// files), and any associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License Subscription 
// Agreement, Altera IP License Agreement, or other applicable 
// license agreement, including, without limitation, that your use is for the 
// sole purpose of programming logic devices manufactured by Altera and sold by 
// Altera or its authorized distributors.  Please refer to the applicable 
// agreement for further details.


`timescale 1ns / 1ns

module intel_generic_serial_flash_interface_addr_swapped #(
    parameter ADDR_WIDTH    = 22
)(
    input                  clk,
    input                  reset,
                                                        
    // ports to access memory                   
    input                  avl_mem_write,
    input                  avl_mem_read,
    input [ADDR_WIDTH-1:0] avl_mem_addr,
    input [31:0]           avl_mem_wrdata,
    input [3:0]            avl_mem_byteenable,
    input [6:0]            avl_mem_burstcount,
    output [31:0]          avl_mem_rddata,
    output logic           avl_mem_rddata_valid,
    output logic           avl_mem_waitrequest,
    
    
    // ASMI PARALLEL interface
    output logic [31:0]    gen_qspi_mem_addr, 
    output logic           gen_qspi_mem_read, 
    input [31:0]           gen_qspi_mem_rddata, 
    output logic           gen_qspi_mem_write, 
    output logic [31:0]    gen_qspi_mem_wrdata, 
    output logic [3:0]     gen_qspi_mem_byteenable, 
    output logic [6:0]     gen_qspi_mem_burstcount, 
    input                  gen_qspi_mem_waitrequest, 
    input                  gen_qspi_mem_rddata_valid

);

    // Do nothing, except adjust the address
     
    assign gen_qspi_mem_addr        = {{31-ADDR_WIDTH{1'b0}}, avl_mem_addr};
    assign gen_qspi_mem_read        = avl_mem_read;
    assign gen_qspi_mem_write       = avl_mem_write;
    assign gen_qspi_mem_wrdata      = {
		avl_mem_wrdata[24], avl_mem_wrdata[25], avl_mem_wrdata[26], avl_mem_wrdata[27], avl_mem_wrdata[28], avl_mem_wrdata[29], avl_mem_wrdata[30], avl_mem_wrdata[31],
		avl_mem_wrdata[16], avl_mem_wrdata[17], avl_mem_wrdata[18], avl_mem_wrdata[19], avl_mem_wrdata[20], avl_mem_wrdata[21], avl_mem_wrdata[22], avl_mem_wrdata[23],
		avl_mem_wrdata[ 8], avl_mem_wrdata[ 9], avl_mem_wrdata[10], avl_mem_wrdata[11], avl_mem_wrdata[12], avl_mem_wrdata[13], avl_mem_wrdata[14], avl_mem_wrdata[15],
		avl_mem_wrdata[ 0], avl_mem_wrdata[ 1], avl_mem_wrdata[ 2], avl_mem_wrdata[ 3], avl_mem_wrdata[ 4], avl_mem_wrdata[ 5], avl_mem_wrdata[ 6], avl_mem_wrdata[ 7]
	};
    assign gen_qspi_mem_byteenable  = avl_mem_byteenable;
    assign gen_qspi_mem_burstcount  = avl_mem_burstcount;
    assign avl_mem_rddata           = {
		gen_qspi_mem_rddata[24], gen_qspi_mem_rddata[25], gen_qspi_mem_rddata[26], gen_qspi_mem_rddata[27], gen_qspi_mem_rddata[28], gen_qspi_mem_rddata[29], gen_qspi_mem_rddata[30], gen_qspi_mem_rddata[31],
		gen_qspi_mem_rddata[16], gen_qspi_mem_rddata[17], gen_qspi_mem_rddata[18], gen_qspi_mem_rddata[19], gen_qspi_mem_rddata[20], gen_qspi_mem_rddata[21], gen_qspi_mem_rddata[22], gen_qspi_mem_rddata[23],
		gen_qspi_mem_rddata[ 8], gen_qspi_mem_rddata[ 9], gen_qspi_mem_rddata[10], gen_qspi_mem_rddata[11], gen_qspi_mem_rddata[12], gen_qspi_mem_rddata[13], gen_qspi_mem_rddata[14], gen_qspi_mem_rddata[15],
		gen_qspi_mem_rddata[ 0], gen_qspi_mem_rddata[ 1], gen_qspi_mem_rddata[ 2], gen_qspi_mem_rddata[ 3], gen_qspi_mem_rddata[ 4], gen_qspi_mem_rddata[ 5], gen_qspi_mem_rddata[ 6], gen_qspi_mem_rddata[ 7]
	};
    assign avl_mem_rddata_valid     = gen_qspi_mem_rddata_valid;
    assign avl_mem_waitrequest      = gen_qspi_mem_waitrequest;

endmodule
