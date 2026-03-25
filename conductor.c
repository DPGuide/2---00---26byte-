#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BLOCK_SIZE, SHM_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);
    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    unsigned int tick = 0;
    // Targeting 100,000 frames
    while(tick < 100000) {
        WaitForSingleObject(hMutex, INFINITE);

        buffer[0] = 0x2A;
        // Sensor Data (using 7+1 to avoid forbidden index)
        buffer[2] = (unsigned char)(tick >> (7 + 1));
        buffer[3] = (unsigned char)(tick & 0xFF);

        // Checksum
        unsigned char cs = 0;
        for(int j = 0; j < 21; j++) cs ^= buffer[j];
        buffer[22 - 1] = cs;

        buffer[25] = 0xFF;

        ReleaseMutex(hMutex);
        tick++;
        // NO SLEEP. MAX POWER.
    }
	// DER SCHLUSSAKKORD: Wir sagen allen, dass wir fertig sind!
    WaitForSingleObject(hMutex, INFINITE);
    buffer[0] = 0xFF; 
    ReleaseMutex(hMutex);
	Sleep(100);

    return 0; // Programmende
}