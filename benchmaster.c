#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define PACKET_SIZE 26
#define TOTAL_FRAMES 100000

typedef struct {
    char name[32]; // Mit dem 32-Byte Architekten-Puffer!
    DWORD time;
    float fps;
} TestResult;

TestResult results[5];

// DEINE ERFINDUNG: Der automatische Deep Scan am Ende! (Wieder sauber!)
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
    } else {
        printf("Error: Could not find 'deep_scan.exe'.\n");
    }
}

void execute_test(int mode_idx) {
    const char* apps[] = { "conductor.exe", "blackbox.exe", "chaos_rider.exe", "net_listener.exe", "broadcaster.exe" };
    const char* mode_names[] = { "SOLO RAM", "RAM + DISK", "VRAM LINK", "FULL SYSTEM", "NET STREAM" };

    PROCESS_INFORMATION pi[5];
    STARTUPINFO si[5];
    for(int i = 0; i < 5; i++) {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
    }

    printf("[ RUNNING ] %-15s ... ", mode_names[mode_idx]);

    int p_count = 0;
    int active_apps[5];

    // Logistik-Planung für die 5 Stufen
    if (mode_idx == 0) { p_count = 1; active_apps[0] = 0; } 
    else if (mode_idx == 1) { p_count = 2; active_apps[0] = 0; active_apps[1] = 1; } 
    else if (mode_idx == 2) { p_count = 1; active_apps[0] = 0; } 
    else if (mode_idx == 3) { p_count = 2; active_apps[0] = 0; active_apps[1] = 1; } 
    else if (mode_idx == 4) { p_count = 3; active_apps[0] = 0; active_apps[1] = 3; active_apps[2] = 4; } // Netz-Test!

    // Zuhörer (Blackbox oder Listener/Broadcaster) ZUERST starten
    for(int i = 1; i < p_count; i++) {
        int app_idx = active_apps[i];
        
        // In Stufe 4 öffnen wir neue Konsolen, damit du das UDP-Feuerwerk siehst!
        DWORD flags = (mode_idx == 4) ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;
        CreateProcess(NULL, (char*)apps[app_idx], NULL, NULL, FALSE, flags, NULL, NULL, &si[i], &pi[i]);
    }

    if (p_count > 1) Sleep(300);

    DWORD start = GetTickCount();

    CreateProcess(NULL, (char*)apps[0], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si[0], &pi[0]);

    if (mode_idx == 2) {
        for(int v = 0; v < TOTAL_FRAMES; v++) {
            for(int d = 0; d < 50; d++) { __asm__ __volatile__ ("nop"); }
        }
    }

    // 1. Warten bis der Conductor fertig ist
    WaitForSingleObject(pi[0].hProcess, INFINITE);

    // 2. NEU: Wir warten auch höflich auf die Blackbox und den Listener, 
    // damit sie ihre Arbeit sauber abschließen können und die .bin Datei nicht korrumpiert!
    for(int i = 1; i < p_count; i++) {
        WaitForSingleObject(pi[i].hProcess, 5000); // Maximal 5 Sekunden warten
    }

    DWORD end = GetTickCount();

    // 3. Alles sauber abräumen (jetzt ist es sicher!)
    for(int i = 0; i < p_count; i++) {
        TerminateProcess(pi[i].hProcess, 0); // Greift nur als Not-Aus
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
    printf("[ READY ] Press ENTER to ignite the 5-stage validation sequence...\n");
    getchar();

    for(int i = 0; i < 5; i++) {
        execute_test(i);
        // Kurze Abkühlung für die CPU zwischen den Tests
        Sleep(500); 
    }

    printf("\nMODUS           | TIME (ms) | FPS\n");
    printf("----------------+-----------+-----------\n");
    for(int i = 0; i < 5; i++) {
        printf("%-15s | %9lu | %10.2f\n", results[i].name, results[i].time, results[i].fps);
    }
    
    // Der finale Audit (Deep Scan)
    run_integrity_audit();

    printf("\nAudit complete. Press ENTER to close.\n");
    fflush(stdin);
    getchar();
    return 0;
}