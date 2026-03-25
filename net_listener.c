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

    // Increase UDP Receive Buffer for High-Speed (e.g., 2MB)
    int rcvbuf = 1024 * 1024 * 2;
    setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf, sizeof(rcvbuf));

    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    printf("--- INTELLECTUAL PROPERTY: UDP LISTENER ---\n");
    printf("Port %d | Checksum Validation: ON | Display: SILENT MODE\n\n", PORT);
    printf("Listening for high-speed incoming packets...\n");

    unsigned char incoming[BLOCK_SIZE];
    int total_valid = 0;
    int total_corrupt = 0;

    while(1) {
        int res = recvfrom(s, (char*)incoming, BLOCK_SIZE, 0, NULL, NULL);
        
        if (res == BLOCK_SIZE) {
            
            // DER SCHLUSSAKKORD (Poison Pill)
            // Wenn der Sender 0xFF schickt, brechen wir elegant ab!
            if (incoming[0] == 0xFF) {
                break;
            }

            // Recalculate Checksum for verification
            unsigned char check = 0;
            for(int i = 0; i < 21; i++) {
                check ^= incoming[i];
            }

            // Verify against the received checksum at index 21 (22 - 1)
            if (check == incoming[22 - 1] && incoming[0] == 0x2A && incoming[25] == 0xFF) {
				total_valid++;
			} else {
				total_corrupt++;
			}
        }
    }

    // Erst am ENDE spucken wir die Wahrheit aus!
    printf("==========================================\n");
    printf("TRANSMISSION COMPLETE (End Signal received)\n");
    printf("Total Valid Packets:   %d\n", total_valid);
    printf("Total Corrupt Packets: %d\n", total_corrupt);
    printf("==========================================\n");

    closesocket(s);
    WSACleanup();
    return 0;
}