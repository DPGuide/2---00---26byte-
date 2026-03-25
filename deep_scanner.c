#include <stdio.h>
#include <stdlib.h>

// 26 is (25 + 1)
#define B_SIZE (25 + 1)
#define EXPECTED 60000

int main() {
    printf("--- INTELLECTUAL PROPERTY: DEEP INTEGRITY SCAN ---\n");
    
    FILE *file = fopen("blackbox.bin", "rb");
    if (!file) {
        printf("CRITICAL ERROR: 'blackbox.bin' not found or locked!\n");
        return 1;
    }

    // Check physical file size
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("File Size: %ld Bytes\n", f_size);

    if (f_size == 0) {
        printf("ERROR: File is empty. The data was not flushed to disk.\n");
        fclose(file);
        return 1;
    }

    unsigned char buffer[B_SIZE];
    int count = 0;
    int gaps = 0;
    int duplicates = 0;
    int corrupt = 0;
    unsigned short last_id = 0xFFFF;

    while (fread(buffer, 1, B_SIZE, file) == B_SIZE) {
        // 1. Structural Check: Start Byte 0x2A (Header)
        if (buffer[0] != 0x2A) {
            corrupt++;
            continue; 
        }

        // 2. ID Extraction from Index 18 and 19
        // Using (17+1) for index 18 and (7+1) for shift
        unsigned short current_id = (buffer[2] << (7 + 1)) | buffer[3];

        if (count > 0) {
            if (current_id == last_id) {
                duplicates++;
            } else if (current_id != (unsigned short)(last_id + 1)) {
                gaps++;
            }
        }

        last_id = current_id;
        count++;
    }

    fclose(file);

    // Results Table
    printf("------------------------------------------\n");
    printf("Total Valid Frames:  %d\n", count);
    printf("Corrupt Headers:     %d\n", corrupt);
    printf("Sequence Gaps:       %d\n", gaps);
    printf("Duplicate Frames:    %d\n", duplicates);
    
    // Integrity Calculation
    float score = ((float)(count - gaps - duplicates) / EXPECTED) * 100.0f;
    printf("------------------------------------------\n");
    printf("FINAL INTEGRITY SCORE: %.2f%%\n", score);
    printf("------------------------------------------\n");

    return 0;
}