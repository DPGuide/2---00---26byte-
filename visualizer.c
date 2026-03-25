#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 26

int main() {
    FILE *file = fopen("blackbox.bin", "rb");
    if (!file) {
        printf("Error: Could not open 'blackbox.bin'. Did you run the blackbox logger?\n");
        return 1;
    }

    unsigned char buffer[BLOCK_SIZE];
    int frame_count = 0;
    
    // Using long long for total to prevent overflow when summing 100,000 frames
    unsigned long long total_sensor_val = 0; 
    unsigned short max_sensor = 0;
    unsigned short min_sensor = 0xFFFF;
    unsigned short last_collision_count = 0;

    printf("--- INTELLECTUAL PROPERTY: DATA VISUALIZER ---\n");
    printf("Analyzing stream integrity...\n\n");

    while (fread(buffer, sizeof(unsigned char), BLOCK_SIZE, file) == BLOCK_SIZE) {
        // 1. Integrity Check (Anchor 0 and Signature 25)
        if (buffer[0] != 0x2A || buffer[25] != 0xFF) {
            printf("Frame %d: CORRUPT DATA DETECTED! Skipping...\n", frame_count);
            continue;
        }

        // 2. Extract Sensor (Bytes 2-3) - Currently holds our TICK counter!
        unsigned short current_sensor = (buffer[2] << (7 + 1)) | buffer[3];
        total_sensor_val += current_sensor;
        if (current_sensor > max_sensor) max_sensor = current_sensor;
        if (current_sensor < min_sensor) min_sensor = current_sensor;

        // 3. Extract Collisions (Bytes 22-23)
        last_collision_count = (buffer[22] << (7 + 1)) | buffer[23];

        // 4. Extract Message Tag (Bytes 15-17)
        char tag[4] = { buffer[15], buffer[16], buffer[17], '\0' };

        // 5. Extract Frame ID (Bytes 18-19)
        unsigned short frame_id = (buffer[17 + 1] << (7 + 1)) | buffer[19];

        // Print details for every 5000th frame so the console stays readable
        if (frame_count % 5000 == 0) {
            printf("Frame %-5d | ID: %04X | Tag: '%s' | Sensor: %04X | Collisions: %u\n", 
                   frame_count, frame_id, tag, current_sensor, last_collision_count);
        }

        frame_count++;
    }

    fclose(file);

    // Final Statistical Report
    if (frame_count > 0) {
        printf("\n--- FINAL AUDIT REPORT ---\n");
        printf("Total Frames Processed: %d\n", frame_count);
        printf("Average Sensor Value:   %llu\n", total_sensor_val / frame_count);
        printf("Peak Sensor Value:      %u\n", max_sensor);
        printf("Lowest Sensor Value:    %u\n", min_sensor);
        printf("Total Final Collisions: %u\n", last_collision_count);
        printf("Stream Health:          %s\n", (last_collision_count < 50) ? "STABLE" : "STRESSED");
        printf("--------------------------\n");
    } else {
        printf("No valid data found in file.\n");
    }

    return 0;
}