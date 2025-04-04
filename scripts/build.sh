export PATH=$PATH:~/intelFPGA_standard/24.1std/quartus/bin:~/intelFPGA_standard/24.1std/quartus/sopc_builder/bin/:/opt/riscv/bin

set -e

gcc tools/bin2hex.c -o tools/bin2hex
gcc tools/create_fw_img.c -o tools/create_fw_img

# cd software/rom
# make HAS_SH1107=y generate_hex
# cd -

cd software/fw
make HAS_SH1107=y generate_bin
cd -

quartus_cdb ossc -c ossc --update_mif
quartus_asm --read_settings_files=on --write_settings_files=off ossc -c ossc

tools/create_fw_img output_files/ossc.rbf software/fw/mem_init/flash.bin 1.12
