:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  package.bat -- 打可分发源码 ZIP（逻辑只有一份 tools\package_source.ps1，两端共用）
REM  用法：scripts\package.bat [版本号]
setlocal
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "OUTDIR=%ROOT%\out\package"
if not exist "%OUTDIR%" md "%OUTDIR%" 2>nul

if "%~1"=="" (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\tools\package_source.ps1" -OutputDir "%OUTDIR%"
) else (
    powershell -NoProfile -ExecutionPolicy Bypass -File "%ROOT%\tools\package_source.ps1" -OutputDir "%OUTDIR%" -Version "%~1"
)
exit /b %errorlevel%

#__SH__
# POSIX 段（pwsh 探测 + 明确报错）
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

PS=$(command -v pwsh 2>/dev/null || true)
if [ -z "$PS" ]; then
    PS=$(command -v powershell 2>/dev/null || true)
fi
if [ -z "$PS" ]; then
    echo "[ERROR] 未找到 PowerShell，无法打包。" >&2
    echo "        Linux 请先安装 PowerShell 7（pwsh）：https://aka.ms/powershell" >&2
    exit 1
fi

mkdir -p out/package
if [ -n "${1:-}" ]; then
    "$PS" -NoProfile -File tools/package_source.ps1 -OutputDir out/package -Version "$1"
else
    "$PS" -NoProfile -File tools/package_source.ps1 -OutputDir out/package
fi
exit $?
