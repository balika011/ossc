# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1

# 
# module
# 
set_module_property DESCRIPTION "OSD generator"
set_module_property NAME osd_generator
#set_module_property VERSION 18.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP "Processors and Peripherals"
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME osd_generator
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false

# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL osd_generator_top
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file osd_generator_top.sv VERILOG PATH osd_generator_top.sv

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE false
set_fileset_property SIM_VERILOG TOP_LEVEL osd_generator_top
add_fileset_file osd_generator_top.sv VERILOG PATH osd_generator_top.sv

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
set_interface_property avalon_s addressUnits WORDS
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
# connection point osd_clk
#
add_interface osd_clk clock end
set_interface_property osd_clk ENABLED true
set_interface_property osd_clk EXPORT_OF ""
set_interface_property osd_clk PORT_NAME_MAP ""
set_interface_property osd_clk CMSIS_SVD_VARIABLES ""
set_interface_property osd_clk SVD_ADDRESS_GROUP ""

add_interface_port osd_clk vclk clk Input 1


#
# connection point avalon_m
#
add_interface avalon_m avalon start
set_interface_property avalon_m addressUnits SYMBOLS
set_interface_property avalon_m associatedClock osd_clk
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
add_interface_port avalon_m avalon_m_address address Output 32
add_interface_port avalon_m avalon_m_readdata readdata Input 32
add_interface_port avalon_m avalon_m_read read Output 1
add_interface_port avalon_m avalon_m_readdatavalid readdatavalid Input 1
add_interface_port avalon_m avalon_m_waitrequest waitrequest Input 1
add_interface_port avalon_m avalon_m_write write Output 1
add_interface_port avalon_m avalon_m_byteenable byteenable Output 4
add_interface_port avalon_m avalon_m_writedata writedata Output 32
add_interface_port avalon_m avalon_m_response response Input 2
add_interface_port avalon_m avalon_m_writeresponsevalid writeresponsevalid Input 1


# 
# connection point osd_in
# 
add_interface osd_in conduit end
set_interface_property osd_in associatedClock ""
set_interface_property osd_in associatedReset ""
set_interface_property osd_in ENABLED true
set_interface_property osd_in EXPORT_OF ""
set_interface_property osd_in PORT_NAME_MAP ""
set_interface_property osd_in CMSIS_SVD_VARIABLES ""
set_interface_property osd_in SVD_ADDRESS_GROUP ""

add_interface_port osd_in x x Input 12
add_interface_port osd_in y y Input 11


# 
# connection point osd_out
# 
add_interface osd_out conduit end
set_interface_property osd_out associatedClock ""
set_interface_property osd_out associatedReset ""
set_interface_property osd_out ENABLED true
set_interface_property osd_out EXPORT_OF ""
set_interface_property osd_out PORT_NAME_MAP ""
set_interface_property osd_out CMSIS_SVD_VARIABLES ""
set_interface_property osd_out SVD_ADDRESS_GROUP ""

add_interface_port osd_out osd_alpha alpha Output 8
add_interface_port osd_out osd_red red Output 8
add_interface_port osd_out osd_green green Output 8
add_interface_port osd_out osd_blue blue Output 8
