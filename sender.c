#include <stdio.h>
#include <stdlib.h>

unsigned char* create_and_fill_block(unsigned short frame) {
    size_t block_size = 26; 
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return NULL;

    buffer[0] = 0x2A; 
    buffer[1] = 0xC0; 
    
    unsigned short sensor_value = 0x1337 + frame;
    unsigned char *sensor_ptr = (unsigned char *)&sensor_value;
    buffer[2] = sensor_ptr[1]; 
    buffer[3] = sensor_ptr[0];

    size_t vram_offset = 10;
    buffer[vram_offset + 0] = 0xFF;           
    buffer[vram_offset + 1] = 0x00 + frame;   
    buffer[vram_offset + 2] = 0x7F;           
    buffer[vram_offset + 3] = 0xFF;           

    return buffer; 
}

int main() {
    printf("\n--- Starte Aufzeichnung des 26-Byte-Streams ---\n\n");

    FILE *file = fopen("mein_protokoll.bin", "wb");
    if (file == NULL) {
        printf("Fehler: Konnte Datei nicht erstellen!\n");
        return 1;
    }

    for (unsigned short frame = 1; frame < 6; frame++) {
        unsigned char *stream_block = create_and_fill_block(frame);
        if (stream_block != NULL) {
            fwrite(stream_block, sizeof(unsigned char), 26, file);
            printf("Frame %d (26 Bytes) auf Festplatte geschrieben.\n", frame);
            free(stream_block);
        }
    }
    
    fclose(file);
    printf("\n--- Aufzeichnung beendet! 'mein_protokoll.bin' wurde erschaffen. ---\n");
    return 0;
}