#include <stdio.h>
#include <stdlib.h>

void read_and_decode_file() {
    size_t block_size = 26;
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return;

    printf("\n--- Lese 'mein_protokoll.bin' von der Festplatte ---\n\n");

    FILE *file = fopen("mein_protokoll.bin", "rb");
    if (file == NULL) {
        printf("Fehler: Konnte Datei nicht finden!\n");
        free(buffer);
        return;
    }

    int frame_counter = 1;

    while (fread(buffer, sizeof(unsigned char), block_size, file) == block_size) {
        printf(">>> Lese Frame %d (26 Bytes)...\n", frame_counter);

        unsigned char magic_number = buffer[0];
        unsigned char command = buffer[1];

        if (magic_number != 0x2A) {
            printf("Warnung: Unbekanntes Protokoll! Ueberspringe...\n");
            continue; 
        }

        unsigned short received_sensor_value = 0;
        unsigned char *read_ptr = (unsigned char *)&received_sensor_value;
        read_ptr[1] = buffer[2];
        read_ptr[0] = buffer[3];

        size_t vram_offset = 10;
        unsigned char r = buffer[vram_offset + 0];
        unsigned char g = buffer[vram_offset + 1];
        unsigned char b = buffer[vram_offset + 2];
        unsigned char a = buffer[vram_offset + 3];

        printf("    [Header] Start: %02X | Command: %02X\n", magic_number, command);
        printf("    [Daten]  Sensorwert (Hex): %04X\n", received_sensor_value);
        printf("    [VRAM]   Pixel 1 -> R:%02X, G:%02X, B:%02X, A:%02X\n\n", r, g, b, a);

        frame_counter++;
    }

    fclose(file);
    free(buffer);
    printf("--- Datei erfolgreich und vollstaendig ausgelesen! ---\n");
}

int main() {
    read_and_decode_file();
    return 0;
}