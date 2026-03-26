#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define BUFFER_CAPACITY 10000 
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    FILE *log_file = fopen("blackbox.bin", "wb");
    if (!log_file) return 1;

    printf("--- INTELLECTUAL PROPERTY: BLACKBOX ACTIVE ---\n");
    printf("[ SYSTEM ] Suche Conductor im RAM...\n");

    HANDLE hMap = NULL;
    HANDLE hMutex = NULL;

    // 1. WARTESCHLEIFE: Warten, bis der Conductor die Tür aufmacht
    while (!hMap || !hMutex) {
        if (!hMap) hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
        if (!hMutex) hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
        if (!hMap || !hMutex) Sleep(50);
    }

    printf("[ SYSTEM ] Conductor gefunden! Klinke mich ein...\n");

    unsigned char* shm_ptr = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);
    unsigned char *disk_buffer = (unsigned char*) malloc(BUFFER_CAPACITY * BLOCK_SIZE);

    int frames_in_buffer = 0;
    int total_logged = 0;

    while (1) {
        WaitForSingleObject(hMutex, INFINITE);

        // 2. ENDE-SIGNAL: Hat der Conductor 0xFF gesendet?
        if (shm_ptr[0] == 0xFF) {
            shm_ptr[0] = 0xAA; // Handshake zurück an Conductor ("Ich mach jetzt zu!")
            ReleaseMutex(hMutex);
            break;
        }

        // 3. DATEN-EMPFANG: Ist ein neuer Frame (0x2A) da?
        if (shm_ptr[0] == 0x2A) {
            memcpy(&disk_buffer[frames_in_buffer * BLOCK_SIZE], shm_ptr, BLOCK_SIZE);
            frames_in_buffer++;
            total_logged++;

            // --- DAS PING-PONG ---
            // Wir sagen dem Conductor: "Frame ist kopiert, schick den nächsten!"
            shm_ptr[0] = 0x00; 
            // ---------------------

            if (frames_in_buffer >= BUFFER_CAPACITY) {
                fwrite(disk_buffer, BLOCK_SIZE, BUFFER_CAPACITY, log_file);
                fflush(log_file);
                frames_in_buffer = 0;
            }
        }

        ReleaseMutex(hMutex);
        Sleep(0); // CPU kurz abgeben
    }

    // 4. RESTE RETTEN: Letzte Frames aus dem RAM auf die Festplatte zwingen
    if (frames_in_buffer > 0) {
        fwrite(disk_buffer, BLOCK_SIZE, frames_in_buffer, log_file);
        fflush(log_file);
    }

    printf("[ OK ] Aufnahme beendet. %d Frames gesichert.\n", total_logged);

    fclose(log_file);
    free(disk_buffer);
    UnmapViewOfFile(shm_ptr);
    CloseHandle(hMap);
    CloseHandle(hMutex);
    return 0;
}