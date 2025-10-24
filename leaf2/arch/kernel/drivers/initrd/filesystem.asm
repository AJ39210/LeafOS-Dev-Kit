section .fs
global _fs_file_count
global _fs_get_file_name
global _fs_get_file_data

_fs_file_count:
    dd 2

file1_name: db "hello.txt", 0
file1_data: db "Hello from hello.txt!\n", 0

file2_name: db "Testing FileSYSTEM", 0
file2_data: db "01010101010100110101010.\n", 0

_fs_get_file_name:
    dd file1_name
    dd file2_name

_fs_get_file_data:
    dd file1_data
    dd file2_data
