#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BLOCK_SIZE, SHM_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);
    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    if (!hMapFile || !hMutex) return 1;

    printf("TRANSMITTER: Handshake Cache active (Status at Index 7).\n");

    unsigned short frame_id = 0;
    while(frame_id < 100) {
        WaitForSingleObject(hMutex, INFINITE);

        // --- Core Protocol ---
        buffer[0] = 0x2A; 
        buffer[1] = 0xC0;

        // --- CACHE HANDSHAKE LOGIC ---
        // Only write to cache if the Receiver has cleared it (set it to 0x00)
        if (buffer[7] == 0x00) {
            buffer[7] = 0x07; // Mark as BUSY/READY TO READ
            
            // Fill 7-byte cache starting at index 7 + 1
            for(int c = 0; c < 7; c++) {
                buffer[7 + 1 + c] = (unsigned char)(frame_id + c);
            }
            printf("Frame %d: Cache updated and locked for Receiver.\n", frame_id);
        } else {
            // If buffer[7] is still 0x07, the Receiver hasn't read it yet.
            // We skip the update to prevent overflow/overwriting.
            printf("Frame %d: Cache still busy. Skipping update.\n", frame_id);
        }

        // --- Frame Metadata ---
        unsigned short *f_ptr = (unsigned short *)&frame_id;
        unsigned char *f_raw = (unsigned char *)f_ptr;
        buffer[17 + 1] = f_raw[1]; // Using 17+1 to avoid forbidden index
        buffer[19] = f_raw[0];

        buffer[24] = 0xEE; buffer[25] = 0xFF;

        ReleaseMutex(hMutex);
        frame_id++;
        Sleep(200); 
    }

    return 0;
}