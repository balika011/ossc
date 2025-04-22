//
// Copyright (C) 2015-2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "system.h"
#include "userdata.h"
#include "fat16_export.h"
#include "flash.h"
#include "sdcard.h"
#include "controls.h"
#include "av_controller.h"
#include "menu.h"
#include "utils.h"

#define USERDATA_ENTRY_SIZE 0x10000

#define UDATA_IMPT_CANCELLED 104
#define UDATA_EXPT_CANCELLED 105

char target_profile_name[PROFILE_NAME_LEN + 1];

typedef enum
{
	UDE_INITCFG = 0,
	UDE_PROFILE,
} ude_type;

typedef struct
{
	char magic[8];
	uint8_t version_major;
	uint8_t version_minor;
	ude_type type;
} __attribute__((packed)) ude_hdr;

typedef struct
{
	ude_hdr hdr;
	uint16_t data_len;
	uint8_t last_profile[AV_LAST];
	uint8_t profile_link;
	uint8_t last_input;
	uint8_t def_input;
	uint8_t lcd_bl_timeout;
	uint8_t auto_input;
	uint8_t auto_av1_ypbpr;
	uint8_t auto_av2_ypbpr;
	uint8_t auto_av3_ypbpr;
	uint8_t osd_enable;
	uint8_t osd_status_timeout;
	uint8_t phase_hotkey_enable;
	uint16_t keys[REMOTE_MAX_KEYS];
} __attribute__((packed)) ude_initcfg;

typedef struct
{
	ude_hdr hdr;
	char name[PROFILE_NAME_LEN + 1];
	uint16_t avc_data_len;
	uint16_t vm_data_len;
	avconfig_t avc;
	// mode_data_t vm[VIDEO_MODES_CNT];
} __attribute__((packed)) ude_profile;

int __attribute__((noinline, __section__(".rtext"))) userdata_save_initconfig()
{
	ude_initcfg initcfg;
	strncpy(initcfg.hdr.magic, "USRDATA", 8);
	initcfg.hdr.type = UDE_INITCFG;
	initcfg.hdr.version_major = INITCFG_VER_MAJOR;
	initcfg.hdr.version_minor = INITCFG_VER_MINOR;

	initcfg.data_len = sizeof(ude_initcfg) - offsetof(ude_initcfg, last_profile);

	memcpy(initcfg.last_profile, input_profiles, sizeof(input_profiles));
	initcfg.last_input = target_input;
	initcfg.def_input = def_input;
	initcfg.profile_link = profile_link;
	initcfg.lcd_bl_timeout = lcd_bl_timeout;
	initcfg.auto_input = auto_input;
	initcfg.auto_av1_ypbpr = auto_av1_ypbpr;
	initcfg.auto_av2_ypbpr = auto_av2_ypbpr;
	initcfg.auto_av3_ypbpr = auto_av3_ypbpr;
	initcfg.osd_enable = osd_enable;
	initcfg.osd_status_timeout = osd_status_timeout;
	initcfg.phase_hotkey_enable = phase_hotkey_enable;
	memcpy(initcfg.keys, rc_keymap, sizeof(rc_keymap));

	uint32_t *initcfg_32 = (uint32_t *)&initcfg;
	volatile uint32_t *flash = (volatile uint32_t *)(FLASH_MEM_BASE + USERDATA_OFFSET + MAX_USERDATA_ENTRY * USERDATA_ENTRY_SIZE);
	flash_write_protect(0);
	flash_erase_64k(USERDATA_OFFSET + MAX_USERDATA_ENTRY * USERDATA_ENTRY_SIZE);
	// We might save some trash, but it doesn't matter...
	for (int i = 0; i < (sizeof(initcfg) + 3) / 4; i++)
		flash[i] = initcfg_32[i];
	flash_write_protect(1);

	printf("Initconfig data written (%u bytes)\n", initcfg.data_len);

	return 0;
}

