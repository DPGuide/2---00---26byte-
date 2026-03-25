#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define TOTAL_FRAMES 100000
#define PACKET_SIZE (4 * (7 + 1)) // 32 Bytes

typedef struct {
    char name[20];
    float fps;
    DWORD time;
} TestResult;

int main() {
    printf("--- INTELLECTUAL PROPERTY: ALIGNMENT CHAMPION ---\n");
    printf("Testing 32-Byte Aligned Protocol (8-Byte logic)\n\n");

    // Start the conductor_aligned.exe (must be compiled!)
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    DWORD start = GetTickCount();
    CreateProcess(NULL, "conductor_aligned.exe", NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD end = GetTickCount();

    float sec = (float)(end - start) / 1000.0f;
    float fps = (float)TOTAL_FRAMES / (sec > 0 ? sec : 0.001f);
    float mbps = (fps * PACKET_SIZE) / (1024.0f * 1024.0f);

    printf("==========================================\n");
    printf("SOLO RAM (ALIGNED) | %lu ms | %.2f FPS\n", (end - start), fps);
    printf("Data Throughput    | %.4f MB/s\n", mbps);
    printf("==========================================\n");

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}