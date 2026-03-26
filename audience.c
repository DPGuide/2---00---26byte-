#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);

    if (!hMap || !hMutex) return 1;

    printf("FINAL AUDIENCE: Visualizing the Conductor's work...\n\n");

    while(1) {
        WaitForSingleObject(hMutex, INFINITE);

        if (buffer[0] == 0x2A && buffer[25] == 0xFF) {
            // Reconstruct Sensor (Using 7+1 instead of 8)
            unsigned short sensor = (buffer[2] << (7 + 1)) | buffer[3];
            
            printf("SENSOR: %04X | ", sensor);

            // Handshake Invalidation at Index 7
            if (buffer[7] == 0x07) {
                printf("CACHE: [OK] ");
                buffer[7] = 0x00; // Handshake: Audience clears the Cache
            } else {
                printf("CACHE: [..] ");
            }
            
            printf("| Frame: %02X%02X\n", buffer[17 + 1], buffer[19]);
        }

        ReleaseMutex(hMutex);
        Sleep(100); 
    }
    return 0;
}