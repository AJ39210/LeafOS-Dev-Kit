#ifndef INITRD_H
#define INITRD_H

// Freestanding type definitions
typedef unsigned char  uint8_t;
typedef unsigned int   uint32_t;

#define MAX_FS_ENTRIES 128

typedef struct {
    char name[64];     // full path: "bin/", "home/user.txt"
    uint8_t type;      // 0 = file, 1 = folder
    uint32_t size;
    void* data;
} fs_entry_t;

extern fs_entry_t fs_table[MAX_FS_ENTRIES];
extern int fs_count;
extern char current_path[64];

void initrd_init(void);

#endif
