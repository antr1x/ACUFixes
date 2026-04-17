[CmdletBinding()]
param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

function Get-MSBuildPath {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path -LiteralPath $vswhere) {
        $installationPath = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath
        if ($LASTEXITCODE -eq 0 -and $installationPath) {
            $candidate = Join-Path $installationPath "MSBuild\Current\Bin\amd64\MSBuild.exe"
            if (Test-Path -LiteralPath $candidate) {
                return $candidate
            }
        }
    }

    $fallbacks = @(
        "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\amd64\MSBuild.exe"
    )

    foreach ($candidate in $fallbacks) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw "MSBuild.exe not found."
}

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$solution = Join-Path $repoRoot "ACUFixes.sln"
$msbuild = Get-MSBuildPath

if (-not (Test-Path -LiteralPath $solution)) {
    throw "Solution not found: $solution"
}

Write-Host "MSBuild: $msbuild"
Write-Host "Solution: $solution"
Write-Host "Configuration: $Configuration | Platform: $Platform"

$target = if ($Clean) { "Clean;Build" } else { "Build" }

& $msbuild $solution `
    /m `
    /t:$target `
    "/p:Configuration=$Configuration" `
    "/p:Platform=$Platform" `
    /verbosity:minimal

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE."
}

$outputRoot = Join-Path $repoRoot "build\$Platform\$Configuration"

Write-Host ""
Write-Host "Build succeeded."
Write-Host "Output root: $outputRoot"
Write-Host "Plugin:      $(Join-Path $outputRoot 'plugins\ACUFixes.dll')"
Write-Host "Loader DLL:  $(Join-Path $outputRoot 'ACUFixes-PluginLoader.dll')"
Write-Host "Loader EXE:  $(Join-Path $outputRoot 'ACUFixesLoader.exe')"
Write-Host "Proxy DLL:   $(Join-Path $outputRoot 'version.dll')"
