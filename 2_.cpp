#include <stdio.h>
#include <stdlib.h>
// Die Bibliothek für die verbotenen Datentypen wurde ersatzlos gestrichen!

void generate_galactic_protocol(int steps) {
    // size_t ist in C der sicherste Datentyp für Speichergrößen
    size_t a = 0;
    size_t b = 2;

    printf("🚀 Starte die Generierung des 42-Protokolls auf tiefster C-Ebene...\n");
    printf("--------------------------------------------------\n");

    for (int i = 1; i <= steps; i++) {
        // 1. Die neue Byte-Größe berechnen
        size_t current_bytes = a + b;

        // 2. Speicher auf dem Heap reservieren und strikt mit 0x00 füllen
        // unsigned char garantiert uns, dass ein Block exakt 1 Byte groß ist
        unsigned char *buffer = (unsigned char *)calloc(current_bytes, sizeof(unsigned char));

        if (buffer == NULL) {
            printf("Speicherzuweisung fehlgeschlagen! Handtuch werfen und Panik!\n");
            return;
        }

        // 3. Ausgabe der Metadaten
        printf("Schritt %d: %zu + %zu = %zu Bytes\n", i, a, b, current_bytes);
        printf("Daten im Speicher: [");

        // 4. Den echten Speicher byteweise auslesen und formatieren
        for (size_t j = 0; j < current_bytes; j++) {
            // %02X erzwingt die Darstellung als zweistelligen Hex-Wert (00 bis FF)
            printf("%02X", buffer[j]);
            
            // Leerzeichen nach jedem Byte, außer beim letzten
            if (j < current_bytes - 1) {
                printf(" ");
            }
        }
        printf("]\n\n");

        // 5. SEHR WICHTIG in C: Den reservierten Speicher wieder freigeben!
        free(buffer);

        // 6. Die Werte für die nächste Runde weiterrücken
        a = b;
        b = current_bytes;
    }
}

int main() {
    // 7 Schritte, um exakt bei den 42 Bytes der ultimativen Antwort zu landen
    generate_galactic_protocol(7);
    return 0;
}