// src/fs/initrd.h - InitRD filesystem header

#ifndef INITRD_H
#define INITRD_H

#include <stdint.h>
#include "vfs.h"

void initrd_init(uint32_t location, uint32_t size);
file_node_t* initrd_get_root(void);

#endif