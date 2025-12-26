// src/shell/elf_exec.h - ELF executable loader header

#ifndef ELF_EXEC_H
#define ELF_EXEC_H

#include <stdint.h>

// Execute an ELF binary from a buffer
int elf_execute_buffer(uint8_t* data, uint32_t size);

// Execute an ELF binary file from InitRD
int elf_execute_file(const char* filename);

#endif