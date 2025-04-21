# (C) 2001-2025 Altera Corporation. All rights reserved.
# Your use of Altera Corporation's design tools, logic functions and other 
# software and tools, and its AMPP partner logic functions, and any output 
# files from any of the foregoing (including device programming or simulation 
# files), and any associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License Subscription 
# Agreement, Altera IP License Agreement, or other applicable 
# license agreement, including, without limitation, that your use is for the 
# sole purpose of programming logic devices manufactured by Altera and sold by 
# Altera or its authorized distributors.  Please refer to the applicable 
# agreement for further details.


# request TCL package
package require -exact qsys 16.0
package require -exact altera_terp 1.0

# +-----------------------------------
# | module ALTASMI_PARALLEL
# +-----------------------------------
set_module_property NAME                            intel_generic_serial_flash_interface_top_swapped
set_module_property AUTHOR                          "Intel Corporation"
set_module_property DESCRIPTION                     " The Generic Serial Flash Interface Intel FPGA IP core provides access to Serial \
													  Peripheral Interface (SPI) flash devices"
set_module_property DISPLAY_NAME                    "Generic Serial Flash Interface Intel FPGA IP"
set_module_property EDITABLE                        false
set_module_property VERSION                         24.1
set_module_property GROUP                           "Basic Functions/Configuration and Programming"
set_module_property INSTANTIATE_IN_SYSTEM_MODULE    true
set_module_property INTERNAL                        false

set_module_property     VALIDATION_CALLBACK     validation
set_module_property     COMPOSITION_CALLBACK    compose

add_display_item "" "General" GROUP tab
add_display_item "" "Simulation" GROUP tab
#add_display_item "" "Default Settings" GROUP tab

# Source files
source intel_generic_serial_flash_interface_top_swapped_hw_proc.tcl

# +-----------------------------------
# | device family info
# +-----------------------------------
set all_supported_device_families_list {"Max 10" "Arria 10" "Cyclone V" "Arria V GZ" "Arria V" "Stratix V" "Stratix IV" \
                                            "Cyclone IV GX" "Cyclone IV E" "Cyclone III GL" "Arria II GZ" "Arria II GX" "Cyclone 10 GX" "Cyclone 10 LP" "Stratix 10"}
                                    
proc check_device_ini {device_families_list}     {
    set enable_max10    [get_quartus_ini enable_max10_active_serial ENABLED]
  
    if {$enable_max10 == 1} {
        lappend device_families_list    "MAX 10 FPGA"
     } 
    return $device_families_list
}

set device_list    [check_device_ini $all_supported_device_families_list]
set_module_property SUPPORTED_DEVICE_FAMILIES    $device_list

add_parameter           DEVICE_FAMILY   STRING
set_parameter_property  DEVICE_FAMILY   SYSTEM_INFO     {DEVICE_FAMILY}
set_parameter_property  DEVICE_FAMILY   VISIBLE         false
set_parameter_property  DEVICE_FAMILY       HDL_PARAMETER   true

add_parameter           INTENDED_DEVICE_FAMILY  STRING
set_parameter_property  INTENDED_DEVICE_FAMILY  SYSTEM_INFO     {DEVICE_FAMILY}
set_parameter_property  INTENDED_DEVICE_FAMILY  VISIBLE         false


# +-----------------------------------
# | Parameters - General tab
# +-----------------------------------
add_parameter DEVICE_DENSITY INTEGER 128
set_parameter_property DEVICE_DENSITY DEFAULT_VALUE 128
set_parameter_property DEVICE_DENSITY DISPLAY_NAME "Device Density (Mb)"
set_parameter_property DEVICE_DENSITY DERIVED false
set_parameter_property DEVICE_DENSITY TYPE INTEGER
set_parameter_property DEVICE_DENSITY VISIBLE true
set_parameter_property DEVICE_DENSITY UNITS None
set_parameter_property DEVICE_DENSITY ALLOWED_RANGES {1 2 4 8 16 32 64 128 256 512 1024 2048}       
set_parameter_property DEVICE_DENSITY HDL_PARAMETER false
add_display_item        "General"       DEVICE_DENSITY       parameter

