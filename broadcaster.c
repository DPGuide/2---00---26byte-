#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex"
#define PORT 9999

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(PORT);
    target.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    printf("--- INTELLECTUAL PROPERTY: UDP BROADCASTER ---\n");
    printf("Waiting for Conductor to ignite the Shared Memory...\n");

    HANDLE hMap = NULL;
    HANDLE hMutex = NULL;

    // FIX: Schreibrechte (ALL_ACCESS) statt nur Lese-Rechte (READ)
    while (!hMap || !hMutex) {
        if (!hMap) hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);
        if (!hMutex) hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);
        if (!hMap || !hMutex) Sleep(10); 
    }

    // FIX: Auch hier Schreibrechte (ALL_ACCESS)
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, BLOCK_SIZE);

    printf("Conductor connected! Streaming to Port %d (MAX POWER)...\n\n", PORT);

    unsigned short last_frame = 0xFFFF;
    int sent_count = 0;

    while(1) {
        WaitForSingleObject(hMutex, INFINITE);

        if (buffer[0] == 0xFF) {
            sendto(s, (char*)buffer, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
            buffer[0] = 0xAA; // ENDE-BESTÄTIGUNG ZURÜCK AN DEN CONDUCTOR
            ReleaseMutex(hMutex);
            break; 
        }

        if (buffer[0] == 0x2A) {
            unsigned short current_frame = (buffer[2] << (7 + 1)) | buffer[3];
            
            if (current_frame != last_frame) {
                sendto(s, (char*)buffer, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
                last_frame = current_frame;
                sent_count++;

                // --- HIER IST DEIN PING-PONG! ---
                buffer[0] = 0x00; // "Ich hab's ins Netz geworfen, weiter geht's!"
                // --------------------------------
            }
        }

        ReleaseMutex(hMutex);
        Sleep(0); 
    }

    printf("Conductor finished. Broadcasted %d packets to the network.\n", sent_count);

    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hMutex);
    closesocket(s);
    WSACleanup();
    
    return 0;
}