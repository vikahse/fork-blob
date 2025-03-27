#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rollsum.h"
#include <openssl/sha.h>

#define WINDOW_SIZE 64
#define MIN_CHUNK_SIZE (64 * 1024)
#define MAX_CHUNK_SIZE (128 * 1024)
#define ANCHOR_MASK 0x1FFFFF

void print_sha256(unsigned char *data, size_t len) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(data, len, hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    size_t start = 0, pos = 0;
    Rollsum rs;

    printf("[\n");
    int first_chunk = 1;

    while (start < size) {
        size_t chunk_end = start + MIN_CHUNK_SIZE;
        if (chunk_end >= size)
            chunk_end = size;

        int anchor_found = 0;
        size_t anchor_pos = chunk_end;

        RollsumInit(&rs);

        for (pos = start; pos < start + WINDOW_SIZE && pos < size; pos++) {
            RollsumRollin(&rs, data[pos]);
        }

        for (; pos < size && pos < start + MAX_CHUNK_SIZE; pos++) {
            if (pos - start >= MIN_CHUNK_SIZE && (RollsumDigest(&rs) & ANCHOR_MASK) == 0) {
                anchor_found = 1;
                anchor_pos = pos;
                break;
            }
            RollsumRollout(&rs, data[pos - WINDOW_SIZE]);
            RollsumRollin(&rs, data[pos]);
        }

        size_t chunk_size = (anchor_found ? anchor_pos : pos) - start;

        if (!first_chunk)
            printf(",\n");
        first_chunk = 0;

        printf("  \"%zu:", chunk_size);
        print_sha256(data + start, chunk_size);
        printf("\"");

        start += chunk_size;
    }

    printf("\n]\n");

    free(data);

    return 0;
}
