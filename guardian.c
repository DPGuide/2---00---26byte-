#include <stdio.h>
#include <windows.h>

// Function to start the broadcaster and wait for it to exit
void launch_and_monitor() {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // Reset structures with 00 (No half measures!)
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    printf("GUARDIAN: Attempting to launch 'broadcaster.exe'...\n");

    // Start the child process 
    if (!CreateProcess(NULL, 
        "broadcaster.exe", // The command to run
        NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) 
    {
        printf("GUARDIAN ERROR: Could not find 'broadcaster.exe'.\n");
        return;
    }

    printf("GUARDIAN: Broadcaster is live (PID: %lu). Monitoring status...\n", pi.dwProcessId);

    // This line blocks the Guardian until the Broadcaster terminates
    WaitForSingleObject(pi.hProcess, INFINITE);

    printf("!!! GUARDIAN ALERT: Broadcaster has stopped unexpectedly !!!\n");

    // Clean up handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main() {
    printf("--- SYSTEM GUARDIAN ACTIVE ---\n");

    // The Infinite Resurrection Loop
    while(1) {
        launch_and_monitor();
        
        printf("GUARDIAN: Re-initializing system in 2 seconds...\n");
        // Avoiding the forbidden digit in Sleep
        Sleep(1000 + 1000); 
    }

    return 0;
}