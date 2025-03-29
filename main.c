#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define WINDOW_SIZE 32
#define MIN_CHUNK 2048
#define MAX_CHUNK 65536
#define HASH_MASK 0x0FFF
#define MOD_ADLER 65521

typedef struct {
    size_t size;
    unsigned char sha256[SHA256_DIGEST_LENGTH];
} Chunk;

typedef struct {
    Chunk *chunks;
    size_t count;
    size_t capacity;
} ChunkList;

void init_chunk_list(ChunkList *list) {
    list->capacity = 16;
    list->count = 0;
    list->chunks = malloc(list->capacity * sizeof(Chunk));
    if (!list->chunks) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
}

void add_chunk(ChunkList *list, size_t size, const unsigned char *sha256) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        Chunk *new_chunks = realloc(list->chunks, list->capacity * sizeof(Chunk));
        if (!new_chunks) {
            perror("Memory reallocation failed");
            exit(EXIT_FAILURE);
        }
        list->chunks = new_chunks;
    }
    
    list->chunks[list->count].size = size;
    memcpy(list->chunks[list->count].sha256, sha256, SHA256_DIGEST_LENGTH);
    list->count++;
}

void print_chunk_list(const ChunkList *list) {
    printf("[\n");
    for (size_t i = 0; i < list->count; i++) {
        printf("\"%zu\":", list->chunks[i].size);
        
        for (int j = 0; j < SHA256_DIGEST_LENGTH; j++) {
            printf("%02x", list->chunks[i].sha256[j]);
        }
        
        printf("%s\n", (i < list->count - 1) ? "," : "");
    }
    printf("]\n");
}

void free_chunk_list(ChunkList *list) {
    free(list->chunks);
    list->chunks = NULL;
    list->count = 0;
    list->capacity = 0;
}

void compute_sha256(const unsigned char *data, size_t len, unsigned char *hash) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, len);
    SHA256_Final(hash, &sha256);
}

void chunk_file(const unsigned char *data, size_t file_size, ChunkList *chunks) {
    size_t current = 0;

    while (current < file_size) {
        size_t max_pos = current + MAX_CHUNK;
        if (max_pos > file_size) max_pos = file_size;

        size_t min_pos = current + MIN_CHUNK;
        if (min_pos > max_pos) min_pos = max_pos;

        size_t split_pos = max_pos;
        int found_split = 0;

        if (min_pos + WINDOW_SIZE <= max_pos) {
            size_t window_start = min_pos - WINDOW_SIZE + 1;
            if (window_start < current) window_start = current;

            uint32_t a = 1, b = 0;
            for (size_t i = window_start; i < window_start + WINDOW_SIZE; ++i) {
                a = (a + data[i]) % MOD_ADLER;
                b = (b + a) % MOD_ADLER;
            }

            uint32_t hash = (b << 16) | a;
            if ((hash & HASH_MASK) == 0) {
                split_pos = window_start + WINDOW_SIZE;
                found_split = 1;
            } else {
                for (size_t j = window_start + WINDOW_SIZE; j < max_pos && !found_split; ++j) {
                    uint8_t out = data[j - WINDOW_SIZE];
                    uint8_t in = data[j];

                    a = (a - out + in) % MOD_ADLER;
                    if (a < 0) a += MOD_ADLER;
                    
                    b = (b - (WINDOW_SIZE * out) + a) % MOD_ADLER;
                    if (b < 0) b += MOD_ADLER;

                    hash = (b << 16) | a;
                    if ((hash & HASH_MASK) == 0) {
                        split_pos = j + 1;
                        found_split = 1;
                    }
                }
            }
        }

        size_t chunk_size = split_pos - current;
        unsigned char sha_hash[SHA256_DIGEST_LENGTH];
        compute_sha256(data + current, chunk_size, sha_hash);

        add_chunk(chunks, chunk_size, sha_hash);

        current = split_pos;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char *data = malloc(size);
    if (!data) {
        perror("Memory allocation failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    if (fread(data, 1, size, file) != size) {
        perror("Error reading file");
        free(data);
        fclose(file);
        return EXIT_FAILURE;
    }
    fclose(file);

    ChunkList chunks;
    init_chunk_list(&chunks);
    chunk_file(data, size, &chunks);
    free(data);

    print_chunk_list(&chunks);
    free_chunk_list(&chunks);

    return EXIT_SUCCESS;
}
