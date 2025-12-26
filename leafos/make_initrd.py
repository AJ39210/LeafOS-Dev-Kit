#!/usr/bin/env python3
import struct
import sys
import os

def create_initrd(paths, output):
    magic = 0x494E5452  # "INTR"
    
    # Collect all files and directories
    entries = []
    
    for path in paths:
        if os.path.isdir(path):
            # Add directory and all its contents
            for root, dirs, files in os.walk(path):
                # Calculate relative path from the input directory
                if root == path:
                    rel_root = ""
                else:
                    rel_root = os.path.relpath(root, path)
                
                # Add subdirectories
                for dirname in dirs:
                    if rel_root:
                        dir_path = os.path.join(rel_root, dirname)
                    else:
                        dir_path = dirname
                    
                    entries.append({
                        'name': dir_path,
                        'is_dir': True,
                        'data': b''
                    })
                
                # Add files in this directory
                for filename in files:
                    file_path = os.path.join(root, filename)
                    
                    if rel_root:
                        rel_file_path = os.path.join(rel_root, filename)
                    else:
                        rel_file_path = filename
                    
                    with open(file_path, 'rb') as f:
                        data = f.read()
                    
                    entries.append({
                        'name': rel_file_path,
                        'is_dir': False,
                        'data': data
                    })
        else:
            # Single file
            basename = os.path.basename(path)
            with open(path, 'rb') as f:
                data = f.read()
            entries.append({
                'name': basename,
                'is_dir': False,
                'data': data
            })
    
    file_count = len(entries)
    header_size = 8
    entry_size = 128 + 4 + 4 + 4  # name + offset + length + is_directory
    entries_size = entry_size * file_count
    data_offset = header_size + entries_size
    
    with open(output, 'wb') as out:
        # Write header
        out.write(struct.pack('<II', magic, file_count))
        
        # Write file entries
        current_offset = data_offset
        for entry in entries:
            name = entry['name'].encode('ascii').ljust(128, b'\0')
            out.write(name)
            out.write(struct.pack('<I', current_offset if not entry['is_dir'] else 0))
            out.write(struct.pack('<I', len(entry['data'])))
            out.write(struct.pack('<I', 1 if entry['is_dir'] else 0))
            current_offset += len(entry['data'])
        
        # Write file data
        for entry in entries:
            if not entry['is_dir']:
                out.write(entry['data'])
    
    print(f"Created {output} with {file_count} entries:")
    for entry in entries:
        entry_type = "[DIR]" if entry['is_dir'] else "[FILE]"
        print(f"  {entry_type} {entry['name']}")

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: make_initrd.py output.img dir1 [dir2...]")
        print("Example: make_initrd.py initrd.img rootfs")
        sys.exit(1)
    
    output = sys.argv[1]
    paths = sys.argv[2:]
    create_initrd(paths, output)