int __attribute__((noinline, __section__(".rtext"))) userdata_save_profile(uint8_t entry)
{
	if (entry > MAX_PROFILE)
	{
		printf("invalid entry\n");
		return -1;
	}

	ude_profile profile;
	strncpy(profile.hdr.magic, "USRDATA", 8);
	profile.hdr.type = UDE_PROFILE;
	profile.hdr.version_major = PROFILE_VER_MAJOR;
	profile.hdr.version_minor = PROFILE_VER_MINOR;

	profile.avc_data_len = sizeof(avconfig_t);
	profile.vm_data_len = sizeof(video_modes_plm);

	if (target_profile_name[0] == 0)
		sniprintf(target_profile_name, PROFILE_NAME_LEN + 1, "<used>");

	strncpy(profile.name, target_profile_name, PROFILE_NAME_LEN + 1);
	memcpy(&profile.avc, &tc, sizeof(avconfig_t));

	uint32_t *profile_32 = (uint32_t *)&profile;
	uint32_t *flash = (uint32_t *)(FLASH_MEM_BASE + USERDATA_OFFSET + entry * USERDATA_ENTRY_SIZE);
	flash_write_protect(0);
	flash_erase_64k(USERDATA_OFFSET + entry * USERDATA_ENTRY_SIZE);
	for (int i = 0; i < sizeof(profile) / 4; i++)
		*flash++ = *profile_32++;

	uint8_t *video_modes_plm_8 = (uint8_t *)video_modes_plm;

	uint32_t profile_rem = sizeof(profile) % 4;
	if (profile_rem != 0)
	{
		uint8_t last[4];
		memcpy(last, profile_32, profile_rem);
		memcpy(&last[profile_rem], video_modes_plm_8, 4 - profile_rem);
		video_modes_plm_8 += 4 - profile_rem;

		*flash++ = *(uint32_t *)last;
	}

	uint32_t *video_modes_plm_32 = (uint32_t *)video_modes_plm_8;
	for (int i = 0; i < (sizeof(video_modes_plm) + 3) / 4; i++) // We might save some trash, but it doesn't matter...
		*flash++ = *video_modes_plm_8++;
	flash_write_protect(1);

	printf("Profile %u data written (%u bytes)\n", entry, sizeof(avconfig_t) + sizeof(video_modes_plm));

	return 0;
}

int userdata_load_initconfig()
{
	ude_hdr *hdr = (ude_hdr *)(FLASH_MEM_BASE + USERDATA_OFFSET + MAX_USERDATA_ENTRY * USERDATA_ENTRY_SIZE);
	if (strncmp(hdr->magic, "USRDATA", 8))
	{
		printf("No userdata found on entry %u\n", MAX_USERDATA_ENTRY);
		return 1;
	}

	if (hdr->type != UDE_INITCFG || hdr->version_major != INITCFG_VER_MAJOR || hdr->version_minor != INITCFG_VER_MINOR)
	{
		printf("Initconfig version %u.%.2u does not match current one\n", hdr->version_major, hdr->version_minor);
		return 2;
	}

	ude_initcfg *initcfg = (ude_initcfg *)hdr;

	if (initcfg->data_len != sizeof(ude_initcfg) - offsetof(ude_initcfg, last_profile))
	{
		printf("Initconfig has corrupted data_len\n");
		return 3;
	}

	for (int i = 0; i < sizeof(input_profiles) / sizeof(*input_profiles); ++i)
		if (initcfg->last_profile[i] <= MAX_PROFILE)
			input_profiles[i] = initcfg->last_profile[i];

	def_input = initcfg->def_input;
	if (def_input < AV_LAST)
		target_input = def_input;
	else if (initcfg->last_input < AV_LAST)
		target_input = initcfg->last_input;

	auto_input = initcfg->auto_input;
	auto_av1_ypbpr = initcfg->auto_av1_ypbpr;
	auto_av2_ypbpr = initcfg->auto_av2_ypbpr;
	auto_av3_ypbpr = initcfg->auto_av3_ypbpr;
	osd_enable = initcfg->osd_enable;
	osd_status_timeout = initcfg->osd_status_timeout;
	profile_link = initcfg->profile_link;
	profile_sel = input_profiles[AV_TESTPAT]; // Global profile
	lcd_bl_timeout = initcfg->lcd_bl_timeout;
	phase_hotkey_enable = initcfg->phase_hotkey_enable;
	memcpy(rc_keymap, initcfg->keys, sizeof(rc_keymap));

	printf("RC data read (%u bytes)\n", sizeof(rc_keymap));

	return 0;
}

