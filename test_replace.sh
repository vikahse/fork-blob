#!/bin/bash

size=1048576

for i in {1..20}; do
    echo "************************************************"
    echo "Test number $i"
    echo "************************************************"
    
    dd if=/dev/random of=test.bin bs=1 count=$size

    idx=$((RANDOM % 200))
    new_idx=$((RANDOM % 200))

    byte=$(dd if=test.bin bs=1 skip="$((idx - 1))" count=1 2>/dev/null | xxd -p)

    head -c "$((new_idx - 1))" test.bin > test2.bin
    echo -n $'\x'$byte >> test2.bin
    tail -c "+$((new_idx + 1))" test.bin >> test2.bin

    ./main test.bin > manifest
    ./main test2.bin > manifest2

    diff manifest manifest2

done