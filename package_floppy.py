# Package multi-stage floppy disk image

def pad_file(path, size):
    with open(path, 'rb') as f:
        data = f.read()
    if len(data) > size:
        print(f"Warning: {path} is larger than requested pad size ({len(data)} > {size} bytes)")
    return data.ljust(size, b'\x00')

# 1. Pad stage 1 sector to 510 bytes
stage1 = pad_file('stage1.com', 510)
# Append boot signature
stage1 += b'\x55\xAA'

# 2. Pad stage 2 sectors (4096 bytes = 8 sectors of 512 bytes each)
stage2 = pad_file('stage2.com', 4096)

# 3. Concatenate and write final boot floppy disk image
with open('bootloader.bin', 'wb') as f:
    f.write(stage1 + stage2)

print("Floppy disk boot image successfully created at bootloader.bin! (Total size: 4608 bytes)")
