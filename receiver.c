#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);

    if (!hMapFile || !hMutex) return 1;

    printf("RECEIVER: Waiting for Cache Handshake (0x07)...\n\n");

    while(1) {
        WaitForSingleObject(hMutex, INFINITE);

        // Check if there is new data in the cache (Status 0x07)
        if (buffer[7] == 0x07) {
            printf("NEW CACHE DATA: [ ");
            for(int i = 0; i < 7; i++) {
                printf("%02X ", buffer[7 + 1 + i]);
            }
            
            // --- THE INVALIDATION ---
            buffer[7] = 0x00; // Reset to 0x00 so Transmitter can write again
            printf("] -> Cache cleared/invalidated.\n");
        }

        ReleaseMutex(hMutex);
        
        // Slightly longer sleep to simulate processing time
        // and test the Transmitter's "skip" logic.
        Sleep(350); 
    }
    return 0;
}