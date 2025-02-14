//
//  lzss_decompress.c
//
//  Created by Pishu Mahtani on 2nd November 2023.
//  Updated 12th February 2025. Bug discovered in the match lengths possible buffer overflow
//  if the file is larger than the buffer size and other edge cases.
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

// Function to decode LZSS compressed data
void decodeLZSS(FILE *input, FILE *output) {
    unsigned char buffer[WINDOW_SIZE] = { 0 };
    int bufferSize = 0;

    while (true) {
        int indicator = fgetc(input);
        if (feof(input)) break;  // End of file check

        if (indicator == 0) {
            // Match found: the next three bytes are the position and length
            int position = (fgetc(input) << 4) | (fgetc(input) >> 4);  // position is stored as a 12-bit number
            int length = (fgetc(input) & 0xF) + 3;  // length is stored as a 4-bit number, with a base of 3

            // Output matched data from the sliding window
            for (int i = 0; i < length; i++) {
                int index = (position + i) % WINDOW_SIZE;
                fputc(buffer[index], output);  // Write the matched character
                buffer[(bufferSize + i) % WINDOW_SIZE] = buffer[index];  // Update the buffer
            }
            bufferSize = (bufferSize + length) % WINDOW_SIZE;  // Update the buffer size
        } else {
            // Literal byte found: just output it
            char nextChar = indicator;
            fputc(nextChar, output);
            buffer[bufferSize] = nextChar;  // Update the buffer with the literal byte
            bufferSize = (bufferSize + 1) % WINDOW_SIZE;  // Update the buffer size
        }
    }
}

// Main function to handle file input/output and call the decode function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input_compressed_file output_decompressed_file\n", argv[0]);
        return 1;
    }

    // Open input file for reading
    FILE *inputFile = fopen(argv[1], "rb");
    if (inputFile == NULL) {
        printf("Error opening input compressed file.\n");
        return 1;
    }

    // Open output file for writing
    FILE *outputFile = fopen(argv[2], "wb");
    if (outputFile == NULL) {
        printf("Error opening output decompressed file.\n");
        fclose(inputFile);
        return 1;
    }

    // Call the decompression function
    decodeLZSS(inputFile, outputFile);

    // Close the files
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
