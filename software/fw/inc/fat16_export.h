//
// Copyright (C) 2020 Ari Sundholm <megari@iki.fi>
//
// This file has been contributed to the Open Source Scan Converter project
// developed by Markus Hiienkari Markus Hiienkari <mhiienka@niksula.hut.fi>
// and other members of the retro gaming community.
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

#ifndef FAT16_EXPORT_H_
#define FAT16_EXPORT_H_

#include <stdint.h>

/* Use a sector size of 512 bytes. */
#define FAT16_SECTOR_SIZE 512U

/* This volume has 2048-byte clusters. */
#define FAT16_CLUSTER_SIZE 2048U
#define FAT16_SECTORS_PER_CLUSTER (FAT16_CLUSTER_SIZE/FAT16_SECTOR_SIZE)

/* Offsets of the two File Allocation Tables. */
#define FAT16_1_OFS 0x10000UL
#define FAT16_2_OFS 0x14000UL

/* Each FAT16 entry is a 16-bit little-endian integer. */
#define FAT16_ENTRY_SIZE 2U
#define FAT16_ENTRY_SHIFT 1U
#define FAT16_ENTRIES_PER_SECTOR (FAT16_SECTOR_SIZE >> FAT16_ENTRY_SHIFT)

/* On this volume, each FAT will be 16 kiB in size. */
#define FAT16_SIZE 0x04000UL

/* The first sector of the root directory. */
#define FAT16_ROOT_DIR_FIRST_SECTOR 192U

/* The length of the root directory in sectors. */
#define FAT16_ROOT_DIR_SECTORS 128U

/*
 * Define the properties and contents of the directory entry for the
 * settings file.
 */
#define PROF_DIRENT_16_OFS  0x18000UL
#define PROF_DIRENT_16_SIZE 32U

extern const uint8_t prof_dirent_16[PROF_DIRENT_16_SIZE];

#define PROF_16_DATA_OFS  0x028000UL
#define PROF_16_DATA_SIZE 0x100200UL
#define PROF_16_CLUSTER_COUNT ((PROF_16_DATA_SIZE/FAT16_CLUSTER_SIZE)+!!(PROF_16_DATA_SIZE%FAT16_CLUSTER_SIZE))
/* Profile file data starts at offset 0x00028000 */
/* Profile file data  ends  at offset 0x00128200 */
/* Profile file data is 1049088 bytes long. */


/* Generate a FAT16 boot sector.
 * buf must be at least FAT16_BOOT_SECTOR_SIZE bytes long,
 * and is assumed to be pre-zeroed.
 */
void generate_boot_sector_16(uint8_t *buf);

/*
 * Generate a FAT of a FAT16 volume.
 * The buffer is assumed to be zeroed out and have a size of at least 512 bytes.
 * The number of clusters already written are given as an argument.
 * The function returns the total number of clusters written so far.
 */
uint16_t generate_fat16(void *buf, uint16_t written);

#endif // FAT16_EXPORT_H_
