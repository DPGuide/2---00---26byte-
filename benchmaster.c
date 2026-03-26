#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define TOTAL_FRAMES 100000

typedef struct {
    char name[32];
    DWORD time;
    float fps;
} TestResult;

TestResult results[5];

void run_integrity_audit() {
    printf("\n[ FINAL STEP ] Initiating Deep Integrity Audit ...\n");
    Sleep(1000); 
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Startet deinen Audit direkt!
    if(CreateProcess(NULL, "audit.exe blackbox.bin", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
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
        ZeroMemory(&pi[i], sizeof(pi[i])); // WICHTIG: Tötet die "Geister-Prozesse"
    }

    printf("[ RUNNING ] %-15s ... ", mode_names[mode_idx]);

    int p_count = 0;
    int active_apps[5];

    if (mode_idx == 0) { p_count = 1; active_apps[0] = 0; } 
    else if (mode_idx == 1) { p_count = 2; active_apps[0] = 0; active_apps[1] = 1; } 
    else if (mode_idx == 2) { p_count = 1; active_apps[0] = 0; } 
    else if (mode_idx == 3) { p_count = 2; active_apps[0] = 0; active_apps[1] = 1; } 
    else if (mode_idx == 4) { p_count = 3; active_apps[0] = 0; active_apps[1] = 3; active_apps[2] = 4; }

    // 1. Empfänger starten (falls vorhanden)
    for(int i = 1; i < p_count; i++) {
        int app_idx = active_apps[i];
        CreateProcess(NULL, (char*)apps[app_idx], NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si[i], &pi[i]);
    }

    if (p_count > 1) Sleep(1000); // Blackbox Zeit geben

    DWORD start_time = GetTickCount();

    // 2. Conductor mit korrekten Parametern zünden
    char cmd[256];
    if (p_count == 1) {
        strcpy(cmd, "conductor.exe SOLO");
    } else {
        strcpy(cmd, "conductor.exe");
    }

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si[0], &pi[0])) {
        printf("FEHLER BEIM START! ");
    }

    // 3. Warten bis Conductor fertig ist
    if (pi[0].hProcess) WaitForSingleObject(pi[0].hProcess, INFINITE);

    // 4. Blackbox höflich beenden lassen
    for(int i = 1; i < p_count; i++) {
        if (pi[i].hProcess) WaitForSingleObject(pi[i].hProcess, 5000);
    }

    DWORD end_time = GetTickCount();

    // 5. Alles abräumen
    for(int i = 0; i < p_count; i++) {
        if (pi[i].hProcess) {
            TerminateProcess(pi[i].hProcess, 0);
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }
    }

    DWORD duration = end_time - start_time;
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
        Sleep(500); 
    }

    printf("\nMODUS           | TIME (ms) | FPS\n");
    printf("----------------+-----------+-----------\n");
    for(int i = 0; i < 5; i++) {
        printf("%-15s | %9lu | %10.2f\n", results[i].name, results[i].time, results[i].fps);
    }
    
    run_integrity_audit();

    printf("\nAudit complete. Press ENTER to close.\n");
    fflush(stdin);
    getchar();
    return 0;
}