:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
setlocal EnableExtensions
REM  git_pull_latest.bat -- 拉取 origin/main 最新代码
for %%I in ("%~dp0..") do set "ROOT_DIR=%%~fI"
cd /d "%ROOT_DIR%"

echo [fetch_latest] Fetching latest from origin...
git fetch origin
if errorlevel 1 goto :fetch_failed

git pull origin main
if errorlevel 1 goto :pull_failed

echo.
echo [fetch_latest] Done. Current status:
git --no-pager log --oneline -5
exit /b 0

:fetch_failed
echo [ERROR] git fetch failed.
exit /b 1

:pull_failed
echo [ERROR] git pull failed.
exit /b 1

#__SH__
cd "$(dirname "$0")/.." || exit 1

echo "[fetch_latest] Fetching latest from origin..."
git fetch origin || { echo "[ERROR] git fetch failed."; exit 1; }

git pull origin main || { echo "[ERROR] git pull failed."; exit 1; }

echo ""
echo "[fetch_latest] Done. Current status:"
git --no-pager log --oneline -5