int userdata_load_profile(uint8_t entry, int dry_run)
{
	target_profile_name[0] = 0;

	if (entry > MAX_PROFILE)
	{
		printf("invalid entry\n");
		return -1;
	}

	ude_hdr *hdr = (ude_hdr *)(FLASH_MEM_BASE + USERDATA_OFFSET + entry * USERDATA_ENTRY_SIZE);
	if (strncmp(hdr->magic, "USRDATA", 8))
	{
		printf("No userdata found on entry %u\n", entry);
		return 1;
	}

	if (hdr->type != UDE_PROFILE || hdr->version_major != PROFILE_VER_MAJOR || hdr->version_minor != PROFILE_VER_MINOR)
	{
		printf("Profile version does not match current one\n");
		return 2;
	}

	ude_profile *profile = (ude_profile *)hdr;

	if ((profile->avc_data_len != sizeof(avconfig_t)) || (profile->vm_data_len != sizeof(video_modes_plm)))
	{
		printf("Profile has corrupted data_len\n");
		return 3;
	}

	strncpy(target_profile_name, profile->name, PROFILE_NAME_LEN + 1);

	if (!dry_run)
	{
		uint16_t vm_to_read = profile->vm_data_len;

		memcpy(&tc, &profile->avc, sizeof(avconfig_t));

		memcpy(video_modes_plm, &profile[1], vm_to_read);

		update_cur_vm = 1;

		printf("Profile %u data read (%u bytes)\n", entry, sizeof(avconfig_t) + sizeof(video_modes_plm));
	}

	return 0;
}

int __attribute__((noinline, __section__(".rtext"))) userdata_import()
{
	int retval = sdcard_check();
	if (retval != 0)
		return retval;

	strncpy(menu_row2, "Import? 1=Y, 2=N", LCD_ROW_LEN+1);
    ui_disp_menu(2);

    while (1) {
		uint32_t btn_vec = SC->controls.ir_code;

		if (btn_vec == rc_keymap[RC_BTN1]) {
            break;
        } else if (btn_vec == rc_keymap[RC_BTN2]) {
            retval = UDATA_IMPT_CANCELLED;
            strncpy(menu_row2, "Cancelled", LCD_ROW_LEN+1);
			return retval;
		}

        usleep(WAITLOOP_SLEEP_US);
    }

    strncpy(menu_row2, "Loading...", LCD_ROW_LEN+1);
    ui_disp_menu(2);

	// Import the userdata
	int entries_imported = 0;
	for (int n = 0; n <= MAX_USERDATA_ENTRY; ++n)
	{
		snprintf(menu_row1, sizeof(menu_row1), "%d", n);
		ui_disp_menu(1);

		ude_hdr header;
		if (sdcard_read(512 + n * USERDATA_ENTRY_SIZE, (uint8_t *)&header, sizeof(header)))
		{
			printf("Failed to read SD card\n");
            return -1;
		}

		if (strncmp(header.magic, "USRDATA", 8)) {
            printf("Not an userdata entry at 0x%x\n", 512+n*USERDATA_ENTRY_SIZE);
            continue;
        }

        if ((header.type == UDE_PROFILE) && ((header.version_major != PROFILE_VER_MAJOR) || (header.version_minor != PROFILE_VER_MINOR))) {
            printf("Profile version %u.%.2u does not match current one\n", header.version_major, header.version_minor);
            continue;
        } else if ((header.type == UDE_INITCFG) && ((header.version_major != INITCFG_VER_MAJOR) || (header.version_minor != INITCFG_VER_MINOR))) {
            printf("Initconfig version %u.%.2u does not match current one\n", header.version_major, header.version_minor);
            continue;
        } else {
            printf("Unknown userdata entry type %u\n", header.type);
            continue;
        }

		int entry_len = header.type == UDE_PROFILE ? (sizeof(ude_profile) + sizeof(video_modes_plm)) : sizeof(ude_initcfg);

		flash_write_protect(0);
		flash_erase_64k(USERDATA_OFFSET + n * USERDATA_ENTRY_SIZE);
		uint8_t temp[SD_BLK_SIZE];
		for (int i = 0; i < entry_len; i += SD_BLK_SIZE)
		{
			sdcard_read(512 + n * USERDATA_ENTRY_SIZE + i, temp, SD_BLK_SIZE);

			for (int j = 0; j < SD_BLK_SIZE; j += 4)
				*(uint32_t *)(FLASH_MEM_BASE + USERDATA_OFFSET + n * USERDATA_ENTRY_SIZE + i + j) = *(uint32_t *)&temp[j];
		}
		flash_write_protect(1);

		entries_imported++;
	}

	// flash read immediately after write might fail, add some delay
    usleep(1000);

    userdata_load_initconfig();
    profile_sel = input_profiles[target_input];
    userdata_load_profile(profile_sel, 0);

    sniprintf(menu_row2, LCD_ROW_LEN+1, "%d slots loaded", entries_imported);

	return 1;
}

