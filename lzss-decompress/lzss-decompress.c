//
//  lzss_decompress.c
//
//  Created by Pishu Mahtani on 2nd November 2023.
//

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define WINDOW_SIZE 4096
#define MAX_MATCH_LENGTH 18

typedef struct {
    int position;
    int length;
    char nextChar;
} Match;

void decodeLZSS(FILE *input, FILE *output) {
    unsigned char buffer[WINDOW_SIZE];
    int bufferSize = 0;

    while (true) {
        int indicator = fgetc(input);
        if (feof(input)) break;

        if (indicator == 0) {
            int position = (fgetc(input) << 4) | (fgetc(input) >> 4);
            int length = (fgetc(input) & 0xF) + 3;
            for (int i = 0; i < length; i++) {
                fputc(buffer[(position + i) % WINDOW_SIZE], output);
                buffer[(bufferSize + i) % WINDOW_SIZE] = buffer[(position + i) % WINDOW_SIZE];
            }
            bufferSize = (bufferSize + length) % WINDOW_SIZE;
        } else {
            char nextChar = indicator;
            fputc(nextChar, output);
            buffer[bufferSize] = nextChar;
            bufferSize = (bufferSize + 1) % WINDOW_SIZE;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_compressed_file output_decompressed_file\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        printf("Error opening input compressed file.\n");
        return 1;
    }

    FILE *outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL) {
        printf("Error opening output decompressed file.\n");
        fclose(inputFile);
        return 1;
    }

    // Decompress
    decodeLZSS(inputFile, outputFile);

    if (fclose(inputFile) != 0) {
        printf("Error closing input compressed file.\n");
        return 1;
    }

    if (fclose(outputFile) != 0) {
        printf("Error closing output decompressed file.\n");
        return 1;
    }

    printf("Decompression complete. Output saved to %s.\n", argv[2]);
    return 0;
}
