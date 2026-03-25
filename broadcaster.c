#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link Windows Socket Library

#define BLOCK_SIZE 26
#define SHM_NAME "Local\\MyCustomProtocol"
#define PORT 9999

int main() {
    // 1. Initialize Windows Sockets
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // 2. Create UDP Socket
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // 3. Setup Target Address (Loopback for testing)
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(PORT);
    target.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    // 4. Connect to your existing Shared Memory
    HANDLE hMap = OpenFileMapping(FILE_MAP_READ, FALSE, SHM_NAME);
    unsigned char* buffer = (unsigned char*) MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, BLOCK_SIZE);

    if (!buffer) {
        printf("Broadcaster: Conductor not found. Start conductor.exe first!\n");
        return 1;
    }

    printf("BROADCASTER: Streaming 26-byte packets to Network (Port %d)...\n", PORT);

    unsigned short last_frame = 0xFFFF;
    while(1) {
        // We check the frame_id at index 17+1 and 19
        unsigned short current_frame = (buffer[17 + 1] << (7 + 1)) | buffer[19];

        // Only send if it's a new frame to save bandwidth
        if (current_frame != last_frame) {
            sendto(s, (char*)buffer, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
            last_frame = current_frame;
        }
        Sleep(10); // High-speed check
    }

    closesocket(s);
    WSACleanup();
    return 0;
}