# use asmiblock - currently is invert. when check means not using ASMIBLOCK, so a bit confusing. Need to change the parameter name when updating presets.
add_parameter           gui_use_asmiblock               BOOLEAN             0
set_parameter_property  gui_use_asmiblock               DISPLAY_NAME        "Disable dedicated Active Serial interface"
set_parameter_property  gui_use_asmiblock               DESCRIPTION         "Check to route ASMIBLOCK signals to top level of design"
set_parameter_property  gui_use_asmiblock               AFFECTS_GENERATION  true
add_display_item        "General"                       gui_use_asmiblock       parameter

# use gpio
add_parameter           gui_use_gpio    BOOLEAN             0
set_parameter_property  gui_use_gpio    DISPLAY_NAME        "Enable SPI pins interface"
set_parameter_property  gui_use_gpio    DESCRIPTION         "Check to translate ASMIBLOCK signals to SPI pins interface"
set_parameter_property  gui_use_gpio    AFFECTS_GENERATION  true
add_display_item        "General"       gui_use_gpio       parameter

# enable csr byteenable 
add_parameter           gui_use_csr_byteenable          BOOLEAN                     0
set_parameter_property  gui_use_csr_byteenable          DISPLAY_NAME                "Use byteenable for CSR"
set_parameter_property  gui_use_csr_byteenable          DESCRIPTION                 "Check to turn on byteenable for CSR writedata interface"
set_parameter_property  gui_use_csr_byteenable          AFFECTS_GENERATION          true 
add_display_item        "General"                       gui_use_csr_byteenable      parameter
# +-----------------------------------
# | Parameters - Simulation tab
# +-----------------------------------

# enable_sim
add_parameter           ENABLE_SIM_MODEL  BOOLEAN             0
set_parameter_property  ENABLE_SIM_MODEL  DISPLAY_NAME        "Enable simulation"
set_parameter_property  ENABLE_SIM_MODEL  DESCRIPTION         "Check to enable simulation for Active Serial pins"
set_parameter_property  ENABLE_SIM_MODEL  AFFECTS_GENERATION  true
add_display_item        "Simulation"    ENABLE_SIM_MODEL      parameter

add_parameter ADDR_WIDTH INTEGER 19
set_parameter_property ADDR_WIDTH DEFAULT_VALUE 19
set_parameter_property ADDR_WIDTH DISPLAY_NAME ADDR_WIDTH
set_parameter_property ADDR_WIDTH DERIVED true
set_parameter_property ADDR_WIDTH TYPE INTEGER
set_parameter_property ADDR_WIDTH VISIBLE true
set_parameter_property ADDR_WIDTH UNITS None
set_parameter_property ADDR_WIDTH ALLOWED_RANGES {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28}
set_parameter_property ADDR_WIDTH HDL_PARAMETER false

add_parameter CHIP_SELS INTEGER "1"
set_parameter_property CHIP_SELS DISPLAY_NAME "Number of Chip Selects used"
set_parameter_property CHIP_SELS ALLOWED_RANGES {1 2 3}
set_parameter_property CHIP_SELS DESCRIPTION "Number of EPCQ(L) devices that are attached and need a CHIPSEL"
set_parameter_property CHIP_SELS HDL_PARAMETER true
set_parameter_property CHIP_SELS AFFECTS_GENERATION true
add_display_item        "General"    CHIP_SELS       parameter

