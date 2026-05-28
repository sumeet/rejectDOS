#!/bin/bash
# Simple reproduction of the interactive shell functionality
# This mimics what the rejectDOS project would do

echo "=== Simple Shell Simulation ==="
echo "This is what the rejectDOS project would have created"
echo "A 16-bit DOS terminal interface"
echo ""

# Simulate the interactive loop
while true; do
    echo -n "> "
    read -r input
    
    if [ -n "$input" ]; then
        echo "evaluating: $input"
    fi
done