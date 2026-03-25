#include <stdio.h>
#include <stdlib.h>

// ==========================================
// DER EMPFAENGER: Liest die .bin Datei
// ==========================================
void read_and_decode_file() {
    size_t block_size = 26;
    
    // RAM-Puffer fuer exakt einen Block vorbereiten (1 Byte = 00)
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return;

    printf("\n--- Lese 'mein_protokoll.bin' von der Festplatte ---\n\n");

    // Datei im binaeren Lesemodus oeffnen ("rb" = read binary)
    FILE *file = fopen("mein_protokoll.bin", "rb");
    
    if (file == NULL) {
        printf("Fehler: Konnte Datei nicht finden! Hast du sie vorher erstellt?\n");
        free(buffer);
        return;
    }

    int frame_counter = 1;

    // Das Herzstueck: fread() liest exakt 26 Bytes auf einmal.
    // Die Schleife laeuft automatisch so lange, bis die Datei zu Ende ist!
    while (fread(buffer, sizeof(unsigned char), block_size, file) == block_size) {
        printf(">>> Lese Frame %d (26 Bytes)...\n", frame_counter);

        // Header pruefen
        unsigned char magic_number = buffer[0];
        unsigned char command = buffer[1];

        if (magic_number != 0x2A) {
            printf("Warnung: Unbekanntes Protokoll in Frame %d! Ueberspringe...\n", frame_counter);
            continue; 
        }

        // Sensordaten rekonstruieren (Pures Pointer-Casting, keine Mathematik!)
        unsigned short received_sensor_value = 0;
        unsigned char *read_ptr = (unsigned char *)&received_sensor_value;
        
        read_ptr[1] = buffer[2];
        read_ptr[0] = buffer[3];

        // VRAM-Farben auslesen (ab Offset 10)
        size_t vram_offset = 10;
        unsigned char r = buffer[vram_offset + 0];
        unsigned char g = buffer[vram_offset + 1];
        unsigned char b = buffer[vram_offset + 2];
        unsigned char a = buffer[vram_offset + 3];

        // Ergebnis fuer jeden Frame sauber auf der Konsole ausgeben
        printf("    [Header] Start: %02X | Command: %02X\n", magic_number, command);
        printf("    [Daten]  Sensorwert (Hex): %04X\n", received_sensor_value);
        printf("    [VRAM]   Pixel 1 -> R:%02X, G:%02X, B:%02X, A:%02X\n\n", r, g, b, a);

        frame_counter++;
    }

    // Alles sauber schliessen und RAM freigeben
    fclose(file);
    free(buffer);
    
    printf("--- Datei erfolgreich und vollstaendig ausgelesen! ---\n");
}

// ==========================================
// DAS BETRIEBSSYSTEM
// ==========================================
int main() {
    read_and_decode_file();
    return 0;
}