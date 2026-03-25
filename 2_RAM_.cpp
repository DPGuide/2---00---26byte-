#include <stdio.h>
#include <stdlib.h>
// Die unnötige Bibliothek wurde restlos entfernt!

void write_payload_to_block() {
    size_t block_size = 42; // Unser ultimativer 42-Byte-Block
    
    // 1. Speicher allozieren mit purem unsigned char
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    
    if (buffer == NULL) return;

    printf("Ursprünglicher Speicher (genullt):\n");
    for(int i = 0; i < 14; i++) printf("%02X ", buffer[i]); 
    printf("... (und so weiter bis 42)\n\n");

    // ==========================================
    // 2. RAM-STIL: Protokoll-Header schreiben
    // ==========================================
    buffer[0] = 0x2A; // Magic Number (42 in Hex) als Start-Signal
    buffer[1] = 0xC0; // Command-Code 
    
    unsigned short sensor_value = 0x1337;
    unsigned char *sensor_ptr = (unsigned char *)&sensor_value;
    
    // Pures Pointer-Casting, komplett ohne Mathematik
    buffer[2] = sensor_ptr[1]; 
    buffer[3] = sensor_ptr[0];

    // ==========================================
    // 3. VRAM-STIL: Pixel-Daten (RGBA) schreiben
    // ==========================================
    size_t vram_offset = 10;
    
    // Pixel 1: Ein leuchtendes Violett
    buffer[vram_offset + 0] = 0xFF; // R 
    buffer[vram_offset + 1] = 0x00; // G 
    buffer[vram_offset + 2] = 0x7F; // B (Geändert, um die Ziffer zu verbannen!)
    buffer[vram_offset + 3] = 0xFF; // A 

    // Pixel 2: Ein reines Weiß direkt danach
    buffer[vram_offset + 4] = 0xFF; // R
    buffer[vram_offset + 5] = 0xFF; // G
    buffer[vram_offset + 6] = 0xFF; // B
    buffer[vram_offset + 7] = 0xFF; // A

    // ==========================================
    // 4. Ergebnis ausgeben
    // ==========================================
    printf("Speicher nach dem Schreiben der Nutzdaten:\n");
    for (size_t i = 0; i < block_size; i++) {
        if (i == 4) printf("\n  [Sensordaten] -> ");
        if (i == 10) printf("\n  [VRAM Pixel]  -> ");
        
        // Mathematik-Trick, um den Index für den Rest zu treffen, ohne die Ziffer zu tippen
        if (i == 10 + 4 + 4) printf("\n  [Leerer Rest] -> ");
        
        printf("%02X ", buffer[i]);
    }
    printf("\n");

    free(buffer);
}

int main() {
    write_payload_to_block();
    return 0;
}