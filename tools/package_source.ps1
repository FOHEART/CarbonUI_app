<#
package_source.ps1 - Build a distributable source ZIP for CarbonUI_app.

Single implementation shared by Windows (powershell.exe) and Linux (pwsh):
  1. Stage repo files with robocopy (Windows) or a recursive PowerShell copy
     (robocopy is Windows-only, so fall back automatically).
  2. Exclude build artifacts, internal CI/dev config and git traces
     (third_party is a git submodule - customers get dependencies via CMake,
     so it stays out of the source package as well).
  3. Leak validation: no .git* files, no git network keywords in shipped
     text files (the scanner itself is exempt).
  4. Compress with 7z (Command PATH -> D:\autotools\7z\7za.exe ->
     ProgramFiles\7-Zip) and fall back to Compress-Archive.

Usage:
  powershell -NoProfile -File tools\package_source.ps1 -OutputDir out\package [-Version 1.0.0]
#>
param(
    [string]$OutputDir = '',
    [string]$Version = '1.0.0'
)

$ErrorActionPreference = 'Stop'

$repoRoot = git rev-parse --show-toplevel 2>$null
if ([string]::IsNullOrEmpty($repoRoot)) { $repoRoot = Split-Path -Parent $PSScriptRoot }
Set-Location $repoRoot

