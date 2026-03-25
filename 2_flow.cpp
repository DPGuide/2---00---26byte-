#include <stdio.h>
#include <stdlib.h>

// ==========================================
// 1. DER SENDER: Generiert den fließenden Stream
// ==========================================
unsigned char* create_and_fill_block(unsigned short frame) {
    size_t block_size = 26; 
    
    // Frischer, reiner Speicher (1 Byte = 00)
    unsigned char *buffer = (unsigned char *)calloc(block_size, sizeof(unsigned char));
    if (buffer == NULL) return NULL;

    // Header (2 Bytes) bleiben immer unser konstanter Anker
    buffer[0] = 0x2A; 
    buffer[1] = 0xC0; 
    
    // Dynamische Sensordaten: Der Wert steigt mit jedem Frame an!
    unsigned short sensor_value = 0x1337 + frame;
    unsigned char *sensor_ptr = (unsigned char *)&sensor_value;
    buffer[2] = sensor_ptr[1]; 
    buffer[3] = sensor_ptr[0];

    // Dynamischer VRAM: Die Farbe "pulsiert" im Datenfluss
    size_t vram_offset = 10;
    
    buffer[vram_offset + 0] = 0xFF;           // R (Rot voll an)
    buffer[vram_offset + 1] = 0x00 + frame;   // G (Grün wird mit jedem Frame heller!)
    buffer[vram_offset + 2] = 0x7F;           // B (Konstant)
    buffer[vram_offset + 3] = 0xFF;           // A (Sichtbar)

    return buffer; 
}

// ==========================================
// 2. DER EMPFAENGER: Liest die Pakete in Echtzeit
// ==========================================
void read_payload_from_block(unsigned char *buffer, unsigned short frame) {
    unsigned char magic_number = buffer[0];

    // Der wichtigste Schutzmechanismus bei echten Streams
    if (magic_number != 0x2A) {
        printf("Kritischer Fehler im Stream bei Frame %d!\n", frame);
        return;
    }

    // Sensordaten direkt aus dem RAM ziehen
    unsigned short received_sensor_value = 0;
    unsigned char *read_ptr = (unsigned char *)&received_sensor_value;
    read_ptr[1] = buffer[2];
    read_ptr[0] = buffer[3];
    
    // VRAM-Farben auslesen
    size_t vram_offset = 10;
    unsigned char r = buffer[vram_offset + 0];
    unsigned char g = buffer[vram_offset + 1];
    unsigned char b = buffer[vram_offset + 2];
    
    // Wir geben nur die veränderten Daten aus, um den Flow zu visualisieren
    printf("FRAME %d empfangen | Sensor: %04X | VRAM (R,G,B): %02X, %02X, %02X\n", 
           frame, received_sensor_value, r, g, b);
}

// ==========================================
// 3. DAS BETRIEBSSYSTEM: Der Kreislauf
// ==========================================
int main() {
    printf("\n--- Starte 26-Byte Daten-Stream ---\n\n");

    // Wir generieren einen Stream aus 5 aufeinanderfolgenden Paketen
    for (unsigned short frame = 1; frame < 6; frame++) {
        
        // 1. Block wird im RAM erschaffen und gefüllt
        unsigned char *stream_block = create_and_fill_block(frame);
        
        if (stream_block != NULL) {
            // 2. Block wird direkt an den Empfänger durchgereicht
            read_payload_from_block(stream_block, frame);
            
            // 3. Block wird sofort wieder vernichtet, der RAM ist wieder frei!
            free(stream_block);
        }
    }
    
    printf("\n--- Stream erfolgreich und sauber beendet ---\n");
    return 0;
}