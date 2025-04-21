.PHONY: generate_hex update_mif generate_firmware assemble_ossc

all: generate_firmware

update_mif:
	cd .. && quartus_cdb ossc --update_mif

assemble_ossc: update_mif
	cd .. && quartus_asm ossc

generate_firmware: assemble_ossc create_fw_img
	./create_fw_img ../output_files/ossc.rbf 0.86 aud-psp

create_fw_img: create_fw_img.c
	gcc create_fw_img.c -o create_fw_img

# vim: set noet ts=2 sw=2 sts=2
