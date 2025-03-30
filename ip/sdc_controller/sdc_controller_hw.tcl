# TCL File Generated by Component Editor 18.0
# Mon Sep 03 12:39:26 EEST 2018
# DO NOT MODIFY


# 
# sdc_controller "sdc_controller" v0
#  2018.09.03.12:39:26
# SD Card Controller
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module sdc_controller
# 
set_module_property DESCRIPTION "SD Card Controller"
set_module_property NAME sdc_controller
#set_module_property VERSION 18.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP "Interface Protocols/Serial"
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME sdc_controller
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL sdc_controller_top
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file bistable_domain_cross.v VERILOG PATH SD-card-controller/rtl/verilog/bistable_domain_cross.v
add_fileset_file byte_en_reg.v VERILOG PATH SD-card-controller/rtl/verilog/byte_en_reg.v
add_fileset_file edge_detect.v VERILOG PATH SD-card-controller/rtl/verilog/edge_detect.v
add_fileset_file generic_dpram.v VERILOG PATH config/generic_dpram.v
add_fileset_file generic_fifo_dc_gray.v VERILOG PATH config/generic_fifo_dc_gray.v
add_fileset_file monostable_domain_cross.v VERILOG PATH SD-card-controller/rtl/verilog/monostable_domain_cross.v
add_fileset_file sd_clock_divider.v VERILOG PATH SD-card-controller/rtl/verilog/sd_clock_divider.v
add_fileset_file sd_cmd_master.v VERILOG PATH SD-card-controller/rtl/verilog/sd_cmd_master.v
add_fileset_file sd_cmd_serial_host.v VERILOG PATH SD-card-controller/rtl/verilog/sd_cmd_serial_host.v
add_fileset_file sd_controller_wb.v VERILOG PATH SD-card-controller/rtl/verilog/sd_controller_wb.v
add_fileset_file sd_crc_16.v VERILOG PATH SD-card-controller/rtl/verilog/sd_crc_16.v
add_fileset_file sd_crc_7.v VERILOG PATH SD-card-controller/rtl/verilog/sd_crc_7.v
add_fileset_file sd_data_master.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_master.v
add_fileset_file sd_data_serial_host.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_serial_host.v
add_fileset_file sd_data_xfer_trig.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_xfer_trig.v
add_fileset_file sd_defines.h OTHER PATH SD-card-controller/rtl/verilog/sd_defines.h
add_fileset_file sd_fifo_filler.v VERILOG PATH SD-card-controller/rtl/verilog/sd_fifo_filler.v
add_fileset_file sd_wb_sel_ctrl.v VERILOG PATH SD-card-controller/rtl/verilog/sd_wb_sel_ctrl.v
add_fileset_file sdc_controller.v VERILOG PATH SD-card-controller/rtl/verilog/sdc_controller.v
add_fileset_file sdc_controller_top.v VERILOG PATH sdc_controller_top.v

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE false
set_fileset_property SIM_VERILOG TOP_LEVEL sdc_controller_top
add_fileset_file bistable_domain_cross.v VERILOG PATH SD-card-controller/rtl/verilog/bistable_domain_cross.v
add_fileset_file byte_en_reg.v VERILOG PATH SD-card-controller/rtl/verilog/byte_en_reg.v
add_fileset_file edge_detect.v VERILOG PATH SD-card-controller/rtl/verilog/edge_detect.v
add_fileset_file generic_dpram.v VERILOG PATH config/generic_dpram.v
add_fileset_file generic_fifo_dc_gray.v VERILOG PATH config/generic_fifo_dc_gray.v
add_fileset_file monostable_domain_cross.v VERILOG PATH SD-card-controller/rtl/verilog/monostable_domain_cross.v
add_fileset_file sd_clock_divider.v VERILOG PATH SD-card-controller/rtl/verilog/sd_clock_divider.v
add_fileset_file sd_cmd_master.v VERILOG PATH SD-card-controller/rtl/verilog/sd_cmd_master.v
add_fileset_file sd_cmd_serial_host.v VERILOG PATH SD-card-controller/rtl/verilog/sd_cmd_serial_host.v
add_fileset_file sd_controller_wb.v VERILOG PATH SD-card-controller/rtl/verilog/sd_controller_wb.v
add_fileset_file sd_crc_16.v VERILOG PATH SD-card-controller/rtl/verilog/sd_crc_16.v
add_fileset_file sd_crc_7.v VERILOG PATH SD-card-controller/rtl/verilog/sd_crc_7.v
add_fileset_file sd_data_master.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_master.v
add_fileset_file sd_data_serial_host.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_serial_host.v
add_fileset_file sd_data_xfer_trig.v VERILOG PATH SD-card-controller/rtl/verilog/sd_data_xfer_trig.v
add_fileset_file sd_defines.h OTHER PATH SD-card-controller/rtl/verilog/sd_defines.h
add_fileset_file sd_fifo_filler.v VERILOG PATH SD-card-controller/rtl/verilog/sd_fifo_filler.v
add_fileset_file sd_wb_sel_ctrl.v VERILOG PATH SD-card-controller/rtl/verilog/sd_wb_sel_ctrl.v
add_fileset_file sdc_controller.v VERILOG PATH SD-card-controller/rtl/verilog/sdc_controller.v
add_fileset_file sdc_controller_top.v VERILOG PATH sdc_controller_top.v