static uint8_t poll_yesno(const useconds_t useconds, uint32_t *const btn_vec_out)
{
	uint32_t btn_vec;
	uint8_t ret = 0U;

	for (uint32_t i = 0; i < (useconds / WAITLOOP_SLEEP_US); ++i)
	{
		btn_vec = SC->controls.ir_code;

		for (uint32_t j = RC_BTN1; j < (REMOTE_MAX_KEYS - 1); ++j)
		{
			if (btn_vec == rc_keymap[j])
			{
				ret = 1U;
				break;
			}
		}

		if (ret)
			break;

		usleep(WAITLOOP_SLEEP_US);
	}

	if (ret)
		*btn_vec_out = btn_vec;

	return ret;
}

const uint8_t prompt_transitions[] = {1, 2, 0, 0};
const uint8_t prompt_ofs[] = {0, 16, 31, LNG(48, 47)};
const char *prompt_msgs = LNG(
	"SD CARD WILL BE"
	"\0" // [ 0..15]
	"OVERWRITTEN!!!"
	"\0" // [16..30]
	"Export? 1=Y, 2=N"
	"\0"			// [31..47]
	"Press 1 or 2", // [48..60]
	"SDｶｰﾄﾞｦｳﾜｶﾞｷｼﾏｽ"
	"\0" // [ 0..15]
	"ｺﾞﾁｭｳｲｸﾀﾞｻｲ!!!"
	"\0" // [16..30]
	"1=ｼﾞｯｺｳｽﾙ 2=ﾔﾒﾙ"
	"\0"			  // [31..46]
	"ﾄﾞﾁﾗｶｴﾗﾝﾃﾞｸﾀﾞｻｲ" // [47..60]
);

