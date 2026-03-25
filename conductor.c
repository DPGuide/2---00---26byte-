#include <stdio.h>
#include <windows.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main() {
    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BLOCK_SIZE, SHM_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);
    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    // Hochpräzisions-Stoppuhr initialisieren
    LARGE_INTEGER freq, start_time, end_time;
    QueryPerformanceFrequency(&freq);

    printf("--- INTELLECTUAL PROPERTY: CONDUCTOR ---\n");
    printf("IGNITING MAX POWER... (Target: 100,000 Frames)\n");
    printf("Hardware Telemetry: ACTIVE (Measuring Mutex Wait Time)\n");

    unsigned int tick = 0;
    
    while(tick < 100000) {
        // 1. STOPPUHR START: Wie lange müssen wir warten?
        QueryPerformanceCounter(&start_time);
        
        WaitForSingleObject(hMutex, INFINITE);

        // 2. STOPPUHR STOP: Zeit in Mikrosekunden umrechnen!
        QueryPerformanceCounter(&end_time);
        long long wait_us = ((end_time.QuadPart - start_time.QuadPart) * 1000000) / freq.QuadPart;
        
        // Sicherheits-Cutoff, falls es länger als 65.535 us dauert
        unsigned short wait_short = (wait_us > 0xFFFF) ? 0xFFFF : (unsigned short)wait_us;

        // 3. PAKET BRENNEN
        buffer[0] = 0x2A;
        
        // ID (Tick)
        buffer[2] = (unsigned char)(tick >> (7 + 1));
        buffer[3] = (unsigned char)(tick & 0xFF);

        // NEU: Echter Hardware-Tag ("MTX" für Mutex-Contention)
        buffer[15] = 'M';
        buffer[16] = 'T';
        buffer[17] = 'X';

        // NEU: Die echte Wartezeit in Mikrosekunden eintragen!
        buffer[22] = (unsigned char)(wait_short >> (7 + 1));
        buffer[23] = (unsigned char)(wait_short & 0xFF);

        // Checksumme berechnen (Deck ab Byte 0 bis 20)
        unsigned char cs = 0;
        for(int j = 0; j < 21; j++) cs ^= buffer[j];
        buffer[22 - 1] = cs;

        // Schluss-Signatur
        buffer[25] = 0xFF;

        ReleaseMutex(hMutex);
        tick++;
    }

    // DER SCHLUSSAKKORD
    WaitForSingleObject(hMutex, INFINITE);
    buffer[0] = 0xFF; 
    ReleaseMutex(hMutex);
    
    Sleep(100);

    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hMutex);

    printf("Conductor finished. Stage cleared.\n");

    return 0;
}