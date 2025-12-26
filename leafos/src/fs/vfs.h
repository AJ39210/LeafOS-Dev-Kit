// src/fs/vfs.h - Virtual File System header

#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

struct file_node; // forward

typedef struct {
    char name[128];
    uint32_t inode;
} dirent_t;

typedef uint32_t (*read_func_t)(struct file_node*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_func_t)(struct file_node*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_func_t)(struct file_node*, uint32_t);
typedef void (*close_func_t)(struct file_node*);
typedef dirent_t* (*readdir_func_t)(struct file_node*, uint32_t);
typedef struct file_node* (*finddir_func_t)(struct file_node*, const char*);

typedef struct file_node {
    char name[128];
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    read_func_t read;
    write_func_t write;
    open_func_t open;
    close_func_t close;
    readdir_func_t readdir;
    finddir_func_t finddir;
} file_node_t;

// VFS API
void vfs_init(void);
void vfs_set_root(file_node_t* root);
file_node_t* vfs_get_root(void);

#endif
