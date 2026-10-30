#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "FAT.h"

int main(void)
{
  FILE *fp = fopen("fat16.img", "rb");
  if (!fp)
  {
    perror("Failed to open fat16.img");
    return 1;
  }

  FAT_BPB bpb;
  fread(&bpb, sizeof(FAT_BPB), 1, fp);

  printf("*********** Raw MetaData ***********\n");
  printf("Entry 1 -> 0(0x00) -> 3 Bytes: %02x %02x %02x\n", bpb.jump[0], bpb.jump[1], bpb.jump[2]);
  printf("Entry 2 -> 3(0x03) -> 8 Bytes: %02x %02x %02x %02x %02x %02x %02x %02x = %s\n", bpb.oem[0], bpb.oem[1], bpb.oem[2], bpb.oem[3], bpb.oem[4], bpb.oem[5], bpb.oem[6], bpb.oem[7], bpb.oem);
  printf("Entry 3 -> 11(0x0B) -> 2 Bytes: %02x\n", bpb.bytes_per_sector);
  printf("Entry 4 -> 13(0x0D) -> 1 Byte: %02x\n", bpb.sectors_per_cluster);
  printf("Entry 5 -> 14(0x0E) -> 2 Bytes: %02x\n", bpb.reserved_sectors);
  printf("Entry 6 -> 16(0x10) -> 1 Byte: %02x\n", bpb.fat_count);
  printf("Entry 7 -> 17(0x11) -> 2 Bytes: %02x\n", bpb.root_dir_entries);
  printf("Entry 8 -> 19(0x13) -> 2 Bytes: %02x\n", bpb.total_sectors_small);
  printf("Entry 9 -> 21(0x15) -> 1 Byte: %02x\n", bpb.media_descriptor);
  printf("Entry 10 -> 22(0x16) -> 2 Bytes: %02x\n", bpb.sectors_per_fat);
  printf("Entry 11 -> 24(0x18) -> 2 Bytes: %02x\n", bpb.sectors_per_track);
  printf("Entry 12 -> 26(0x1A) -> 2 Bytes: %02x\n", bpb.heads_or_sides);
  printf("Entry 13 -> 28(0x1C) -> 4 Bytes: %02x\n", bpb.hidden_sectors);
  printf("Entry 14 -> 32(0x20) -> 4 Bytes: %02x\n", bpb.total_sectors_large);

  printf("Entry 15 -> 36(0x24) -> 1 Byte: %02x\n", bpb.drive_number);
  printf("Entry 16 -> 37(0x25) -> 1 Byte: %02x\n", bpb.flags);
  printf("Entry 17 -> 38(0x26) -> 1 Byte: %02x\n", bpb.signature);
  printf("Entry 18 -> 39(0x27) -> 4 Bytes: %02x\n", bpb.volume_id);
  printf("Entry 19 -> 43(0x2B) -> 11 Bytes: %.11s\n", bpb.label_string);
  printf("Entry 20 -> 54(0x36) -> 8 Bytes: %.8s\n", bpb.system_id_string);

  printf("Entry 21 -> 62(0x3F) -> 448 Bytes:\n");
  for (int i = 0; i < 448; i++)
  {
    printf("%02x ", bpb.code[i]);
  }

  printf("\n");
  printf("Entry 22 -> 510(0x1FE) -> 2 Bytes: %02x", bpb.boot_signature);

  printf("\n\n");

  // Calculate regions
  uint32_t fat1_lba = bpb.reserved_sectors;
  uint32_t fat2_lba = fat1_lba + bpb.sectors_per_fat;
  uint32_t root_lba = fat1_lba + (bpb.fat_count * bpb.sectors_per_fat);
  uint32_t root_sectors = (bpb.root_dir_entries * 32) / bpb.bytes_per_sector;
  uint32_t data_lba = root_lba + root_sectors;

  printf("=== FAT16 Volume Geometry ===\n");
  printf("Bytes per Sector:    %u\n", bpb.bytes_per_sector);
  printf("Sectors per Cluster: %u\n", bpb.sectors_per_cluster);
  printf("FAT1 Start LBA:      Sector %u (Byte offset: %u)\n", fat1_lba, fat1_lba * 512);
  printf("FAT2 Start LBA:      Sector %u (Byte offset: %u)\n", fat2_lba, fat2_lba * 512);
  printf("Root Dir Start LBA:  Sector %u (Byte offset: %u)\n", root_lba, root_lba * 512);
  printf("Data Start LBA:      Sector %u (Byte offset: %u)\n", data_lba, data_lba * 512);

  fseek(fp, root_lba * bpb.bytes_per_sector, SEEK_SET);
  FAT_root_dir_entry *root_dir_en_16 = malloc(bpb.root_dir_entries * sizeof(FAT_root_dir_entry));
  fread(root_dir_en_16, bpb.root_dir_entries * sizeof(FAT_root_dir_entry), 1, fp);
  printf("Root Directory Entries: \n");
  for (int i = 0; i < bpb.root_dir_entries; i++)
  {
    if (root_dir_en_16[i].attribute == 0x00)
    {
      break;
    }

    printf("%d -> ", i + 1);
    printf("Name: %.8s\n", root_dir_en_16[i].name);
    printf("Extension: %.3s\n", root_dir_en_16[i].extension);
    printf("Attribute: %02x ", root_dir_en_16[i].attribute);

    if (root_dir_en_16[i].attribute == 0x08)
    {
      printf("It's volumne\n");
    }

    if (root_dir_en_16[i].size == 0x0000)
    {
      printf("Empty File/Directory\n");
      continue;
    }

    printf("First Cluster: %02x\n", root_dir_en_16[i].first_cluster);
    printf("Size (bytes): %02x\n", root_dir_en_16[i].size);
  }

  free(root_dir_en_16);
  fclose(fp);
  return 0;
}