# 
# parameters
# 


# 
# display items
# 


# 
# connection point clock_sink
# 
add_interface clock_sink clock end
set_interface_property clock_sink clockRate 0
set_interface_property clock_sink ENABLED true
set_interface_property clock_sink EXPORT_OF ""
set_interface_property clock_sink PORT_NAME_MAP ""
set_interface_property clock_sink CMSIS_SVD_VARIABLES ""
set_interface_property clock_sink SVD_ADDRESS_GROUP ""

add_interface_port clock_sink clk_i clk Input 1


# 
# connection point reset_sink
# 
add_interface reset_sink reset end
set_interface_property reset_sink associatedClock clock_sink
set_interface_property reset_sink synchronousEdges DEASSERT
set_interface_property reset_sink ENABLED true
set_interface_property reset_sink EXPORT_OF ""
set_interface_property reset_sink PORT_NAME_MAP ""
set_interface_property reset_sink CMSIS_SVD_VARIABLES ""
set_interface_property reset_sink SVD_ADDRESS_GROUP ""

add_interface_port reset_sink rst_i reset Input 1


# 
# connection point avalon_s
# 
add_interface avalon_s avalon end
set_interface_property avalon_s addressUnits SYMBOLS
set_interface_property avalon_s associatedClock clock_sink
set_interface_property avalon_s associatedReset reset_sink
set_interface_property avalon_s bitsPerSymbol 8
set_interface_property avalon_s burstOnBurstBoundariesOnly false
set_interface_property avalon_s burstcountUnits WORDS
set_interface_property avalon_s explicitAddressSpan 0
set_interface_property avalon_s holdTime 0
set_interface_property avalon_s linewrapBursts false
set_interface_property avalon_s maximumPendingReadTransactions 0
set_interface_property avalon_s maximumPendingWriteTransactions 0
set_interface_property avalon_s readLatency 0
set_interface_property avalon_s readWaitTime 1
set_interface_property avalon_s setupTime 0
set_interface_property avalon_s timingUnits Cycles
set_interface_property avalon_s writeWaitTime 0
set_interface_property avalon_s ENABLED true
set_interface_property avalon_s EXPORT_OF ""
set_interface_property avalon_s PORT_NAME_MAP ""
set_interface_property avalon_s CMSIS_SVD_VARIABLES ""
set_interface_property avalon_s SVD_ADDRESS_GROUP ""

add_interface_port avalon_s avalon_s_address address Input 8
add_interface_port avalon_s avalon_s_writedata writedata Input 32
add_interface_port avalon_s avalon_s_readdata readdata Output 32
add_interface_port avalon_s avalon_s_byteenable byteenable Input 4
add_interface_port avalon_s avalon_s_write write Input 1
add_interface_port avalon_s avalon_s_read read Input 1
add_interface_port avalon_s avalon_s_chipselect chipselect Input 1
add_interface_port avalon_s avalon_s_waitrequest_n waitrequest_n Output 1
set_interface_assignment avalon_s embeddedsw.configuration.isFlash 0
set_interface_assignment avalon_s embeddedsw.configuration.isMemoryDevice 0
set_interface_assignment avalon_s embeddedsw.configuration.isNonVolatileStorage 0
set_interface_assignment avalon_s embeddedsw.configuration.isPrintableDevice 0


