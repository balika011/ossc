export PATH=$PATH:~/intelFPGA_standard/24.1std/quartus/bin:~/intelFPGA_standard/24.1std/quartus/sopc_builder/bin/:/opt/riscv/bin

set -e

quartus_cpf --convert --frequency=2MHz --voltage=3.3V --operation=p output_files/ossc.sof output_files/ossc.svf
openocd -f scripts/openocd_load.cfg

openocd -f scripts/openocd_gdb.cfg
