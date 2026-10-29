#ifndef FAT_H
#define FAT_H

#include <stdio.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct
{
  uint8_t jump[3];
  char oem[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t fat_count;
  uint16_t root_dir_entries;
  uint16_t total_sectors_small;
  uint8_t media_descriptor;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t heads_or_sides;
  uint32_t hidden_sectors;
  uint32_t total_sectors_large;

  uint8_t drive_number;
  uint8_t flags;
  uint8_t signature;
  uint32_t volume_id;
  char label_string[11];
  char system_id_string[8];
  uint8_t code[448];
  uint16_t boot_signature;
} FAT_BPB;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct
{
  char name[8];
  char extension[3];
  uint8_t attribute;
  uint8_t reserved[10];
  uint16_t last_modification_time;
  uint16_t last_modification_date;
  uint16_t first_cluster;
  uint32_t size;
} FAT_root_dir_entry;
#pragma pack(pop)

#endif