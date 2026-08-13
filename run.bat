@echo off
setlocal

where gcc >nul 2>nul
if errorlevel 1 (
    echo gcc was not found. Install GCC or MinGW-w64, then run this script again.
    exit /b 1
)

gcc -O2 -std=c11 -Wall -Wextra -o numeric_diff_experiment.exe numeric_diff_experiment.c -lm
if errorlevel 1 exit /b 1

numeric_diff_experiment.exe
