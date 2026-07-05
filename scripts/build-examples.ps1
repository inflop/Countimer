<#
.SYNOPSIS
    Compile examples/ sketches with arduino-cli.
.EXAMPLE
    scripts/build-examples.ps1
    Builds every sketch under examples/.
.EXAMPLE
    scripts/build-examples.ps1 -Fqbn esp32:esp32:esp32 CountimerTest
    Builds a single example for a specific board.
#>
param(
    [Parameter(Position = 0, ValueFromRemainingArguments = $true)]
    [string[]]$Examples,
    [string]$Fqbn = "arduino:avr:uno"
)

$ErrorActionPreference = "Stop"

$RepoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$ExamplesDir = Join-Path $RepoRoot "examples"

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
    Write-Error "arduino-cli not found on PATH (see .claude/skills/verify-examples for install steps)"
    exit 1
}

if (-not $Examples -or $Examples.Count -eq 0) {
    $Examples = Get-ChildItem -Path $ExamplesDir -Directory | Select-Object -ExpandProperty Name
}

$status = 0
foreach ($name in $Examples) {
    $sketch = Join-Path $ExamplesDir $name
    if (-not (Test-Path $sketch -PathType Container)) {
        Write-Error "no such example: $name"
        $status = 1
        continue
    }

    Write-Host "==> Building $name ($Fqbn)"
    arduino-cli compile --fqbn $Fqbn --library "$RepoRoot" --output-dir (Join-Path $sketch "build") "$sketch"
    if ($LASTEXITCODE -ne 0) {
        $status = 1
    }
}

exit $status
