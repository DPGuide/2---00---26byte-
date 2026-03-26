@echo off
setlocal enabledelayedexpansion
cls
echo ======================================================
echo ===   INTELLECTUAL PROPERTY: FULL PIPELINE V1.0    ===
echo ======================================================
echo.

:: [0] KOMPILIERUNG
echo [0] Starte Kompilierung (GCC)...
gcc conductor.c -o conductor.exe
if %errorlevel% neq 0 goto build_error
gcc audience.c -o audience.exe
if %errorlevel% neq 0 goto build_error
gcc blackbox.c -o blackbox.exe
if %errorlevel% neq 0 goto build_error
gcc audit.c -o audit.exe
if %errorlevel% neq 0 goto build_error

echo [ OK ] Alle Binaries erfolgreich erstellt.
echo.

:: [1] VORBEREITUNG
if exist "blackbox.bin" (
    echo [1] Altes Log gefunden. Loesche 'blackbox.bin' fuer sauberen Run...
    del "blackbox.bin"
) else (
    echo [1] Keine alte Log-Datei vorhanden. Start frei.
)

:: ... [1] Altes Log loeschen ...

echo [2] Starte Black-Box (Wartet auf Daten)...
:: /B startet sie im Hintergrund, damit das Skript weiterlaeuft
start /B "" "blackbox.exe"

echo [3] Initialisiere Shared Memory (Warte 2 Sek)...
timeout /t 2 /nobreak > NUL

echo [4] Oeffne Audience Live-Monitor...
start /B "" "audience.exe"

echo [5] Starte Conductor (Taktgeber zündet!)...
:: Ohne /B, damit das Skript hier wartet, bis der Conductor fertig ist
start /WAIT "" "conductor.exe"

echo [6] Beende Live-Komponenten...
taskkill /F /IM "audience.exe" > NUL 2>&1
taskkill /F /IM "blackbox.exe" > NUL 2>&1

:: [7] Starte 5-Stage Validation Sequence...
echo.
echo [7] Starte 5-Stage Validation Sequence...
set "BLACKBOX_PATH=%~dp0blackbox.bin"

if not exist "%BLACKBOX_PATH%" (
    echo [ KRITISCH ] 'blackbox.bin' wurde nicht erstellt!
    goto cleanup
)

echo [ RUNNING ] audit.exe "%BLACKBOX_PATH%"
echo.
"audit.exe" "%BLACKBOX_PATH%"
set "AUDIT_RESULT=%errorlevel%"

echo.
if %AUDIT_RESULT% equ 0 (
    echo ======================================================
    echo ===   BENCHMARK SUCCESSFUL: INTEGRITY VERIFIED     ===
    echo ======================================================
) else (
    echo ======================================================
    echo ===   BENCHMARK FAILED: DATA CORRUPTION DETECTED   ===
    echo ======================================================
)

:cleanup
echo.
echo [8] Abschlusspflege...
taskkill /F /IM "conductor.exe" > NUL 2>&1
taskkill /F /IM "audience.exe" > NUL 2>&1
echo === ORCHESTRIERUNG ABGESCHLOSSEN ===
pause
exit /b

:build_error
echo.
echo [ FEHLER ] Kompilierung fehlgeschlagen! Bitte C-Code pruefen.
pause
exit /b 1