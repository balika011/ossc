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


# +-----------------------------------
# | Validate user selection during validation callback
# +-----------------------------------
proc validation {}  {

    set get_device_setting      [get_parameter_value DEVICE_FAMILY]
    set sim_setting             [get_parameter_value ENABLE_SIM_MODEL]

    # devices that supported QSPI - Quad/Dual data width, asmi_dataout, asmi_sdoin, asmi_dataoe
    set supported_QSPI_devices_list {"Arria 10" "Cyclone V" "Arria V GZ" "Arria V" "Stratix V" "Cyclone 10 GX"}
    
    # devices that supported simulation
    set supported_sim_devices_list {"Arria 10" "Cyclone V" "Arria V GZ" "Arria V" "Stratix V" "MAX 10 FPGA" "Cyclone 10 GX"}
    
    # --- checking for information --- #
    set is_qspi_devices_list    "false"
    set is_sim_devices_list     "false"
    set is_multi_flash_support  "false"

    # check whether devices supporting QSPI
    if {[check_device_family_equivalence $get_device_setting $supported_QSPI_devices_list]} {
        set is_qspi_devices_list    "true"
     }
    
    # check whether devices supporting simulation
    if {[check_device_family_equivalence $get_device_setting $supported_sim_devices_list]} {
        set is_sim_devices_list "true"
     }
    
    # --- ENABLE_SIM --- #
    #if {$is_sim_devices_list eq "false" && $sim_setting eq "true"} {
    #    send_message error "$get_device_setting does not support enable_sim parameter"
    #} 
       
    # check whether devices supporting multiple flash - only for Arria 10
    set CHIP_SELS               [ get_parameter_value CHIP_SELS]
    if {[check_device_family_equivalence $get_device_setting {"Arria 10"}] || [check_device_family_equivalence $get_device_setting {"Cyclone 10 GX"}]} {
        set is_multi_flash_support  "true"
    } else {
        set is_multi_flash_support  "false"
    }
}

