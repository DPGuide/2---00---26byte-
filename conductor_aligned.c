#include <stdio.h>
#include <windows.h>

// 32 Bytes (4 * 8) for perfect memory alignment
#define ALIGNED_SIZE (4 * (7 + 1)) 
#define SHM_NAME "Local\\AlignedProtocol"
#define MUTEX_NAME "Local\\AlignedMutex"

int main() {
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, ALIGNED_SIZE, SHM_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, ALIGNED_SIZE);
    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    unsigned int tick = 0;
    while(tick < 100000) {
        WaitForSingleObject(hMutex, INFINITE);

        // Core data (26 Bytes remain the same)
        buffer[0] = 0x2A; 
        buffer[1] = 0xC0;
        
        // Data processing...
        buffer[25] = 0xFF;

        // The "Kofferraum" (Bytes 26 to 31)
        // Filled with 00, these ensure the NEXT frame starts at a multiple of 8
        for(int i = 26; i < ALIGNED_SIZE; i++) {
            buffer[i] = 0x00;
        }

        ReleaseMutex(hMutex);
        tick++;
    }
    return 0;
}