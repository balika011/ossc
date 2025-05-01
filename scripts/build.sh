export PATH=$PATH:~/intelFPGA_standard/24.1std/quartus/bin:~/intelFPGA_standard/24.1std/quartus/sopc_builder/bin/:/opt/riscv/bin

set -e

gcc tools/create_fw_img.c -o tools/create_fw_img

cd software/fw
make generate_bin
cd -

cd software/updater
make clean
make generate_bin
cd -

quartus_asm --read_settings_files=on --write_settings_files=off ossc -c ossc

tools/create_fw_img output_files/ossc.rbf software/fw/fw.bin software/updater/updater.bin 2.00 beta-1
