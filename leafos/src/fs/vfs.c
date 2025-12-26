// src/fs/vfs.c - Virtual File System core

#include "vfs.h"

static file_node_t* vfs_root = 0;

void vfs_init(void) {
    vfs_root = 0;
}

void vfs_set_root(file_node_t* root) {
    vfs_root = root;
}

file_node_t* vfs_get_root(void) {
    return vfs_root;
}