if ([string]::IsNullOrEmpty($OutputDir)) { $OutputDir = Join-Path $repoRoot 'out/package' }
if (-not (Test-Path -LiteralPath $OutputDir))
{
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# Temp dir fallback: TEMP -> TMPDIR -> GetTempPath() (TEMP is Windows-only)
$tempRoot = $env:TEMP
if (-not $tempRoot) { $tempRoot = $env:TMPDIR }
if (-not $tempRoot) { $tempRoot = [System.IO.Path]::GetTempPath() }
if (-not $tempRoot) { throw 'cannot determine temp directory (TEMP/TMPDIR are both empty)' }
$stageParent = Join-Path $tempRoot "pack_stage_$PID"
if (Test-Path -LiteralPath $stageParent) { Remove-Item -LiteralPath $stageParent -Recurse -Force }
New-Item -ItemType Directory -Path $stageParent | Out-Null
$stageDir = Join-Path $stageParent 'CarbonUI_app'

# ---- excludes ---------------------------------------------------------------
# Join-Path must use '/' separators: '\' is a literal character on Linux.
$excludeDirNames = @('.git', '.vs', '.vscode', '.github', 'out', 'build',
                     'CMakeFiles', 'Testing', 'output', 'third_party', 'wsl', 'html')
$excludeDirAbsolute = @(
    (Join-Path $repoRoot 'out'),
    (Join-Path $repoRoot 'build'),
    (Join-Path $repoRoot 'output'),
    (Join-Path $repoRoot 'out/docs'),
    (Join-Path $repoRoot 'docs/html'),
    (Join-Path $repoRoot '.git'),
    (Join-Path $repoRoot '.github'),
    (Join-Path $repoRoot '.vscode'),
    (Join-Path $repoRoot '.vs'),
    (Join-Path $repoRoot 'third_party'),
    (Join-Path $repoRoot 'scripts/wsl'),
    (Join-Path $repoRoot 'scripts/ci'),
    (Join-Path $repoRoot 'scripts/git_pull_latest.bat')
)
$excludeFilePatterns = @('*.log', '*.zip', '*.user', '*.bak', '*.tmp',
                         '.gitignore', '.gitmodules', 'doxygen_warnings.log',
                         'git_pull_latest.bat')

function Copy-TreeWithExcludes
{
    param(
        [string]$Source,
        [string]$Dest,
        [string[]]$ExcludeDirNames,
        [string[]]$ExcludeFilePatterns,
        [string[]]$ExcludeAbsolutePaths
    )
    foreach ($item in Get-ChildItem -LiteralPath $Source -Recurse -Force)
    {
        $rel = $item.FullName.Substring($Source.Length).TrimStart('\', '/')
        $skip = $false
        foreach ($abs in $ExcludeAbsolutePaths)
        {
            if ($item.FullName -eq $abs -or $item.FullName.StartsWith($abs + [IO.Path]::DirectorySeparatorChar) -or $item.FullName.StartsWith($abs + '/')) { $skip = $true; break }
        }
        if (-not $skip)
        {
            $segs = $rel -split '[\\/]'
            # any ancestor directory (not the leaf itself for files) hitting an exclude name skips
            for ($s = 0; $s -lt $segs.Count - 1; $s++)
            {
                if ($ExcludeDirNames -contains $segs[$s]) { $skip = $true; break }
            }
        }
        if (-not $skip -and ($item.PSIsContainer -eq $false))
        {
            foreach ($pat in $ExcludeFilePatterns)
            {
                if ($item.Name -like $pat) { $skip = $true; break }
            }
        }
        if ($skip) { continue }
        $target = Join-Path $Dest $rel
        if ($item.PSIsContainer)
        {
            New-Item -ItemType Directory -Path $target -Force | Out-Null
        }
        else
        {
            New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
            Copy-Item -LiteralPath $item.FullName -Destination $target -Force
        }
    }
}

Write-Host "[1/3] Staging $repoRoot -> $stageDir"
$robocopy = Get-Command robocopy -ErrorAction SilentlyContinue
if ($robocopy)
{
    & robocopy $repoRoot $stageDir /E /XD @excludeDirAbsolute /XF @excludeFilePatterns /NFL /NDL /NJH /NJS | Out-Null
    if ($LASTEXITCODE -ge 8) { throw "copy failed (robocopy exit code $LASTEXITCODE)" }
}
else
{
    Write-Host '  robocopy not found (non-Windows), falling back to PowerShell copy'
    Copy-TreeWithExcludes -Source $repoRoot -Dest $stageDir `
        -ExcludeDirNames $excludeDirNames `
        -ExcludeFilePatterns $excludeFilePatterns `
        -ExcludeAbsolutePaths $excludeDirAbsolute
}

# ---- leak validation --------------------------------------------------------
Write-Host '[2/3] Leak validation (git traces / internal files)'
$leaks = Get-ChildItem -LiteralPath $stageDir -Recurse -Force |
    Where-Object { $_.Name -like '.git*' }
if ($leaks)
{
    throw ("git trace files leaked into package: " + (($leaks | ForEach-Object { $_.FullName }) -join '; '))
}

$scannerExempt = Join-Path $stageDir 'tools/package_source.ps1'
$textExt = @('.bat', '.ps1', '.sh', '.md', '.txt', '.dox', '.cmake', '.yml', '.json')
$hits = foreach ($f in (Get-ChildItem -LiteralPath $stageDir -Recurse -File |
        Where-Object { $textExt -contains $_.Extension -and $_.FullName -ne $scannerExempt }))
{
    if (Select-String -LiteralPath $f.FullName -Pattern 'git (fetch|pull|clone|push|remote|checkout|submodule)' -Quiet)
    {
        $f.FullName
    }
}
if ($hits)
{
    throw ("git network keywords leaked into package: " + ($hits -join '; '))
}

# ---- compress ---------------------------------------------------------------
Write-Host '[3/3] Compressing'
$zipPath = Join-Path $OutputDir ("CarbonUI_app_v{0}.zip" -f $Version)
if (Test-Path -LiteralPath $zipPath) { Remove-Item -LiteralPath $zipPath -Force }

$isWindowsPs = (-not $PSVersionTable.Platform) -or ($PSVersionTable.Platform -eq 'Win32NT')
$sevenZip = $null
if ($isWindowsPs)
{
    $sevenZip = (Get-Command 7z -ErrorAction SilentlyContinue).Source
    if (-not $sevenZip -and (Test-Path -LiteralPath 'D:\autotools\7z\7za.exe')) { $sevenZip = 'D:\autotools\7z\7za.exe' }
    if (-not $sevenZip) { $sevenZip = Join-Path ${env:ProgramFiles} '7-Zip\7z.exe' }
    if (-not (Test-Path -LiteralPath $sevenZip)) { $sevenZip = $null }
}
else
{
    $sevenZip = (Get-Command 7z -ErrorAction SilentlyContinue).Source
}

Push-Location $stageParent
try
{
    if ($sevenZip)
    {
        & $sevenZip a -tzip $zipPath CarbonUI_app | Out-Null
        if ($LASTEXITCODE -ne 0) { throw "7z failed with exit code $LASTEXITCODE" }
    }
    else
    {
        Compress-Archive -Path $stageDir -DestinationPath $zipPath -Force
    }
}
finally
{
    Pop-Location
    Remove-Item -LiteralPath $stageParent -Recurse -Force
}

$size = [Math]::Round((Get-Item -LiteralPath $zipPath).Length / 1KB, 1)
Write-Host "[OK] package ready: $zipPath ($size KB)"
