#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define TOTAL_FRAMES 100000
#define PACKET_SIZE 26

typedef struct {
    char name[20];
    float fps;
    DWORD time;
} TestResult;

TestResult results[4]; 

void run_integrity_audit() {
    printf("\n[ FINAL STEP ] Initiating Deep Integrity Audit ...\n");
    
    // Wir geben dem System eine kurze Verschnaufpause zum Aufräumen der Dateien
    Sleep(1000); 

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // Startet deinen deep_scan.exe
    if(CreateProcess(NULL, "deep_scan.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        printf("\nIntegrity Audit Completed.\n");
    } else {
        printf("Error: Could not find 'deep_scan.exe'.\n");
    }
}

void execute_test(int mode_idx) {
    const char* apps[] = { "conductor.exe", "blackbox.exe", "audience.exe", "net_listener.exe", "chaos_rider.exe" };
    const char* mode_names[] = { "SOLO RAM", "RAM + DISK", "VRAM LINK", "FULL SYSTEM" };
    
    int p_count = 0;
    if (mode_idx == 0) p_count = 1;
    else if (mode_idx == 1) p_count = 2;
    else if (mode_idx == 2) p_count = 1;
    else p_count = 5;

    PROCESS_INFORMATION pi[5];
    STARTUPINFO si[5];

    printf("[ RUNNING ] %-15s ... ", mode_names[mode_idx]);

    // 1. LOGISTIK-FIX: Zuerst ALLE ZUHÖRER starten (Index 1 bis Ende)
    for(int i = 1; i < p_count; i++) {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
        CreateProcess(NULL, (char*)apps[i], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si[i], &pi[i]);
    }

    // 2. WARTEN: Gib der Blackbox und Co. 300ms Zeit, hochzufahren und den RAM zu mappen
    if (p_count > 1) {
        Sleep(300);
    }

    // 3. SHOWTIME: Jetzt erst die Stoppuhr starten und den Dirigenten loslassen!
    DWORD start = GetTickCount();
    
    ZeroMemory(&si[0], sizeof(si[0]));
    si[0].cb = sizeof(si[0]);
    CreateProcess(NULL, (char*)apps[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si[0], &pi[0]);

    // VRAM Simulation Pfad
    if (mode_idx == 2) {
        for(int v = 0; v < TOTAL_FRAMES; v++) {
            for(int d = 0; d < 50; d++) { __asm__ __volatile__ ("nop"); }
        }
    }

    // Warte auf den Conductor
    WaitForSingleObject(pi[0].hProcess, INFINITE);

    // Warte auf die Blackbox (nur in den Modi, wo sie läuft)
    if (p_count >= 2) {
        WaitForSingleObject(pi[1].hProcess, 5000);
    }

    DWORD end = GetTickCount();

    // Alle restlichen Prozesse beenden
    for(int i = 0; i < p_count; i++) {
        TerminateProcess(pi[i].hProcess, 0);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    DWORD duration = end - start;
    if (duration == 0) duration = 1;
    
    results[mode_idx].time = duration;
    results[mode_idx].fps = (float)TOTAL_FRAMES / ((float)duration / 1000.0f);
    sprintf(results[mode_idx].name, "%s", mode_names[mode_idx]);

    printf("DONE (%lu ms)\n", duration);
}

int main() {
    printf("--- INTELLECTUAL PROPERTY: COMPLETE SYSTEM AUDIT ---\n");
    printf("[ READY ] Press ENTER to ignite the 4-stage validation sequence...");
    getchar();

    for(int m = 0; m < 4; m++) {
        execute_test(m);
        // Kurze Abkühlung für die CPU zwischen den Tests
        Sleep(500); 
    }

    // Ergebnistabelle ausgeben
    printf("\nMODUS           | TIME (ms) | FPS       \n");
    printf("----------------+-----------+-----------\n");
    for(int i = 0; i < 4; i++) {
        printf("%-15s | %9lu | %10.2f\n", results[i].name, results[i].time, results[i].fps);
    }

    // Der finale Audit (Deep Scan)
    run_integrity_audit();

    printf("\nAudit complete. Press ENTER to close.");
    fflush(stdin);
    getchar();
    return 0;
}