# 
# connection point avalon_m
# 
add_interface avalon_m avalon start
set_interface_property avalon_m addressUnits SYMBOLS
set_interface_property avalon_m associatedClock clock_sink
set_interface_property avalon_m associatedReset reset_sink
set_interface_property avalon_m bitsPerSymbol 8
set_interface_property avalon_m burstOnBurstBoundariesOnly false
set_interface_property avalon_m burstcountUnits WORDS
set_interface_property avalon_m doStreamReads false
set_interface_property avalon_m doStreamWrites false
set_interface_property avalon_m holdTime 0
set_interface_property avalon_m linewrapBursts false
set_interface_property avalon_m maximumPendingReadTransactions 0
set_interface_property avalon_m maximumPendingWriteTransactions 0
set_interface_property avalon_m readLatency 0
set_interface_property avalon_m readWaitTime 1
set_interface_property avalon_m setupTime 0
set_interface_property avalon_m timingUnits Cycles
set_interface_property avalon_m writeWaitTime 0
set_interface_property avalon_m ENABLED true
set_interface_property avalon_m EXPORT_OF ""
set_interface_property avalon_m PORT_NAME_MAP ""
set_interface_property avalon_m CMSIS_SVD_VARIABLES ""
set_interface_property avalon_m SVD_ADDRESS_GROUP ""

add_interface_port avalon_m avalon_m_writedata writedata Output 32
add_interface_port avalon_m avalon_m_readdata readdata Input 32
add_interface_port avalon_m avalon_m_address address Output 32
add_interface_port avalon_m avalon_m_byteenable byteenable Output 4
add_interface_port avalon_m avalon_m_write write Output 1
add_interface_port avalon_m avalon_m_read read Output 1
add_interface_port avalon_m avalon_m_chipselect chipselect Output 1
add_interface_port avalon_m avalon_m_waitrequest_n waitrequest_n Input 1


#
# connection point sd_clk_i
#
add_interface sd_clk_i clock end
set_interface_property sd_clk_i clockRate 0
set_interface_property sd_clk_i ENABLED true
set_interface_property sd_clk_i EXPORT_OF ""
set_interface_property sd_clk_i PORT_NAME_MAP ""
set_interface_property sd_clk_i CMSIS_SVD_VARIABLES ""
set_interface_property sd_clk_i SVD_ADDRESS_GROUP ""

add_interface_port sd_clk_i sd_clk_i_pad clk Input 1


#
# connection point sd_clk_o
#
add_interface sd_clk_o clock start
add_interface_port sd_clk_o sd_clk_o_pad clk Output 1


#
# connection point sd
# 
add_interface sd conduit end
set_interface_property sd associatedClock "sd_clk_o"
set_interface_property sd associatedReset ""
set_interface_property sd ENABLED true
set_interface_property sd EXPORT_OF ""
set_interface_property sd PORT_NAME_MAP ""
set_interface_property sd CMSIS_SVD_VARIABLES ""
set_interface_property sd SVD_ADDRESS_GROUP ""

add_interface_port sd sd_cmd_dat_i sd_cmd_dat_i Input 1
add_interface_port sd sd_cmd_out_o sd_cmd_out_o Output 1
add_interface_port sd sd_cmd_oe_o sd_cmd_oe_o Output 1
add_interface_port sd sd_dat_dat_i sd_dat_dat_i Input 4
add_interface_port sd sd_dat_out_o sd_dat_out_o Output 4
add_interface_port sd sd_dat_oe_o sd_dat_oe_o Output 1


# 
# connection point int_cmd
# 
add_interface int_cmd interrupt end
set_interface_property int_cmd associatedAddressablePoint ""
set_interface_property int_cmd associatedClock clock_sink
set_interface_property int_cmd bridgedReceiverOffset ""
set_interface_property int_cmd bridgesToReceiver ""
set_interface_property int_cmd ENABLED true
set_interface_property int_cmd EXPORT_OF ""
set_interface_property int_cmd PORT_NAME_MAP ""
set_interface_property int_cmd CMSIS_SVD_VARIABLES ""
set_interface_property int_cmd SVD_ADDRESS_GROUP ""

add_interface_port int_cmd int_cmd irq Output 1


# 
# connection point int_data
# 
add_interface int_data interrupt end
set_interface_property int_data associatedAddressablePoint ""
set_interface_property int_data associatedClock clock_sink
set_interface_property int_data bridgedReceiverOffset ""
set_interface_property int_data bridgesToReceiver ""
set_interface_property int_data ENABLED true
set_interface_property int_data EXPORT_OF ""
set_interface_property int_data PORT_NAME_MAP ""
set_interface_property int_data CMSIS_SVD_VARIABLES ""
set_interface_property int_data SVD_ADDRESS_GROUP ""

add_interface_port int_data int_data irq Output 1