add_parameter 			DEFAULT_VALUE_REG_0 INTEGER 0x00000001
set_parameter_property 	DEFAULT_VALUE_REG_0 DISPLAY_NAME {Control Register}
set_parameter_property 	DEFAULT_VALUE_REG_0 AFFECTS_ELABORATION true
set_parameter_property 	DEFAULT_VALUE_REG_0 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_0 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_0 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_1 INTEGER 0x00000010
set_parameter_property 	DEFAULT_VALUE_REG_1 DISPLAY_NAME {Clock Baudrate Register}
set_parameter_property 	DEFAULT_VALUE_REG_1 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_1 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_1 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_2 INTEGER 0x00000000
set_parameter_property 	DEFAULT_VALUE_REG_2 DISPLAY_NAME {Chip Select Delay Register}
set_parameter_property 	DEFAULT_VALUE_REG_2 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_2 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_2 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_3 INTEGER 0x00000000
set_parameter_property 	DEFAULT_VALUE_REG_3 DISPLAY_NAME {Read Capturing Register}
set_parameter_property 	DEFAULT_VALUE_REG_3 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_3 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_3 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_4 INTEGER 0x00000000
set_parameter_property 	DEFAULT_VALUE_REG_4 DISPLAY_NAME {Protocol Settings Register}
set_parameter_property 	DEFAULT_VALUE_REG_4 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_4 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_4 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_5 INTEGER 0x00000003
set_parameter_property 	DEFAULT_VALUE_REG_5 DISPLAY_NAME {Read Instruction Register}
set_parameter_property 	DEFAULT_VALUE_REG_5 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_5 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_5 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_6 INTEGER 0x00000502
set_parameter_property 	DEFAULT_VALUE_REG_6 DISPLAY_NAME {Write Instruction Register}
set_parameter_property 	DEFAULT_VALUE_REG_6 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_6 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_6 DISPLAY_HINT "hexadecimal"

add_parameter 			DEFAULT_VALUE_REG_7 INTEGER 0x00001805
set_parameter_property 	DEFAULT_VALUE_REG_7 DISPLAY_NAME {Flash Command Setting Register}
set_parameter_property 	DEFAULT_VALUE_REG_7 HDL_PARAMETER false
set_parameter_property  DEFAULT_VALUE_REG_7 VISIBLE true
set_parameter_property 	DEFAULT_VALUE_REG_7 DISPLAY_HINT "hexadecimal"


add_display_item  "Default Settings"    DEFAULT_VALUE_REG_0  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_1  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_2  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_3  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_4  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_5  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_6  parameter
add_display_item  "Default Settings"    DEFAULT_VALUE_REG_7  parameter

# +-----------------------------------
# | Debug Parameters
# +----------------------------------- 
add_parameter           CHIP_SELECT_BYPASS  BOOLEAN             false
set_parameter_property  CHIP_SELECT_BYPASS  AFFECTS_GENERATION  true
set_parameter_property  CHIP_SELECT_BYPASS  AFFECTS_ELABORATION true
set_parameter_property  CHIP_SELECT_BYPASS  VISIBLE             false

add_parameter               USE_CHIP_SEL_FROM_CSR INTEGER 1
set_parameter_property      USE_CHIP_SEL_FROM_CSR DISPLAY_NAME "Use Chip Select From CSR"
set_parameter_property      USE_CHIP_SEL_FROM_CSR UNITS None
set_parameter_property      USE_CHIP_SEL_FROM_CSR ALLOWED_RANGES { "0: Use dedicated Chip Select for XIP" "1: Use Chip Select from CSR"}
set_parameter_property      USE_CHIP_SEL_FROM_CSR AFFECTS_ELABORATION true
set_parameter_property      USE_CHIP_SEL_FROM_CSR AFFECTS_GENERATION true
set_parameter_property      USE_CHIP_SEL_FROM_CSR DERIVED false
set_parameter_property      USE_CHIP_SEL_FROM_CSR HDL_PARAMETER false
set_parameter_property      USE_CHIP_SEL_FROM_CSR VISIBLE true
add_display_item        "General"   USE_CHIP_SEL_FROM_CSR       parameter


