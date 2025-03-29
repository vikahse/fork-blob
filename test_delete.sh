#!/bin/bash

size=1048576

for i in {1..20}; do
    echo "************************************************"
    echo "Test number $i"
    echo "************************************************"
    
    dd if=/dev/random of=test.bin bs=1 count=$size

    idx=$((RANDOM % 200))

    head -c "$idx" test.bin > test2.bin #idx включительно
    tail -c "+$((idx + 2))" test.bin >> test2.bin #idx + 1 не включен

    ./main test.bin > manifest
    ./main test2.bin > manifest2

    diff manifest manifest2
done