#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BLOCK_SIZE 26
#define PORT 9999

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(PORT);
    target.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    unsigned char packet[BLOCK_SIZE];
    printf("CHAOS-RIDER: Starting malicious packet injection...\n\n");

    // 1. PHASE: Die 4 gezielten Angriffe (Die Schleife)
    for(int attack = 0; attack < 4; attack++) {
        // Clear packet with 00
        for(int i = 0; i < BLOCK_SIZE; i++) packet[i] = 0x00;

        // Base setup (valid frame template)
        packet[0] = 0x2A; packet[1] = 0xC0;
        packet[24] = 0xEE; packet[25] = 0xFF;

        switch(attack) {
            case 0:
                printf("ATTACK 1: Sending a PERFECT packet (Control Group)...\n");
                packet[2] = 0x13; packet[3] = 0x37;
                // Correct Checksum for this specific packet
                unsigned char cs = 0;
                for(int j = 0; j < 21; j++) cs ^= packet[j];
                packet[22 - 1] = cs;
                break;

            case 1:
                printf("ATTACK 2: Sending WRONG ANCHOR (Security Check)...\n");
                packet[0] = 0x00; // Killing the Magic Number 2A
                break;

            case 2:
                printf("ATTACK 3: Manipulating SENSOR (Checksum Mismatch)...\n");
                packet[2] = 0xFF; // Data change
                packet[22 - 1] = 0x00; // Wrong checksum
                break;

            case 3:
                printf("ATTACK 4: Damaging END SIGNATURE (Integrity Check)...\n");
                packet[25] = 0x00; // Killing the FF at the end
                break;
        }

        // Feuer für den aktuellen Angriff frei!
        sendto(s, (char*)packet, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
        
        // Sleep between attacks (Avoid forbidden digit)
        Sleep(700 + 300); 
    }
    // --- HIER ENDET DIE SCHLEIFE ---


    // 2. PHASE: Der Mic Drop (Poison Pill)
    printf("\nSending POISON PILL (0xFF) to terminate the Listener...\n");
    
    // Paket leeren und den Schlussakkord auf Index 0 setzen
    for(int i = 0; i < BLOCK_SIZE; i++) packet[i] = 0x00;
    packet[0] = 0xFF; 
    
    // Das finale Signal senden
    sendto(s, (char*)packet, BLOCK_SIZE, 0, (struct sockaddr*)&target, sizeof(target));
    Sleep(100); // Kurz warten, damit das Paket sicher ankommt

    printf("Chaos-Rider has finished the assault.\n");
    closesocket(s);
    WSACleanup();
    return 0;
}