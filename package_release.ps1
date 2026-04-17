[CmdletBinding()]
param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64",
    [string]$OutputPath = "package"
)

$ErrorActionPreference = "Stop"

function Copy-RequiredItem {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Source,
        [Parameter(Mandatory = $true)]
        [string]$Destination,
        [switch]$Recurse
    )

    if (-not (Test-Path -LiteralPath $Source)) {
        throw "Required package input not found: $Source"
    }

    if ($Recurse) {
        Copy-Item -LiteralPath $Source -Destination $Destination -Recurse -Force
    }
    else {
        Copy-Item -LiteralPath $Source -Destination $Destination -Force
    }
}

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRootFull = [System.IO.Path]::GetFullPath($repoRoot)
$packageRoot = if ([System.IO.Path]::IsPathRooted($OutputPath)) {
    [System.IO.Path]::GetFullPath($OutputPath)
}
else {
    [System.IO.Path]::GetFullPath((Join-Path $repoRoot $OutputPath))
}

$repoRootWithSeparator = $repoRootFull.TrimEnd('\', '/') + [System.IO.Path]::DirectorySeparatorChar
if (-not $packageRoot.StartsWith($repoRootWithSeparator, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputPath must be inside the repository: $packageRoot"
}

if (Test-Path -LiteralPath $packageRoot) {
    Remove-Item -LiteralPath $packageRoot -Recurse -Force
}

$buildRoot = Join-Path $repoRoot "build\$Platform\$Configuration"
$acuRoot = Join-Path $packageRoot "ACUFixes"
$pluginsRoot = Join-Path $acuRoot "plugins"

New-Item -ItemType Directory -Force -Path $pluginsRoot | Out-Null

Copy-RequiredItem `
    -Source (Join-Path $buildRoot "version.dll") `
    -Destination (Join-Path $packageRoot "version.dll")

Copy-RequiredItem `
    -Source (Join-Path $buildRoot "ACUFixes-PluginLoader.dll") `
    -Destination (Join-Path $acuRoot "ACUFixes-PluginLoader.dll")

Copy-RequiredItem `
    -Source (Join-Path $buildRoot "plugins\ACUFixes.dll") `
    -Destination (Join-Path $pluginsRoot "ACUFixes.dll")

Copy-RequiredItem `
    -Source (Join-Path $repoRoot "NewAnimations") `
    -Destination (Join-Path $pluginsRoot "NewAnimations") `
    -Recurse

Copy-RequiredItem `
    -Source (Join-Path $repoRoot "README.md") `
    -Destination (Join-Path $packageRoot "ACUFixesAndPluginLoader-readme.txt")

Write-Host "Package prepared: $packageRoot"
Get-ChildItem -LiteralPath $packageRoot -Recurse | ForEach-Object {
    Write-Host $_.FullName.Substring($packageRoot.Length + 1)
}
