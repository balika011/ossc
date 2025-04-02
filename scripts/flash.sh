export PATH=$PATH:~/intelFPGA_standard/24.1std/quartus/bin:~/intelFPGA_standard/24.1std/quartus/sopc_builder/bin/:/opt/riscv/bin

set -e

gcc tools/bitswap.c -o tools/bitswap

cd software/fw
make clean
make HAS_SH1107=y generate_bin
cd -

quartus_cpf --convert --frequency=2MHz --voltage=3.3V --operation=p output_files/ossc.sof output_files/ossc.svf
openocd -f scripts/openocd_load.cfg

tools/bitswap 32 output_files/ossc.rbf output_files/ossc.fbr

openocd -f scripts/openocd_flash.cfg