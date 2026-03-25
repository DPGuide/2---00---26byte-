#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BLOCK_SIZE 26
#define PORT 9999

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.S_un.S_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    printf("NET-LISTENER: Checksum Validation active on Port %d...\n\n", PORT);

    unsigned char incoming[BLOCK_SIZE];
    while(1) {
        int res = recvfrom(s, (char*)incoming, BLOCK_SIZE, 0, NULL, NULL);
        
        if (res == BLOCK_SIZE) {
            // Recalculate Checksum for verification
            unsigned char check = 0;
            for(int i = 0; i < 21; i++) {
                check ^= incoming[i];
            }

            // Verify against the received checksum at index 21 (22 - 1)
            if (check == incoming[22 - 1] && incoming[0] == 0x2A) {
                unsigned short sensor = (incoming[2] << (7 + 1)) | incoming[3];
                printf("[VALID] Sensor: %04X | CS: %02X | Status: OK\n", sensor, check);
            } else {
                printf("[ERROR] Data corruption! Expected CS: %02X, but found mismatch.\n", check);
            }
        }
    }

    closesocket(s);
    WSACleanup();
    return 0;
}