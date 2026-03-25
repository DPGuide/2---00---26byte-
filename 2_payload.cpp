#include <stdio.h>
#include <stdlib.h>

// Wir übergeben den echten Pointer auf unseren 42-Byte-Block
void read_payload_from_block(unsigned char *buffer) {
    printf("\nLese Protokoll aus echtem RAM aus...\n");
    printf("--------------------------------------------------\n");

    // ==========================================
    // 1. Header prüfen (RAM-STIL)
    // ==========================================
    unsigned char magic_number = buffer[0];
    unsigned char command = buffer[1];

    printf("[Header] Start-Signal: %02X\n", magic_number);
    printf("[Header] Command-Code: %02X\n", command);

    // Sicherheitsprüfung: Ist das wirklich unser 42-Protokoll?
    if (magic_number != 0x2A) {
        printf("Fehler: Unbekanntes Protokoll!\n");
        return;
    }

    // ==========================================
    // 2. Sensordaten rekonstruieren
    // ==========================================
    // Wir bereiten eine leere Variable vor
    unsigned short received_sensor_value = 0;
    
    // Wir nutzen wieder unseren Pointer-Trick, diesmal rückwärts!
    unsigned char *read_ptr = (unsigned char *)&received_sensor_value;

    // Wir greifen die einzelnen "00"-Blöcke aus dem Puffer und 
    // legen sie direkt in den Speicherplatz unserer Variablen.
    read_ptr[1] = buffer[2];
    read_ptr[0] = buffer[3];

    // Zur Kontrolle ausgeben: Es sollte wieder 1337 sein!
    printf("[Daten]  Sensorwert (Hex): %04X\n", received_sensor_value);
    
    // ==========================================
    // 3. VRAM-Farben auslesen
    // ==========================================
    size_t vram_offset = 10;
    
    // Wir lesen das erste Pixel aus
    unsigned char r = buffer[vram_offset + 0];
    unsigned char g = buffer[vram_offset + 1];
    unsigned char b = buffer[vram_offset + 2];
    unsigned char a = buffer[vram_offset + 3];

    printf("[VRAM]   Pixel 1 -> Rot: %02X, Gruen: %02X, Blau: %02X, Alpha: %02X\n", r, g, b, a);
}