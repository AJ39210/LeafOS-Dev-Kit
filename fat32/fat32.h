// include/fat32.h
#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

#define SECTOR_SIZE 512
#define MAX_FILENAME 256
#define MAX_OPEN_FILES 16

// FAT32 Boot Sector
typedef struct {
    uint8_t  jump[3];
    uint8_t  oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  num_fats;
    uint16_t root_entries;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t fat_size_16;
    uint16_t sectors_per_track;
    uint16_t num_heads;
    uint32_t hidden_sectors;
    uint32_t total_sectors_32;
    uint32_t fat_size_32;
    uint16_t flags;
    uint16_t version;
    uint32_t root_cluster;
    uint16_t fsinfo_sector;
    uint16_t backup_boot_sector;
    uint8_t  reserved[12];
    uint8_t  drive_number;
    uint8_t  reserved1;
    uint8_t  boot_signature;
    uint32_t volume_id;
    uint8_t  volume_label[11];
    uint8_t  fs_type[8];
} __attribute__((packed)) fat32_boot_sector_t;

// Directory Entry
typedef struct {
    uint8_t  name[11];
    uint8_t  attributes;
    uint8_t  reserved;
    uint8_t  creation_time_ms;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t cluster_high;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t cluster_low;
    uint32_t file_size;
} __attribute__((packed)) fat32_dir_entry_t;

// File attributes
#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN    0x02
#define FAT_ATTR_SYSTEM    0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE   0x20

// File handle
typedef struct {
    uint8_t  in_use;
    uint32_t cluster;
    uint32_t size;
    uint32_t position;
    uint8_t  attributes;
    char     name[MAX_FILENAME];
} fat32_file_t;

// FAT32 functions
void fat32_init(void);
int fat32_mount(void);
int fat32_list_root(void);
int fat32_read_file(const char* filename, uint8_t* buffer, uint32_t max_size);
int fat32_write_file(const char* filename, const uint8_t* data, uint32_t size);
int fat32_file_exists(const char* filename);
uint32_t fat32_get_file_size(const char* filename);

// File operations
fat32_file_t* fat32_open(const char* filename);
int fat32_read(fat32_file_t* file, uint8_t* buffer, uint32_t size);
void fat32_close(fat32_file_t* file);

#endif