#include <stdio.h>
#include <stdlib.h>

#define BLOCK_SIZE 26

// argc = Anzahl der Argumente, argv = Die Argumente selbst
int main(int argc, char *argv[]) {
    // Pruefen, ob der Pfad vom Batch-Skript uebergeben wurde
    if (argc < 2) {
        printf("[ AUDIT FEHLER ] Kein Pfad zur blackbox.bin uebergeben!\n");
        return 1;
    }

    const char* filepath = argv[1];
    FILE *file = fopen(filepath, "rb");
    
    if (!file) {
        printf("[ AUDIT FEHLER ] Konnte Datei nicht oeffnen: %s\n", filepath);
        return 1;
    }

    printf("--- INTELLECTUAL PROPERTY: DEEP INTEGRITY SCAN ---\n");
    printf("Target : %s\n\n", filepath);

    unsigned char buffer[BLOCK_SIZE];
    int frame_count = 0;
    int valid_frames = 0;

    // Lese die Datei Block fuer Block (je 26 Bytes) bis zum Ende
    while (fread(buffer, sizeof(unsigned char), BLOCK_SIZE, file) == BLOCK_SIZE) {
        frame_count++;
        
        // Verifiziere Anchor (0) und Signature (25)
        if (buffer[0] == 0x2A && buffer[25] == 0xFF) {
            valid_frames++;
            
            // Rekonstruiere die echte Frame-ID (Tick) aus Byte 2 und 3 (mit 7+1)
            unsigned short frame_id = (buffer[2] << (7 + 1)) | buffer[3];
            
            // Rekonstruiere Sensor (Wait-Time) aus Byte 22 und 23
            unsigned short sensor = (buffer[22] << (7 + 1)) | buffer[23];
            
            // Spam-Schutz: Wir drucken nur jeden 5000. Frame oder den Letzten
            if (frame_count % 5000 == 0 || frame_count == 100000) {
                printf("Frame %06d | SENSOR: %04X | Frame-ID: %04X [ VALID ]\n", 
                       frame_count, sensor, frame_id);
            }
        } else {
            printf("Frame %06d | [ CORRUPTED ]\n", frame_count);
        }
    }

    fclose(file);
    
    printf("\n[ FINAL RESULT ] %d / %d Frames erfolgreich verifiziert.\n", valid_frames, frame_count);
    
    // DER ENTSCHEIDENDE FIX: Ziel sind 100.000, nicht 100!
    if (valid_frames == 100000) {
        printf("SYSTEM STATUS  : PERFECT INTEGRITY\n");
        return 0; // Erfolg -> Batch-Skript zeigt SUCCESSFUL
    } else {
        printf("SYSTEM STATUS  : COMPROMISED\n");
        return 1; // Fehler -> Batch-Skript zeigt FAILED
    }
}