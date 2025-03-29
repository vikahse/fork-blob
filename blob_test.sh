#!/bin/bash

cp test.bin test_modified.bin

printf '\x01' | dd of=test_modified.bin bs=1 seek=500000 conv=notrunc

gcc -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib -lcrypto -o main main.c rollsum.c

./main test.bin > orig.manifest

./main test_modified.bin > modified.manifest

diff_count=$(diff orig.manifest modified.manifest)
echo "$diff_count"