proc compose { } {

    # --- check ini for hidden devices --- #
    set disable_asmiblock       [get_parameter_value gui_use_asmiblock]
    set use_gpio                [get_parameter_value gui_use_gpio]
    set use_csr_byteenable      [get_parameter_value gui_use_csr_byteenable]
    set chip_sels               [get_parameter_value CHIP_SELS]

    set debug_option            [get_parameter_value DEBUG_OPTION]
    set use_chip_sel_from_csr   [get_parameter_value USE_CHIP_SEL_FROM_CSR]
    set pipe_xip                [get_parameter_value PIPE_XIP]
    set pipe_csr                [get_parameter_value PIPE_CSR]
    set pipe_cmd_gen_cmd        [get_parameter_value PIPE_CMD_GEN_CMD]
    set pipe_mux_cmd            [get_parameter_value PIPE_MUX_CMD]
    set enable_sim_model        [get_parameter_value ENABLE_SIM_MODEL]
    set chip_select_bypass      [get_parameter_value CHIP_SELECT_BYPASS]
    set flash_density           [get_parameter_value DEVICE_DENSITY]

    # based on the density, find the correct address width for avl_mem
    set avl_mem_addr_width     [ proc_get_derive_addr_width $flash_density ]
    


    # Instances and instance parameters
    # (disabled instances are intentionally culled)
    add_instance csr_controller intel_generic_serial_flash_interface_csr 24.1
    # Apply those default setting register values to the csr controller
    for { set i 0 } { $i < 8 } { incr i } {
        set default_value_reg_($i) [get_parameter_value DEFAULT_VALUE_REG_${i}]
        #send_message info "$i --------integer   $default_value_reg_($i)"
        set default_value_reg_bin($i) [dec2bin $default_value_reg_($i) 32]
        #send_message info "$i --------bin value   $default_value_reg_bin($i)"
        # add 32'h to pass to the parameter
        set default_value_reg_bin_param($i)  32'b$default_value_reg_bin($i)
        #send_message info "$i --------bin param format   $default_value_reg_bin_param($i)"
        set_instance_parameter_value csr_controller DEFAULT_VALUE_REG_${i} $default_value_reg_bin_param($i)
    }

    add_instance xip_controller intel_generic_serial_flash_interface_xip 24.1

    add_instance xip_addr_adaption intel_generic_serial_flash_interface_addr_swapped 24.1
    # update the avl_mem address width
    set_instance_parameter_value xip_addr_adaption ADDR_WIDTH  $avl_mem_addr_width

    add_instance clk_bridge altera_clock_bridge 24.1
    set_instance_parameter_value clk_bridge {EXPLICIT_CLOCK_RATE} {0.0}
    set_instance_parameter_value clk_bridge {NUM_CLOCK_OUTPUTS} {1}

    add_instance merlin_demultiplexer_0 altera_merlin_demultiplexer 24.1
    set_instance_parameter_value merlin_demultiplexer_0 {ST_DATA_W} {32}
    set_instance_parameter_value merlin_demultiplexer_0 {ST_CHANNEL_W} {2}
    set_instance_parameter_value merlin_demultiplexer_0 {NUM_OUTPUTS} {2}
    set_instance_parameter_value merlin_demultiplexer_0 {VALID_WIDTH} {1}
    set_instance_parameter_value merlin_demultiplexer_0 {MERLIN_PACKET_FORMAT} {}

    add_instance multiplexer altera_merlin_multiplexer 24.1
    set_instance_parameter_value multiplexer {ST_DATA_W} {32}
    set_instance_parameter_value multiplexer {ST_CHANNEL_W} {2}
    set_instance_parameter_value multiplexer {NUM_INPUTS} {2}
    set_instance_parameter_value multiplexer {PIPELINE_ARB} {1}
    set_instance_parameter_value multiplexer {USE_EXTERNAL_ARB} {0}
    set_instance_parameter_value multiplexer {PKT_TRANS_LOCK} {-1}
    set_instance_parameter_value multiplexer {ARBITRATION_SCHEME} {fixed-priority}
    set_instance_parameter_value multiplexer {MERLIN_PACKET_FORMAT} {}

    add_instance reset_bridge altera_reset_bridge 24.1
    set_instance_parameter_value reset_bridge {ACTIVE_LOW_RESET} {0}
    set_instance_parameter_value reset_bridge {SYNCHRONOUS_EDGES} {none}
    set_instance_parameter_value reset_bridge {NUM_RESET_OUTPUTS} {1}
    set_instance_parameter_value reset_bridge {USE_RESET_REQUEST} {0}

    # add in single stage st PIPELINE
    if {$pipe_csr == 1} {
        add_instance csr_ctrl_pipeline altera_avalon_st_pipeline_stage 24.1
        set_instance_parameter_value csr_ctrl_pipeline {SYMBOLS_PER_BEAT} {1}
        set_instance_parameter_value csr_ctrl_pipeline {BITS_PER_SYMBOL} {32}
        set_instance_parameter_value csr_ctrl_pipeline {USE_PACKETS} {1}
        set_instance_parameter_value csr_ctrl_pipeline {USE_EMPTY} {0}
        set_instance_parameter_value csr_ctrl_pipeline {CHANNEL_WIDTH} {2}
        set_instance_parameter_value csr_ctrl_pipeline {MAX_CHANNEL} {0}
        set_instance_parameter_value csr_ctrl_pipeline {ERROR_WIDTH} {0}
        set_instance_parameter_value csr_ctrl_pipeline {PIPELINE_READY} {1}
        # connections and connection parameters - st pipeline, 
        # put it in front of the xip xip_controller
        add_connection clk_bridge.out_clk csr_ctrl_pipeline.cr0 clock
        add_connection reset_bridge.out_reset csr_ctrl_pipeline.cr0_reset reset
        # route the output of the xip controller to this pipeline stage
        add_connection csr_controller.cmd_pck csr_ctrl_pipeline.sink0 avalon_streaming
        add_connection csr_ctrl_pipeline.source0  multiplexer.sink1 avalon_streaming 
        # connections and connection parameters - csr_controller
        #add_connection csr_controller.cmd_pck multiplexer.sink0 avalon_streaming
    } else {
        add_connection csr_controller.cmd_pck multiplexer.sink1 avalon_streaming
    }

    if {$pipe_xip == 1} {
        add_instance xip_ctrl_pipeline altera_avalon_st_pipeline_stage 24.1
        set_instance_parameter_value xip_ctrl_pipeline {SYMBOLS_PER_BEAT} {1}
        set_instance_parameter_value xip_ctrl_pipeline {BITS_PER_SYMBOL} {32}
        set_instance_parameter_value xip_ctrl_pipeline {USE_PACKETS} {1}
        set_instance_parameter_value xip_ctrl_pipeline {USE_EMPTY} {0}
        set_instance_parameter_value xip_ctrl_pipeline {CHANNEL_WIDTH} {2}
        set_instance_parameter_value xip_ctrl_pipeline {MAX_CHANNEL} {0}
        set_instance_parameter_value xip_ctrl_pipeline {ERROR_WIDTH} {0}
        set_instance_parameter_value xip_ctrl_pipeline {PIPELINE_READY} {1}
        # connections and connection parameters - st pipeline, 
        # put it in front of the xip xip_controller
        add_connection clk_bridge.out_clk xip_ctrl_pipeline.cr0 clock
        add_connection reset_bridge.out_reset xip_ctrl_pipeline.cr0_reset reset
        # route the output of the xip controller to this pipeline stage
        add_connection xip_controller.cmd_pck xip_ctrl_pipeline.sink0 avalon_streaming
        add_connection xip_ctrl_pipeline.source0  multiplexer.sink0 avalon_streaming 
        # connections and connection parameters - csr_controller
        #add_connection csr_controller.cmd_pck multiplexer.sink0 avalon_streaming
    } else {
        add_connection xip_controller.cmd_pck multiplexer.sink0 avalon_streaming
    }

    add_connection clk_bridge.out_clk csr_controller.clk clock
    add_connection reset_bridge.out_reset csr_controller.reset reset
    add_connection merlin_demultiplexer_0.src1 csr_controller.rsp_pck avalon_streaming
    add_interface avl_csr avalon slave
    set_interface_property avl_csr EXPORT_OF csr_controller.csr

    # connections and connection parameters - xip_controller
    add_connection clk_bridge.out_clk xip_controller.clk clock
    add_connection reset_bridge.out_reset xip_controller.reset reset
    # address adaption for XIP controller, it uses 32 bits word unit
    add_connection clk_bridge.out_clk xip_addr_adaption.clock_sink clock
    add_connection reset_bridge.out_reset xip_addr_adaption.reset reset
    add_connection merlin_demultiplexer_0.src0 xip_controller.rsp_pck avalon_streaming
    add_interface avl_mem avalon slave
    #set_interface_property avl_mem EXPORT_OF xip_controller.mem
    set_interface_property avl_mem EXPORT_OF xip_addr_adaption.avl_mem 
    add_connection xip_addr_adaption.gen_qspi_mem xip_controller.mem avalon

    # +-------------------------------------
    # | Add settings needed by Nios tools
    # +-------------------------------------
    # Tells us component is a flash 
    set_module_assignment embeddedsw.memoryInfo.IS_FLASH 1
    
    # interface assignments for embedded software
    #set_interface_assignment avl_mem embeddedsw.configuration.isFlash 1
    #set_interface_assignment avl_mem embeddedsw.configuration.isMemoryDevice 1
    #set_interface_assignment avl_mem embeddedsw.configuration.isNonVolatileStorage 1
    #set_interface_assignment avl_mem embeddedsw.configuration.isPrintableDevice 0
    
    # These assignments tells tools to create byte-addressed .hex files only
    set_module_assignment embeddedsw.memoryInfo.GENERATE_HEX 1
    set_module_assignment embeddedsw.memoryInfo.USE_BYTE_ADDRESSING_FOR_HEX 1
    set_module_assignment embeddedsw.memoryInfo.GENERATE_DAT_SYM 0
    set_module_assignment embeddedsw.memoryInfo.GENERATE_FLASH 0
    
    # Width of memory
    set_module_assignment embeddedsw.memoryInfo.MEM_INIT_DATA_WIDTH 32
    
    # Output directories for programming files
    #set_module_assignment embeddedsw.memoryInfo.DAT_SYM_INSTALL_DIR {SIM_DIR}
    #set_module_assignment embeddedsw.memoryInfo.FLASH_INSTALL_DIR {APP_DIR}
    set_module_assignment embeddedsw.memoryInfo.HEX_INSTALL_DIR {QPF_DIR}
    
    # Module assignments related to names of simulation files
    #set_module_assignment postgeneration.simulation.init_file.param_name {INIT_FILENAME}
    #set_module_assignment postgeneration.simulation.init_file.type {MEM_INIT}

    # connections and connection parameters - demux and mux
    add_connection clk_bridge.out_clk multiplexer.clk clock
    add_connection clk_bridge.out_clk merlin_demultiplexer_0.clk clock
    add_connection reset_bridge.out_reset multiplexer.clk_reset reset
    add_connection reset_bridge.out_reset merlin_demultiplexer_0.clk_reset reset

    # exported interfaces

    add_interface clk clock sink
    set_interface_property clk EXPORT_OF clk_bridge.in_clk
    add_interface reset reset sink
    set_interface_property reset EXPORT_OF reset_bridge.in_reset

    # In case ASMI 2, the chip select is from the CSR< user should use csr comnponent to select the devices
    # then XIP controller will talk to that device, 
    # In case QSPI 2, the want the XIP to have dedicated chip select, since they embeded the chip select into the rd_wr_byte_addressing
    # So base on this, set correct value chip select enbable for the two CSR and XIP
    if {$use_chip_sel_from_csr == 1} {
        set_instance_parameter_value csr_controller {CHIP_SELECT_EN} {1}
        set_instance_parameter_value xip_controller {CHIP_SELECT_EN} {1}
        # connect chip select from the CSR to XIP controller
        add_connection csr_controller.chip_select xip_controller.chip_select conduit
    } else {
        set_instance_parameter_value csr_controller {CHIP_SELECT_EN} {0}
        set_instance_parameter_value xip_controller {CHIP_SELECT_EN} {1}
        add_interface chip_select conduit end
        set_interface_property chip_select EXPORT_OF xip_controller.chip_select
    }
    # Generic QSPI, for ASMI backward, allow export chip select input signal, which overwrite the value from
    # the control register
    if {$chip_select_bypass} {
        set_instance_parameter_value csr_controller CHIP_SELECT_BYPASS 1
        add_interface in_chip_select conduit end
        set_interface_property in_chip_select EXPORT_OF csr_controller.in_chip_select
    } else {
        set_instance_parameter_value csr_controller CHIP_SELECT_BYPASS 0
    }
    
    # If user enables CSR byteenable from gui, add csr_byteenable port , if not terminate it 
    if {$use_csr_byteenable} {
        set_instance_parameter_value csr_controller {ENABLE_CSR_BYTEENABLE} {1}
    } else {
        set_instance_parameter_value csr_controller {ENABLE_CSR_BYTEENABLE} {0}
    }
switch $debug_option {
    0 {
        add_instance serial_flash_inf_cmd_gen_inst intel_generic_serial_flash_interface_cmd

        add_instance qspi_inf_inst intel_generic_serial_flash_interface_if_ctrl
        #set_instance_parameter_value qspi_inf_inst {DATA_WIDTH} $io_mode
        set_instance_parameter_value qspi_inf_inst {DISABLE_ASMIBLOCK} $disable_asmiblock
        set_instance_parameter_value qspi_inf_inst {USE_GPIO} $use_gpio
        set_instance_parameter_value qspi_inf_inst {NCS_NUM} $chip_sels
        set_instance_parameter_value qspi_inf_inst {ENABLE_SIM} $enable_sim_model
        
        # Connect some extra signals
        add_connection serial_flash_inf_cmd_gen_inst.addr_num_lines   qspi_inf_inst.addr_num_lines conduit
        add_connection serial_flash_inf_cmd_gen_inst.data_num_lines   qspi_inf_inst.data_num_lines conduit
        add_connection serial_flash_inf_cmd_gen_inst.op_num_lines     qspi_inf_inst.op_num_lines   conduit

        if { $use_gpio } {
            add_interface qspi_pins conduit end
            set_interface_property qspi_pins EXPORT_OF qspi_inf_inst.qspi_pins
        } elseif { $disable_asmiblock } {
            add_interface atom_ports conduit end
            set_interface_property atom_ports EXPORT_OF qspi_inf_inst.atom_ports
        }

        add_connection csr_controller.addr_bytes_csr serial_flash_inf_cmd_gen_inst.addr_bytes_csr conduit
        add_connection serial_flash_inf_cmd_gen_inst.addr_bytes_xip xip_controller.addr_bytes_xip conduit
        add_connection csr_controller.op_type serial_flash_inf_cmd_gen_inst.op_type conduit
        add_connection csr_controller.wr_addr_type serial_flash_inf_cmd_gen_inst.wr_addr_type conduit
        add_connection csr_controller.wr_data_type serial_flash_inf_cmd_gen_inst.wr_data_type conduit
        add_connection csr_controller.rd_addr_type serial_flash_inf_cmd_gen_inst.rd_addr_type conduit
        add_connection csr_controller.rd_data_type serial_flash_inf_cmd_gen_inst.rd_data_type conduit

        add_connection csr_controller.wr_en_opcode          xip_controller.wr_en_opcode             conduit 
        add_connection csr_controller.polling_opcode        xip_controller.polling_opcode           conduit 
        add_connection csr_controller.polling_bit           xip_controller.polling_bit              conduit 
        add_connection csr_controller.wr_opcode             xip_controller.wr_opcode                conduit 
        add_connection csr_controller.rd_opcode             xip_controller.rd_opcode                conduit 
        add_connection csr_controller.rd_dummy_cycles       xip_controller.rd_dummy_cycles          conduit 
        add_connection csr_controller.is_4bytes_addr_xip    xip_controller.is_4bytes_addr_xip       conduit 
        # some extra connection from the csr to the qspi interface
        add_connection csr_controller.baud_rate_divisor     qspi_inf_inst.baud_rate_divisor         conduit
        add_connection csr_controller.cs_delay_setting      qspi_inf_inst.cs_delay_setting          conduit
        add_connection csr_controller.read_capture_delay    qspi_inf_inst.read_capture_delay        conduit

        add_connection xip_controller.xip_trans_type serial_flash_inf_cmd_gen_inst.xip_trans_type conduit 
        

        add_connection clk_bridge.out_clk serial_flash_inf_cmd_gen_inst.clk clock
        add_connection reset_bridge.out_reset serial_flash_inf_cmd_gen_inst.reset reset
        add_connection serial_flash_inf_cmd_gen_inst.out_rsp_pck merlin_demultiplexer_0.sink avalon_streaming
        #add_connection multiplexer.src serial_flash_inf_cmd_gen_inst.in_cmd_pck avalon_streaming
        add_connection clk_bridge.out_clk qspi_inf_inst.clk clock
        add_connection reset_bridge.out_reset qspi_inf_inst.reset reset
        
        #add_connection serial_flash_inf_cmd_gen_inst.out_cmd_pck qspi_inf_inst.in_cmd_pck avalon_streaming
        add_connection qspi_inf_inst.out_rsp_pck serial_flash_inf_cmd_gen_inst.in_rsp_pck avalon_streaming

        add_connection serial_flash_inf_cmd_gen_inst.dummy_cycles qspi_inf_inst.dummy_cycles conduit
        add_connection serial_flash_inf_cmd_gen_inst.chip_select qspi_inf_inst.chip_select conduit
        add_connection serial_flash_inf_cmd_gen_inst.require_rdata qspi_inf_inst.require_rdata conduit
        add_connection csr_controller.qspi_interface_en qspi_inf_inst.qspi_interface_en conduit
        #add_interface rsp avalon_streaming sink
        #set_interface_property rsp EXPORT_OF serial_flash_inf_cmd_gen_inst.in_rsp_pck
        #add_interface cmd avalon_streaming source
        #set_interface_property cmd EXPORT_OF serial_flash_inf_cmd_gen_inst.out_cmd_pck

        # Need add in SPI interfac component
        if {$pipe_cmd_gen_cmd ==1} {
            add_instance cmd_gen_pipe_cmd altera_avalon_st_pipeline_stage 24.1
            set_instance_parameter_value cmd_gen_pipe_cmd {SYMBOLS_PER_BEAT} {1}
            set_instance_parameter_value cmd_gen_pipe_cmd {BITS_PER_SYMBOL} {8}
            set_instance_parameter_value cmd_gen_pipe_cmd {USE_PACKETS} {1}
            set_instance_parameter_value cmd_gen_pipe_cmd {USE_EMPTY} {0}
            set_instance_parameter_value cmd_gen_pipe_cmd {CHANNEL_WIDTH} {2}
            set_instance_parameter_value cmd_gen_pipe_cmd {MAX_CHANNEL} {0}
            set_instance_parameter_value cmd_gen_pipe_cmd {ERROR_WIDTH} {0}
            set_instance_parameter_value cmd_gen_pipe_cmd {PIPELINE_READY} {1}
            
            add_connection clk_bridge.out_clk cmd_gen_pipe_cmd.cr0 clock
            add_connection reset_bridge.out_reset cmd_gen_pipe_cmd.cr0_reset reset
            add_connection serial_flash_inf_cmd_gen_inst.out_cmd_pck cmd_gen_pipe_cmd.sink0 avalon_streaming
            add_connection cmd_gen_pipe_cmd.source0 qspi_inf_inst.in_cmd_pck avalon_streaming

        } else {
            add_connection serial_flash_inf_cmd_gen_inst.out_cmd_pck qspi_inf_inst.in_cmd_pck avalon_streaming
        }

        # pipeline output of the mux
        if {$pipe_mux_cmd ==1} {
            add_instance mux_output_pipe altera_avalon_st_pipeline_stage 24.1
            set_instance_parameter_value mux_output_pipe {SYMBOLS_PER_BEAT} {1}
            set_instance_parameter_value mux_output_pipe {BITS_PER_SYMBOL} {32}
            set_instance_parameter_value mux_output_pipe {USE_PACKETS} {1}
            set_instance_parameter_value mux_output_pipe {USE_EMPTY} {0}
            set_instance_parameter_value mux_output_pipe {CHANNEL_WIDTH} {2}
            set_instance_parameter_value mux_output_pipe {MAX_CHANNEL} {0}
            set_instance_parameter_value mux_output_pipe {ERROR_WIDTH} {0}
            set_instance_parameter_value mux_output_pipe {PIPELINE_READY} {1}
            
            add_connection clk_bridge.out_clk mux_output_pipe.cr0 clock
            add_connection reset_bridge.out_reset mux_output_pipe.cr0_reset reset
            add_connection multiplexer.src mux_output_pipe.sink0 avalon_streaming
            add_connection mux_output_pipe.source0 serial_flash_inf_cmd_gen_inst.in_cmd_pck avalon_streaming

        } else {
            add_connection multiplexer.src serial_flash_inf_cmd_gen_inst.in_cmd_pck avalon_streaming
        }

    }

    1 {
        add_interface addr_bytes_csr conduit end
        set_interface_property addr_bytes_csr EXPORT_OF csr_controller.addr_bytes_csr
        add_interface addr_bytes_xip conduit end
        set_interface_property addr_bytes_xip EXPORT_OF xip_controller.addr_bytes_xip
        add_interface rsp avalon_streaming sink
        set_interface_property rsp EXPORT_OF merlin_demultiplexer_0.sink
        add_interface cmd avalon_streaming source
        set_interface_property cmd EXPORT_OF multiplexer.src
        add_interface qspi_interface_en conduit end
        set_interface_property qspi_interface_en EXPORT_OF csr_controller.qspi_interface_en
    }
    2 {
        add_instance serial_flash_inf_cmd_gen_inst serial_flash_inf_cmd_gen 

        add_connection csr_controller.addr_bytes_csr serial_flash_inf_cmd_gen_inst.addr_bytes_csr conduit
        add_connection serial_flash_inf_cmd_gen_inst.addr_bytes_xip xip_controller.addr_bytes_xip conduit

        add_connection clk_bridge.out_clk serial_flash_inf_cmd_gen_inst.clk clock
        add_connection reset_bridge.out_reset serial_flash_inf_cmd_gen_inst.reset reset
        add_connection serial_flash_inf_cmd_gen_inst.out_rsp_pck merlin_demultiplexer_0.sink avalon_streaming
        add_connection multiplexer.src serial_flash_inf_cmd_gen_inst.in_cmd_pck avalon_streaming
        add_interface rsp avalon_streaming sink
        set_interface_property rsp EXPORT_OF serial_flash_inf_cmd_gen_inst.in_rsp_pck
        add_interface cmd avalon_streaming source
        set_interface_property cmd EXPORT_OF serial_flash_inf_cmd_gen_inst.out_cmd_pck
        add_interface qspi_interface_en conduit end
        set_interface_property qspi_interface_en EXPORT_OF csr_controller.qspi_interface_en
        add_interface dummy_cycles conduit end
        set_interface_property dummy_cycles EXPORT_OF serial_flash_inf_cmd_gen_inst.dummy_cycles
        add_interface require_rdata conduit end
        set_interface_property require_rdata EXPORT_OF serial_flash_inf_cmd_gen_inst.require_rdata
        add_interface chip_select conduit end
        set_interface_property chip_select EXPORT_OF serial_flash_inf_cmd_gen_inst.chip_select
        }
    }

}


proc dec2bin {i {width {}}} {
    #returns the binary representation of $i
    # width determines the length of the returned string (left truncated or added left 0)
    # use of width allows concatenation of bits sub-fields

    set res {}
    if {$i<0} {
        set sign -
        set i [expr {abs($i)}]
    } else {
        set sign {}
    }
    while {$i>0} {
        set res [expr {$i%2}]$res
        set i [expr {$i/2}]
    }
    if {$res eq {}} {set res 0}

    if {$width ne {}} {
        append d [string repeat 0 $width] $res
        set res [string range $d [string length $res] end]
    }
    return $sign$res
}

proc proc_get_derive_addr_width {flash_density} {
    switch $flash_density {
        "1" {
            return 15
        }
        "2" {
            return 16
        }
        "4" {
            return 17
        }
        "8" {
            return 18
        }
        "16" {
            return 19 
        }
        "32" {
            return 20
        }
        "64" {
            return 21
        }
        "128" {
            return 22
        }
        "256" {
            return 23
        }
        "512" {
            return 24
        }
        "1024" {
            return 25
        }
        "2048" {
            return 26
        }
        default {
            # Should never enter this function
            send_message error "$flash_density is not a valid type"
        }
    
    }
}