add_parameter               DEBUG_OPTION INTEGER 0
set_parameter_property      DEBUG_OPTION DISPLAY_NAME "Debug Options"
set_parameter_property      DEBUG_OPTION UNITS None
set_parameter_property      DEBUG_OPTION ALLOWED_RANGES { "0:None" "1:Two controllers only - 32 bits" "2:Two Controller and Command Generator - 8 bits"}
set_parameter_property      DEBUG_OPTION AFFECTS_ELABORATION true
set_parameter_property      DEBUG_OPTION AFFECTS_GENERATION true
set_parameter_property      DEBUG_OPTION DERIVED false
set_parameter_property      DEBUG_OPTION HDL_PARAMETER false
set_parameter_property  	DEBUG_OPTION VISIBLE true
add_display_item        "General"   DEBUG_OPTION       parameter

add_parameter               PIPE_CSR INTEGER 0
set_parameter_property      PIPE_CSR DISPLAY_NAME "Pipeline CSR output"
set_parameter_property      PIPE_CSR UNITS None
set_parameter_property      PIPE_CSR ALLOWED_RANGES { "0:None" "1:Pipeline"}
set_parameter_property      PIPE_CSR AFFECTS_ELABORATION true
set_parameter_property      PIPE_CSR AFFECTS_GENERATION true
set_parameter_property      PIPE_CSR DERIVED false
set_parameter_property      PIPE_CSR HDL_PARAMETER false
set_parameter_property  	PIPE_CSR VISIBLE true
add_display_item        "General"   PIPE_CSR       parameter

add_parameter               PIPE_XIP INTEGER 0
set_parameter_property      PIPE_XIP DISPLAY_NAME "Pipeline XIP output"
set_parameter_property      PIPE_XIP UNITS None
set_parameter_property      PIPE_XIP ALLOWED_RANGES { "0:None" "1:Pipeline"}
set_parameter_property      PIPE_XIP AFFECTS_ELABORATION true
set_parameter_property      PIPE_XIP AFFECTS_GENERATION true
set_parameter_property      PIPE_XIP DERIVED false
set_parameter_property      PIPE_XIP HDL_PARAMETER false
set_parameter_property      PIPE_XIP VISIBLE true
add_display_item        "General"   PIPE_XIP       parameter

add_parameter               PIPE_CMD_GEN_CMD INTEGER 0
set_parameter_property      PIPE_CMD_GEN_CMD DISPLAY_NAME "Pipeline Cmd Generator output"
set_parameter_property      PIPE_CMD_GEN_CMD UNITS None
set_parameter_property      PIPE_CMD_GEN_CMD ALLOWED_RANGES { "0:None" "1:Pipeline"}
set_parameter_property      PIPE_CMD_GEN_CMD AFFECTS_ELABORATION true
set_parameter_property      PIPE_CMD_GEN_CMD AFFECTS_GENERATION true
set_parameter_property      PIPE_CMD_GEN_CMD DERIVED false
set_parameter_property      PIPE_CMD_GEN_CMD HDL_PARAMETER false
set_parameter_property      PIPE_CMD_GEN_CMD VISIBLE true
add_display_item        "General"   PIPE_CMD_GEN_CMD       parameter

add_parameter               PIPE_MUX_CMD INTEGER 0
set_parameter_property      PIPE_MUX_CMD DISPLAY_NAME "Pipeline Mux Output"
set_parameter_property      PIPE_MUX_CMD UNITS None
set_parameter_property      PIPE_MUX_CMD ALLOWED_RANGES { "0:None" "1:Pipeline"}
set_parameter_property      PIPE_MUX_CMD AFFECTS_ELABORATION true
set_parameter_property      PIPE_MUX_CMD AFFECTS_GENERATION true
set_parameter_property      PIPE_MUX_CMD DERIVED false
set_parameter_property      PIPE_MUX_CMD HDL_PARAMETER false
set_parameter_property      PIPE_MUX_CMD VISIBLE true
add_display_item        "General"   PIPE_MUX_CMD       parameter




# Add documentation links for user guide and/or release notes
add_documentation_link "User Guide" https://www.intel.com/content/www/us/en/docs/programmable/683419/
add_documentation_link "Release Notes" https://www.intel.com/content/www/us/en/docs/programmable/683746/
