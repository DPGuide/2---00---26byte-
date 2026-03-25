#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define BUFFER_CAPACITY 1000 
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
    
    if (!hMap || !hMutex) {
        printf("Error: Shared Memory or Mutex not found. Start Conductor first.\n");
        return 1;
    }

    unsigned char* shm_ptr = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, BLOCK_SIZE);
    if (!shm_ptr) return 1;

    FILE *log_file = fopen("blackbox.bin", "wb");
    if (!log_file) return 1;

    unsigned char *disk_buffer = (unsigned char*) malloc(BUFFER_CAPACITY * BLOCK_SIZE);
    int frames_in_buffer = 0;
    int total_logged = 0;
    unsigned short last_logged_id = 0xFFFF;

    printf("--- INTELLECTUAL PROPERTY: SMART-SYNC LOGGER ---\n");
    printf("Mode: Waiting for Conductor END Signal (0xFF)...\n\n");

    // Die intelligente Schleife: Wir lauschen, bis der Dirigent aufhört!
    while (1) {
        WaitForSingleObject(hMutex, INFINITE);

        // DER SCHLUSSAKKORD: Hat der Conductor 0xFF als Header gesetzt?
        if (shm_ptr[0] == 0xFF) {
            ReleaseMutex(hMutex);
            break; // Raus aus der Schleife, das Konzert ist vorbei!
        }

        unsigned short current_id = (shm_ptr[2] << (7 + 1)) | shm_ptr[3];

        // Normaler Frame (0x2A) und neue ID?
        if (shm_ptr[0] == 0x2A && current_id != last_logged_id) {
            
            memcpy(&disk_buffer[frames_in_buffer * BLOCK_SIZE], shm_ptr, BLOCK_SIZE);
            
            last_logged_id = current_id;
            frames_in_buffer++;
            total_logged++;

            if (frames_in_buffer >= BUFFER_CAPACITY) {
                fwrite(disk_buffer, BLOCK_SIZE, BUFFER_CAPACITY, log_file);
                frames_in_buffer = 0;
            }
        }

        ReleaseMutex(hMutex);

        if (current_id == last_logged_id) {
            Sleep(0); 
        }
    }

    // RETTUNG: Schreibe die restlichen Daten aus dem RAM auf die Festplatte
    if (frames_in_buffer > 0) {
        fwrite(disk_buffer, BLOCK_SIZE, frames_in_buffer, log_file);
    }

    printf("Conductor finished! Secured %d unique frames.\n", total_logged);

    // Sauber zumachen, damit die Dateigröße nicht 0 ist!
    fclose(log_file);
    free(disk_buffer);
    UnmapViewOfFile(shm_ptr);
    CloseHandle(hMap);
    CloseHandle(hMutex);
    
    return 0;
}