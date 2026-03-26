#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"

int main(int argc, char *argv[]) {
    // Erkennt sofort, ob er vom Benchmaster als "SOLO" aufgerufen wurde
    int is_solo = (argc > 1) ? 1 : 0;

    HANDLE hMap = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BLOCK_SIZE, SHM_NAME);
    unsigned char* shm_buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);
    HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME);

    LARGE_INTEGER freq, start_time, end_time;
    QueryPerformanceFrequency(&freq);

    unsigned int tick = 0;
    unsigned char local_packet[BLOCK_SIZE];
    memset(shm_buffer, 0, BLOCK_SIZE);

    while(tick < 100000) {
        QueryPerformanceCounter(&start_time);
        WaitForSingleObject(hMutex, INFINITE);

        // Ping-Pong nur aktivieren, wenn die Blackbox auch da ist!
        if (!is_solo && shm_buffer[0] == 0x2A) {
            ReleaseMutex(hMutex);
            Sleep(0);
            continue;
        }

        QueryPerformanceCounter(&end_time);
        long long wait_us = ((end_time.QuadPart - start_time.QuadPart) * 1000000) / freq.QuadPart;
        unsigned short wait_short = (wait_us > 0xFFFF) ? 0xFFFF : (unsigned short)wait_us;

        memset(local_packet, 0, BLOCK_SIZE);
        local_packet[0] = 0x2A;
        local_packet[2] = (unsigned char)(tick >> (7 + 1));
        local_packet[3] = (unsigned char)(tick & 0xFF);
        local_packet[15] = 'M';
        local_packet[16] = 'T';
        local_packet[17] = 'X';
        local_packet[22] = (unsigned char)(wait_short >> (7 + 1));
        local_packet[23] = (unsigned char)(wait_short & 0xFF);

        unsigned char cs = 0;
        for(int j = 0; j < 21; j++) cs ^= local_packet[j];
        local_packet[21] = cs;
        local_packet[25] = 0xFF;

        memcpy(shm_buffer, local_packet, BLOCK_SIZE);
        ReleaseMutex(hMutex);
        tick++;
    }

    WaitForSingleObject(hMutex, INFINITE);
    shm_buffer[0] = 0xFF;
    ReleaseMutex(hMutex);

    // Wenn er alleine ist, wartet er NICHT auf eine Antwort!
    if (!is_solo) {
        int timeout = 0;
        while(shm_buffer[0] != 0xAA && timeout < 10) {
            Sleep(50);
            timeout++;
        }
    }

    UnmapViewOfFile(shm_buffer);
    CloseHandle(hMap);
    CloseHandle(hMutex);
    return 0;
}