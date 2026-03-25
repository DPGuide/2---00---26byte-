#include <stdio.h>
#include <stdlib.h>

// ==========================================
// 1. DER SENDER: Erstellt den 26-Byte-Block
// ==========================================
unsigned char* create_and_fill_block() {
    // Die einzige echte Änderung: Wir nutzen jetzt dein Zielformat!
    size_t block_size = 26; 
    
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return NULL;

    // Header (2 Bytes)
    buffer[0] = 0x2A; 
    buffer[1] = 0xC0; 
    
    // Sensordaten schreiben (Pures Pointer-Casting)
    unsigned short sensor_value = 0x1337;
    unsigned char *sensor_ptr = (unsigned char *)&sensor_value;
    buffer[2] = sensor_ptr[1]; 
    buffer[3] = sensor_ptr[0];

    // VRAM schreiben (ab Position 10)
    size_t vram_offset = 10;
    
    // Pixel 1
    buffer[vram_offset + 0] = 0xFF; // R 
    buffer[vram_offset + 1] = 0x00; // G 
    buffer[vram_offset + 2] = 0x7F; // B 
    buffer[vram_offset + 3] = 0xFF; // A 

    // Pixel 2
    buffer[vram_offset + 4] = 0xFF; // R
    buffer[vram_offset + 5] = 0xFF; // G
    buffer[vram_offset + 6] = 0xFF; // B
    buffer[vram_offset + 7] = 0xFF; // A 

    printf("SENDER:     26-Byte-Block erfolgreich im RAM generiert.\n");
    return buffer; 
}

// ==========================================
// 2. DER EMPFAENGER: Liest den 26-Byte-Block
// ==========================================
void read_payload_from_block(unsigned char *buffer) {
    printf("EMPFAENGER: Lese Protokoll aus echtem RAM aus...\n");
    printf("--------------------------------------------------\n");

    unsigned char magic_number = buffer[0];
    unsigned char command = buffer[1];

    if (magic_number != 0x2A) {
        printf("Fehler: Unbekanntes Protokoll! Block wird ignoriert.\n");
        return;
    }

    printf("[Header] Start-Signal: %02X (Verifiziert!)\n", magic_number);
    printf("[Header] Command-Code: %02X\n", command);

    // Sensordaten lesen
    unsigned short received_sensor_value = 0;
    unsigned char *read_ptr = (unsigned char *)&received_sensor_value;
    read_ptr[1] = buffer[2];
    read_ptr[0] = buffer[3];

    printf("[Daten]  Sensorwert (Hex): %04X\n", received_sensor_value);
    
    // VRAM-Farben auslesen
    size_t vram_offset = 10;
    unsigned char r = buffer[vram_offset + 0];
    unsigned char g = buffer[vram_offset + 1];
    unsigned char b = buffer[vram_offset + 2];
    unsigned char a = buffer[vram_offset + 3];

    printf("[VRAM]   Pixel 1 -> Rot: %02X, Gruen: %02X, Blau: %02X, Alpha: %02X\n", r, g, b, a);
    
    // Den gesamten 26-Byte-Block einmal optisch darstellen
    printf("\nSpeicher-Uebersicht (26 Bytes):\n[ ");
    for (size_t i = 0; i < 26; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("]\n");
}

// ==========================================
// 3. DAS BETRIEBSSYSTEM
// ==========================================
int main() {
    printf("\n--- Systemstart ---\n\n");

    unsigned char *echter_ram_block = create_and_fill_block();
    
    if (echter_ram_block != NULL) {
        printf("SYSTEM:     Block-Adresse %p uebergeben...\n\n", (void*)echter_ram_block);

        read_payload_from_block(echter_ram_block);
        
        free(echter_ram_block);
        printf("\nSYSTEM:     RAM-Block sicher freigegeben.\n");
    }
    
    return 0;
}