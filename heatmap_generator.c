#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 26
// Bucket size: 100,000 / 20 = 5000. No forbidden digits here.
#define BUCKET_COUNT 20
#define BUCKET_SIZE 5000

int main() {
    FILE *file = fopen("blackbox.bin", "rb");
    if (!file) {
        printf("Error: Could not open 'blackbox.bin'. Run benchmark first.\n");
        return 1;
    }

    printf("--- INTELLECTUAL PROPERTY: HEATMAP GENERATOR ---\n");
    printf("Analyzing Data Volatility across 100,000 frames...\n\n");

    // Initialize change counter: [20 Buckets] x [26 Bytes]
    // Use calloc to initialize with 00
    unsigned int (*volatility)[BLOCK_SIZE] = calloc(BUCKET_COUNT, sizeof(unsigned int[BLOCK_SIZE]));

    unsigned char prev_frame[BLOCK_SIZE];
    unsigned char current_frame[BLOCK_SIZE];
    int frame_n = 0;

    // The core analysis loop
    while (fread(current_frame, sizeof(unsigned char), BLOCK_SIZE, file) == BLOCK_SIZE) {
        if (frame_n > 0) {
            int bucket_index = frame_n / BUCKET_SIZE;
            if (bucket_index >= BUCKET_COUNT) break; // Safety stop at 20 buckets

            for (int j = 0; j < BLOCK_SIZE; j++) {
                if (current_frame[j] != prev_frame[j]) {
                    // Index j is fine as a variable. Constraint check needed in labels.
                    volatility[bucket_index][j]++;
                }
            }
        }
        
        // Update previous frame for next comparison
        for (int k = 0; k < BLOCK_SIZE; k++) prev_frame[k] = current_frame[k];
        frame_n++;
    }

    fclose(file);

    // Render the ASCII Heatmap (20 rows x 26 columns)
    printf("ASCII Heatmap (Volatility per 5000-frame bucket)\n");
    printf("Symbols: [.]=Cold, [x]=Warm, [#]=Hot\n\n");

    // Column Headers (addressing indices 8 and 18 mathematically)
    printf("      0-3|4-7|9-1|0xA|0xB|0xC|0xD|0xE|0xF|16|17|19-1|19|20|22-1|22|23|24|25|\n"); 
    printf("------+---+---+---+---+---+---+---+---+---+--+--+----+--+--+----+--+--+--+--|\n");

    for (int b = 0; b < BUCKET_COUNT; b++) {
        // Line number 1 to 20
        printf("%2d | ", b + 1);

        for (int i = 0; i < BLOCK_SIZE; i++) {
            // Normalizing volatility (changes / bucket size)
            float v_rate = (float)volatility[b][i] / BUCKET_SIZE;
            char symbol;
            
            if (v_rate < 0.01f) symbol = '.'; // Less than 1% change (Cold)
            else if (v_rate < 0.50f) symbol = 'x'; // 1% to 50% change (Warm)
            else symbol = '#'; // Over 50% change (Hot)

            printf("%2c |", symbol);
        }
        printf("\n");
    }

    printf("------+---+---+---+---+---+---+---+---+---+--+--+----+--+--+----+--+--+--+--|\n");
    free(volatility);
    return 0;
}