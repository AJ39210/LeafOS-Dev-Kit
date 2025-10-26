// kernel/fat32.c
#include "../include/fat32.h"
#include "../include/vga.h"

// Simulated disk (we'll use a RAM disk for now)
static uint8_t ramdisk[1024 * 1024]; // 1MB RAM disk
static fat32_boot_sector_t boot_sector;
static uint32_t fat_start;
static uint32_t data_start;
static uint32_t root_cluster;
static uint8_t mounted = 0;

// Open file table
static fat32_file_t open_files[MAX_OPEN_FILES];

// Helper: Read sector from disk
static void read_sector(uint32_t sector, uint8_t* buffer) {
    for (int i = 0; i < SECTOR_SIZE; i++) {
        buffer[i] = ramdisk[sector * SECTOR_SIZE + i];
    }
}

// Helper: Write sector to disk
static void write_sector(uint32_t sector, uint8_t* buffer) {
    for (int i = 0; i < SECTOR_SIZE; i++) {
        ramdisk[sector * SECTOR_SIZE + i] = buffer[i];
    }
}

// Helper: Get next cluster from FAT
static uint32_t get_next_cluster(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fat_start + (fat_offset / SECTOR_SIZE);
    uint32_t entry_offset = fat_offset % SECTOR_SIZE;
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(fat_sector, buffer);
    
    uint32_t next = *((uint32_t*)&buffer[entry_offset]) & 0x0FFFFFFF;
    
    // Check for end of chain
    if (next >= 0x0FFFFFF8) {
        return 0xFFFFFFFF;
    }
    
    return next;
}

// Helper: String compare
static int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Helper: String copy
static void strcpy(char* dst, const char* src) {
    while (*src) {
        *dst++ = *src++;
    }
    *dst = '\0';
}

// Helper: Convert FAT filename to normal string
static void fat_to_filename(const uint8_t* fat_name, char* filename) {
    int i, j = 0;
    
    // Copy name part (8 chars)
    for (i = 0; i < 8 && fat_name[i] != ' '; i++) {
        filename[j++] = fat_name[i];
    }
    
    // Add extension if present
    if (fat_name[8] != ' ') {
        filename[j++] = '.';
        for (i = 8; i < 11 && fat_name[i] != ' '; i++) {
            filename[j++] = fat_name[i];
        }
    }
    
    filename[j] = '\0';
}

void fat32_init(void) {
    // Initialize open file table
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        open_files[i].in_use = 0;
    }
    
    // Create a simple FAT32 filesystem in RAM
    // This is a minimal implementation for demonstration
    
    // Boot sector
    boot_sector.bytes_per_sector = SECTOR_SIZE;
    boot_sector.sectors_per_cluster = 1;
    boot_sector.reserved_sectors = 32;
    boot_sector.num_fats = 2;
    boot_sector.total_sectors_32 = 2048; // 1MB
    boot_sector.fat_size_32 = 16;
    boot_sector.root_cluster = 2;
    
    // Write boot sector
    write_sector(0, (uint8_t*)&boot_sector);
    
    fat_start = boot_sector.reserved_sectors;
    data_start = fat_start + (boot_sector.num_fats * boot_sector.fat_size_32);
    root_cluster = boot_sector.root_cluster;
    
    vga_print("[FAT32] Driver initialized (RAM disk)\n");
}

int fat32_mount(void) {
    uint8_t buffer[SECTOR_SIZE];
    read_sector(0, buffer);
    
    fat32_boot_sector_t* bs = (fat32_boot_sector_t*)buffer;
    
    if (bs->bytes_per_sector != SECTOR_SIZE) {
        return -1;
    }
    
    boot_sector = *bs;
    fat_start = boot_sector.reserved_sectors;
    data_start = fat_start + (boot_sector.num_fats * boot_sector.fat_size_32);
    root_cluster = boot_sector.root_cluster;
    
    mounted = 1;
    vga_print("[FAT32] Filesystem mounted\n");
    return 0;
}

int fat32_list_root(void) {
    if (!mounted) {
        vga_print("[FAT32] Filesystem not mounted\n");
        return -1;
    }
    
    uint32_t cluster = root_cluster;
    uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(cluster_sector, buffer);
    
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buffer;
    int count = 0;
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_print("\nDirectory listing:\n");
    vga_print("==================\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    for (int i = 0; i < SECTOR_SIZE / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00) break; // End of directory
        if (entries[i].name[0] == 0xE5) continue; // Deleted entry
        if (entries[i].attributes & FAT_ATTR_VOLUME_ID) continue; // Volume label
        
        char filename[MAX_FILENAME];
        fat_to_filename(entries[i].name, filename);
        
        vga_print("  ");
        if (entries[i].attributes & FAT_ATTR_DIRECTORY) {
            vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
            vga_print("[DIR]  ");
        } else {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_print("[FILE] ");
        }
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        vga_print(filename);
        vga_print("\n");
        count++;
    }
    
    if (count == 0) {
        vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
        vga_print("  (empty)\n");
    }
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_print("\n");
    
    return count;
}

int fat32_file_exists(const char* filename) {
    if (!mounted) return 0;
    
    uint32_t cluster = root_cluster;
    uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(cluster_sector, buffer);
    
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buffer;
    
    for (int i = 0; i < SECTOR_SIZE / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        
        char entry_name[MAX_FILENAME];
        fat_to_filename(entries[i].name, entry_name);
        
        if (strcmp(entry_name, filename) == 0) {
            return 1;
        }
    }
    
    return 0;
}

