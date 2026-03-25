#include <stdio.h>
#include <winsock2.h>
#include <windows.h> // WICHTIG für Mutex und Sleep

#pragma comment(lib, "ws2_32.lib")

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define MUTEX_NAME "Local\\MyProtocolMutex" // Mutex nicht vergessen!
#define PORT 9999

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(PORT);
    target.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);
    HANDLE hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_NAME);

    if (!hMap || !hMutex) {
        printf("Broadcaster: Conductor or Mutex not found. Start conductor.exe first!\n");
        return 1;
    }

    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, BLOCK_SIZE);

    printf("--- INTELLECTUAL PROPERTY: UDP BROADCASTER ---\n");
    printf("Streaming 26-byte 'WC-Packets' to Port %d (MAX POWER)...\n\n", PORT);

    unsigned short last_frame = 0xFFFF;
    int sent_count = 0;

    while(1) {
        WaitForSingleObject(hMutex, INFINITE);

        // DER SCHLUSSAKKORD (Poison Pill)
        if (buffer[0] == 0xFF) {
            // Wir feuern das 0xFF auch übers Netzwerk, damit der Listener abschaltet!
            sendto(s, (char*)buffer, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
            ReleaseMutex(hMutex);
            break; 
        }

        // KORREKT: ID aus Byte 2 und 3 extrahieren
        unsigned short current_frame = (buffer[2] << (7 + 1)) | buffer[3];

        // Nur neue und gültige Frames (0x2A) versenden
        if (buffer[0] == 0x2A && current_frame != last_frame) {
            sendto(s, (char*)buffer, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
            last_frame = current_frame;
            sent_count++;
        }

        ReleaseMutex(hMutex);

        // Keine 10ms Bremse! Wir atmen nur eine Nanosekunde durch.
        if (current_frame == last_frame) {
            Sleep(0); 
        }
    }

    printf("Conductor finished. Broadcasted %d packets to the network.\n", sent_count);

    UnmapViewOfFile(buffer);
    CloseHandle(hMap);
    CloseHandle(hMutex);
    closesocket(s);
    WSACleanup();
    
    return 0;
}