int userdata_export()
{
	_Static_assert(SD_BLK_SIZE == FAT16_SECTOR_SIZE, "Sector size mismatch");

	int retval = sdcard_check();
	if (retval != 0)
	{
		retval = -retval;
		goto out;
	}

	usleep(100000U);

	uint32_t btn_vec;
	uint8_t prompt_state = 0;
	while (1)
	{
		const char *msg = &prompt_msgs[prompt_ofs[prompt_state]];
		useconds_t prompt_delay = (prompt_state == 2) ? 2000000U : ((prompt_state == 3) ? 300000U : 1000000U);
		prompt_state = prompt_transitions[prompt_state];

		strncpy(menu_row2, msg, sizeof(menu_row2));
		ui_disp_menu(2);
		if (poll_yesno(prompt_delay, &btn_vec))
		{
			if (btn_vec == rc_keymap[RC_BTN1])
			{
				break;
			}
			else if (btn_vec == rc_keymap[RC_BTN2] ||
					 btn_vec == rc_keymap[RC_BACK])
			{
				retval = UDATA_EXPT_CANCELLED;
				goto out;
			}
			prompt_state = 3;
		}
	}

	usleep(1000000);
	strncpy(menu_row1, "Format?", sizeof(menu_row1));
	strncpy(menu_row2, "1=FAT16, 2=RAW", sizeof(menu_row2));
	ui_disp_menu(2);
	if ((!poll_yesno(5000000U, &btn_vec)) || ((btn_vec != rc_keymap[RC_BTN1]) && (btn_vec != rc_keymap[RC_BTN2])))
	{
		retval = UDATA_EXPT_CANCELLED;
		goto out;
	}
	uint32_t sd_block_offset = (btn_vec == rc_keymap[RC_BTN1]) ? (PROF_16_DATA_OFS / SD_BLK_SIZE) : 0;

	strncpy(menu_row1, "Export sett.", sizeof(menu_row1));
	strncpy(menu_row2, LNG("Exporting...", "ｵﾏﾁｸﾀﾞｻｲ"), sizeof(menu_row2));
	ui_disp_menu(2);

	// RAW copy
	if (btn_vec == rc_keymap[RC_BTN2])
		goto copy_start;

	/* Zero out the boot sector, FATs and root directory. */
	uint8_t databuf[SD_BLK_SIZE];
	memset(databuf, 0, SD_BLK_SIZE);
	for (uint32_t i = 0; i < FAT16_ROOT_DIR_FIRST_SECTOR + FAT16_ROOT_DIR_SECTORS; i++)
	{
		retval = sdcard_write(i * SD_BLK_SIZE, databuf, SD_BLK_SIZE);
		if (retval)
			goto out;
	}

	/* Generate and write the boot sector. */
	generate_boot_sector_16(databuf);
	retval = sdcard_write(0, databuf, SD_BLK_SIZE);
	if (retval)
		goto out;

	/* Generate and write the file allocation tables. */
	for (uint16_t clusters_written = 0, sd_blk_idx = 0;
		 clusters_written < (PROF_16_DATA_SIZE / FAT16_CLUSTER_SIZE);)
	{
		memset(databuf, 0, SD_BLK_SIZE);
		clusters_written = generate_fat16(databuf, clusters_written);
		retval = sdcard_write(FAT16_1_OFS + sd_blk_idx * SD_BLK_SIZE, databuf, SD_BLK_SIZE);
		if (retval)
			goto out;

		retval = sdcard_write(FAT16_2_OFS + sd_blk_idx * SD_BLK_SIZE, databuf, SD_BLK_SIZE);
		if (retval)
			goto out;

		++sd_blk_idx;
	}

	/* Write the directory entry of the settings file. */
	memset(databuf, 0, SD_BLK_SIZE);
	memcpy(databuf, prof_dirent_16, PROF_DIRENT_16_SIZE);
	retval = sdcard_write(PROF_DIRENT_16_OFS, databuf, SD_BLK_SIZE);
	if (retval)
		goto out;

copy_start:
	// Zero out first 512 bytes (1 SD block) of the file
	memset(databuf, 0, SD_BLK_SIZE);
	retval = sdcard_write(sd_block_offset++ * SD_BLK_SIZE, databuf, SD_BLK_SIZE);
	if (retval)
		goto out;

	for (int i = 0; i < MAX_USERDATA_ENTRY + 1; i++)
	{
		ude_hdr *hdr = (ude_hdr *)(FLASH_MEM_BASE + USERDATA_OFFSET + i * USERDATA_ENTRY_SIZE);
		if (strncmp(hdr->magic, "USRDATA", 8))
			continue;

		uint8_t *data = (uint8_t *) hdr;
		for (int j = 0; j < USERDATA_ENTRY_SIZE; j += SD_BLK_SIZE)
		{
			uint8_t temp[SD_BLK_SIZE];
			memcpy(temp, &data[j], SD_BLK_SIZE);

			retval = sdcard_write(sd_block_offset * SD_BLK_SIZE + i * USERDATA_ENTRY_SIZE + j, temp, SD_BLK_SIZE);
			if (retval)
				goto out;
		}
	}

out:
	const char *msg;
	switch (retval)
	{
		case 0: msg = LNG("Success", "ｶﾝﾘｮｳｼﾏｼﾀ"); break; // Alternative: "ｶﾝﾘｮｳｲﾀｼﾏｼﾀ"
		case SD_NOINIT: msg = LNG("No SD card det.", "SDｶｰﾄﾞｶﾞﾐﾂｶﾘﾏｾﾝ"); break;
		case -EINVAL: msg = LNG("Invalid params.", "ﾊﾟﾗﾒｰﾀｶﾞﾑｺｳﾃﾞｽ"); break;
		case UDATA_EXPT_CANCELLED: msg = LNG("Cancelled", "ｷｬﾝｾﾙｻﾚﾏｼﾀ"); break; // Alternative: "ｷｬﾝｾﾙｻｾﾃｲﾀﾀﾞｷﾏｽ"
		default: msg = LNG("SD/Flash error", "SDｶFLASHﾉｴﾗｰ"); break; // ﾌﾗｯｼｭ would be NG.
	}
	strncpy(menu_row2, msg, sizeof(menu_row2));
	ui_disp_menu(2);

	if (!retval)
	{
		return 1;
	}

	/*
	 * We want the message above to remain on screen, so return a
	 * positive value which nevertheless stands out when debugging.
	 */
	return 0xdead;
}
