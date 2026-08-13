$ErrorActionPreference = "Stop"

if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) {
    Write-Error "gcc was not found. Install GCC or MinGW-w64, then run this script again."
}

gcc -O2 -std=c11 -Wall -Wextra -o numeric_diff_experiment.exe numeric_diff_experiment.c -lm
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

.\numeric_diff_experiment.exe
