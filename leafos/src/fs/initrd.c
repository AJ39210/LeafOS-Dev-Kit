// src/fs/initrd.c - Initial RAM disk filesystem with directory support
#include "initrd.h"
#include "vfs.h"
#include <stdint.h>
#include <stddef.h>
#define INITRD_MAGIC 0x494E5452  // "INTR"

typedef struct {
    uint32_t magic;
    uint32_t file_count;
} initrd_header_t;

typedef struct {
    char name[128];
    uint32_t offset;
    uint32_t length;
    uint32_t is_directory;  // 1 if directory, 0 if file
} initrd_file_entry_t;

static initrd_header_t* initrd_header;
static initrd_file_entry_t* file_entries;
static uint8_t* initrd_data;
static file_node_t root_node;
static file_node_t file_nodes[64];
static uint32_t max_files = 64;

// String helper - check if path starts with directory
static int path_in_directory(const char* path, const char* dir) {
    // Check if path starts with "dir/"
    while (*dir) {
        if (*path != *dir) return 0;
        path++;
        dir++;
    }
    // Must be followed by '/'
    return (*path == '/');
}

// Get filename from path (after last /)
static const char* get_filename(const char* path) {
    const char* last_slash = path;
    while (*path) {
        if (*path == '/') last_slash = path + 1;
        path++;
    }
    return last_slash;
}

// Read file contents
static uint32_t initrd_read(file_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->inode >= initrd_header->file_count) return 0;
    initrd_file_entry_t* entry = &file_entries[node->inode];
    
    // Can't read directories
    if (entry->is_directory) return 0;
    
    if (offset >= entry->length) return 0;
    if (offset + size > entry->length) size = entry->length - offset;
    const uint8_t* file_data = initrd_data + entry->offset;
    for (uint32_t i = 0; i < size; i++) buffer[i] = file_data[offset + i];
    return size;
}

// Directory listing
static dirent_t* initrd_readdir(file_node_t* node, uint32_t index) {
    (void)node;
    if (index >= initrd_header->file_count) return NULL;
    static dirent_t dirent;
    
    const char* name = file_entries[index].name;
    uint32_t i = 0;
    for (; i < 127 && name[i] != '\0'; i++)
        dirent.name[i] = name[i];
    dirent.name[i] = '\0';
    dirent.inode = index;
    return &dirent;
}

// Find file by name
static file_node_t* initrd_finddir(file_node_t* node, const char* name) {
    (void)node;
    for (uint32_t i = 0; i < initrd_header->file_count; i++) {
        const char* a = file_entries[i].name;
        const char* b = name;
        while (*a && *b && (*a == *b)) { a++; b++; }
        if (*a == '\0' && *b == '\0') return &file_nodes[i];
    }
    return NULL;
}

// InitRD setup
void initrd_init(uint32_t location, uint32_t size) {
    (void)size;
    initrd_header = (initrd_header_t*)location;
    if (initrd_header->magic != INITRD_MAGIC) {
        initrd_header->magic = INITRD_MAGIC;
        initrd_header->file_count = 0;
    }
    file_entries = (initrd_file_entry_t*)(location + sizeof(initrd_header_t));
    initrd_data = (uint8_t*)(location + sizeof(initrd_header_t) +
                             sizeof(initrd_file_entry_t) * initrd_header->file_count);
    
    // Root node
    root_node.flags = FS_DIRECTORY;
    root_node.inode = 0;
    root_node.length = 0;
    root_node.read = NULL;
    root_node.write = NULL;
    root_node.open = NULL;
    root_node.close = NULL;
    root_node.readdir = initrd_readdir;
    root_node.finddir = initrd_finddir;
    
    // Populate file nodes
    for (uint32_t i = 0; i < initrd_header->file_count && i < max_files; i++) {
        file_node_t* fnode = &file_nodes[i];
        for (uint32_t j = 0; j < 127; j++) {
            fnode->name[j] = file_entries[i].name[j];
            if (file_entries[i].name[j] == '\0') break;
        }
        
        if (file_entries[i].is_directory) {
            fnode->flags = FS_DIRECTORY;
            fnode->read = NULL;
            fnode->readdir = initrd_readdir;
            fnode->finddir = initrd_finddir;
        } else {
            fnode->flags = FS_FILE;
            fnode->read = initrd_read;
            fnode->readdir = NULL;
            fnode->finddir = NULL;
        }
        
        fnode->inode = i;
        fnode->length = file_entries[i].length;
        fnode->write = NULL;
        fnode->open = NULL;
        fnode->close = NULL;
    }
    vfs_set_root(&root_node);
}

file_node_t* initrd_get_root(void) {
    return &root_node;
}