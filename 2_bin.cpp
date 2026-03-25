#include <stdio.h>
#include <stdlib.h>

// ==========================================
// 1. DER SENDER: Erschafft den 26-Byte-Block
// ==========================================
unsigned char* create_and_fill_block(unsigned short frame) {
    size_t block_size = 26; 
    
    // Frischer RAM (1 Byte = 00)
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return NULL;

    // Header 
    buffer[0] = 0x2A; 
    buffer[1] = 0xC0; 
    
    // Sensordaten 
    unsigned short sensor_value = 0x1337 + frame;
    unsigned char *sensor_ptr = (unsigned char *)&sensor_value;
    buffer[2] = sensor_ptr[1]; 
    buffer[3] = sensor_ptr[0];

    // VRAM 
    size_t vram_offset = 10;
    buffer[vram_offset + 0] = 0xFF;           
    buffer[vram_offset + 1] = 0x00 + frame;   
    buffer[vram_offset + 2] = 0x7F;           
    buffer[vram_offset + 3] = 0xFF;           

    return buffer; 
}

// ==========================================
// 2. DAS BETRIEBSSYSTEM: Speichert auf Festplatte
// ==========================================
int main() {
    printf("\n--- Starte Aufzeichnung des 26-Byte-Streams ---\n\n");

    // Wir oeffnen eine Datei im binaeren Schreibmodus ("wb" = write binary)
    FILE *file = fopen("mein_protokoll.bin", "wb");
    
    if (file == NULL) {
        printf("Fehler: Konnte Datei nicht auf der Festplatte erstellen!\n");
        return 1;
    }

    printf("Datei 'mein_protokoll.bin' erfolgreich erstellt.\n");
    printf("Schreibe Pakete auf die Festplatte...\n");

    // Wir generieren 5 Frames und schreiben sie direkt in die Datei
    for (unsigned short frame = 1; frame < 6; frame++) {
        
        unsigned char *stream_block = create_and_fill_block(frame);
        
        if (stream_block != NULL) {
            // fwrite kopiert exakt unsere 26 Bytes aus dem RAM in die Datei
            fwrite(stream_block, sizeof(unsigned char), 26, file);
            
            printf("Frame %d (26 Bytes) sicher auf Festplatte geschrieben.\n", frame);
            
            // RAM sofort wieder freigeben, die Daten sind ja jetzt sicher auf der Disk!
            free(stream_block);
        }
    }
    
    // Datei sicher schliessen
    fclose(file);
    
    printf("\n--- Aufzeichnung beendet! ---\n");
    printf("Du hast jetzt eine echte, physische .bin Datei mit deinem eigenen Format erschaffen.\n");
    return 0;
}