uint32_t fat32_get_file_size(const char* filename) {
    if (!mounted) return 0;
    
    uint32_t cluster = root_cluster;
    uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(cluster_sector, buffer);
    
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buffer;
    
    for (int i = 0; i < SECTOR_SIZE / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        
        char entry_name[MAX_FILENAME];
        fat_to_filename(entries[i].name, entry_name);
        
        if (strcmp(entry_name, filename) == 0) {
            return entries[i].file_size;
        }
    }
    
    return 0;
}

fat32_file_t* fat32_open(const char* filename) {
    if (!mounted) return 0;
    
    // Find free file handle
    fat32_file_t* file = 0;
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (!open_files[i].in_use) {
            file = &open_files[i];
            break;
        }
    }
    
    if (!file) return 0;
    
    // Find file in directory
    uint32_t cluster = root_cluster;
    uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(cluster_sector, buffer);
    
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buffer;
    
    for (int i = 0; i < SECTOR_SIZE / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        
        char entry_name[MAX_FILENAME];
        fat_to_filename(entries[i].name, entry_name);
        
        if (strcmp(entry_name, filename) == 0) {
            file->in_use = 1;
            file->cluster = ((uint32_t)entries[i].cluster_high << 16) | entries[i].cluster_low;
            file->size = entries[i].file_size;
            file->position = 0;
            file->attributes = entries[i].attributes;
            strcpy(file->name, filename);
            return file;
        }
    }
    
    return 0;
}

int fat32_read(fat32_file_t* file, uint8_t* buffer, uint32_t size) {
    if (!file || !file->in_use) return -1;
    
    uint32_t bytes_read = 0;
    uint32_t cluster = file->cluster;
    
    while (bytes_read < size && file->position < file->size && cluster != 0xFFFFFFFF) {
        uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
        uint8_t sector_buffer[SECTOR_SIZE];
        read_sector(cluster_sector, sector_buffer);
        
        uint32_t bytes_to_copy = SECTOR_SIZE;
        if (file->position + bytes_to_copy > file->size) {
            bytes_to_copy = file->size - file->position;
        }
        if (bytes_read + bytes_to_copy > size) {
            bytes_to_copy = size - bytes_read;
        }
        
        for (uint32_t i = 0; i < bytes_to_copy; i++) {
            buffer[bytes_read++] = sector_buffer[i];
        }
        
        file->position += bytes_to_copy;
        cluster = get_next_cluster(cluster);
    }
    
    return bytes_read;
}

void fat32_close(fat32_file_t* file) {
    if (file && file->in_use) {
        file->in_use = 0;
    }
}

int fat32_read_file(const char* filename, uint8_t* buffer, uint32_t max_size) {
    fat32_file_t* file = fat32_open(filename);
    if (!file) return -1;
    
    int bytes = fat32_read(file, buffer, max_size);
    fat32_close(file);
    
    return bytes;
}

int fat32_write_file(const char* filename, const uint8_t* data, uint32_t size) {
    if (!mounted) return -1;
    
    // Find root directory sector
    uint32_t cluster = root_cluster;
    uint32_t cluster_sector = data_start + ((cluster - 2) * boot_sector.sectors_per_cluster);
    
    uint8_t buffer[SECTOR_SIZE];
    read_sector(cluster_sector, buffer);
    
    fat32_dir_entry_t* entries = (fat32_dir_entry_t*)buffer;
    
    // Find empty slot
    int slot = -1;
    for (int i = 0; i < SECTOR_SIZE / sizeof(fat32_dir_entry_t); i++) {
        if (entries[i].name[0] == 0x00 || entries[i].name[0] == 0xE5) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) return -1; // Directory full
    
    // Create filename (8.3 format)
    for (int i = 0; i < 11; i++) {
        entries[slot].name[i] = ' ';
    }
    
    int name_len = 0;
    while (filename[name_len] && filename[name_len] != '.') name_len++;
    
    for (int i = 0; i < name_len && i < 8; i++) {
        entries[slot].name[i] = filename[i];
    }
    
    // Find extension
    int ext_start = name_len;
    if (filename[ext_start] == '.') ext_start++;
    
    for (int i = 0; i < 3 && filename[ext_start + i]; i++) {
        entries[slot].name[8 + i] = filename[ext_start + i];
    }
    
    // Allocate cluster for file data
    uint32_t file_cluster = 3; // Start from cluster 3
    entries[slot].cluster_low = file_cluster & 0xFFFF;
    entries[slot].cluster_high = (file_cluster >> 16) & 0xFFFF;
    entries[slot].file_size = size;
    entries[slot].attributes = FAT_ATTR_ARCHIVE;
    
    // Write directory entry
    write_sector(cluster_sector, buffer);
    
    // Write file data
    uint32_t data_sector = data_start + ((file_cluster - 2) * boot_sector.sectors_per_cluster);
    uint8_t data_buffer[SECTOR_SIZE];
    
    for (uint32_t i = 0; i < SECTOR_SIZE; i++) {
        if (i < size) {
            data_buffer[i] = data[i];
        } else {
            data_buffer[i] = 0;
        }
    }
    
    write_sector(data_sector, data_buffer);
    
    return size;
}