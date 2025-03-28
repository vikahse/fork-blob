#!/bin/bash
size=1048576

dd if=/dev/random of=test.bin bs=1 count=$size

idx=$((RANDOM % 200))


for i in {1..20}; do
    echo "************************************************"
    echo "Test number $i"
    echo "************************************************"

    head -c "$idx" test.bin > test2.bin #idx включительно
    echo -n $'\x01' >> test2.bin
    tail -c "+$((idx + 1))" test.bin >> test2.bin #idx + 1 включено

    ./main test.bin > manifest
    ./main test2.bin > manifest2

    diff manifest manifest2
done
