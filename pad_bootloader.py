#!/usr/bin/env python
file_path = 'main.com'  # Replace with your .COM file path
output_path = 'bootloader.bin'  # Output file

with open(file_path, 'rb') as f:
    data = f.read()

if len(data) > 510:
    print(f"code in {file_path} is too large: {len(data)} bytes > 510 bytes")
    exit(1)

# Pad the data to 510 bytes
data = data.ljust(510, b'\x00')

# Add the 0x55AA boot signature
data += b'\x55\xAA'

# Write the final 512-byte bootloader to a file
with open(output_path, 'wb') as f:
    f.write(data)

print(f"Bootloader created at {output_path}")

