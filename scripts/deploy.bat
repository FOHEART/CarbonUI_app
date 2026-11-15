:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  deploy.bat -- 把运行时产物（exe + Qt DLL + Resources）从 out\build\output\%CONFIG% 汇总到 out\install\x64-%CONFIG%
REM  用法：scripts\deploy.bat [Debug|Release] [构建树目录]
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=all"
if /i "%CONFIG%"=="all" (
    call "%~f0" Debug
    if errorlevel 1 exit /b 1
    call "%~f0" Release
    exit /b !errorlevel!
)

set "BDIR=%~2"
if "%BDIR%"=="" set "BDIR=out\build\x64-%CONFIG%"
if not exist "%BDIR%\CMakeCache.txt" (
    echo [ERROR] 未找到构建树 %BDIR%，请先运行 scripts\build.bat。
    exit /b 1
)

set "INSTALL=out\install\x64-%CONFIG%"
set "RUNTIME=%ROOT%\out\build\output\%CONFIG%"
if not exist "%RUNTIME%\CarbonUI_app.exe" (
    echo [ERROR] 未找到运行时输出 %RUNTIME%\CarbonUI_app.exe（CMake 将产物输出到该目录）。
    exit /b 1
)

echo   -^> 汇总运行时到 %INSTALL%
if exist "%INSTALL%" rd /s /q "%INSTALL%"
md "%INSTALL%" 2>nul
xcopy /e /i /y /q "%RUNTIME%\*" "%INSTALL%\" >nul
if errorlevel 1 (
    echo [ERROR] 运行时汇总失败, exit code: !errorlevel!
    exit /b 1
)

set "EXE_COUNT=0"
set "DLL_COUNT=0"
for %%F in ("%INSTALL%\*.exe") do set /a EXE_COUNT+=1
for %%F in ("%INSTALL%\*.dll") do set /a DLL_COUNT+=1
echo      %INSTALL%：!EXE_COUNT! 个 exe，!DLL_COUNT! 个 DLL（含 Resources 目录）
exit /b 0

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIG=${1:-all}
if [ "$CONFIG" = "all" ]; then
    sh "$(dirname "$0")/deploy.bat" Debug || exit 1
    sh "$(dirname "$0")/deploy.bat" Release || exit 1
    exit 0
fi

BDIR=${2:-out/build-linux/x64-$CONFIG}
if [ ! -f "$BDIR/CMakeCache.txt" ]; then
    echo "[ERROR] 未找到构建树 $BDIR，请先运行 sh scripts/build.bat。"
    exit 1
fi

INSTALL="out/install/x64-$CONFIG"
RUNTIME="$ROOT/out/build-linux/output/$CONFIG"
if [ ! -e "$RUNTIME/CarbonUI_app.exe" ] && [ ! -e "$RUNTIME/CarbonUI_app" ]; then
    echo "[ERROR] 未找到运行时输出 $RUNTIME（CMake 将产物输出到该目录）。"
    exit 1
fi

echo "  -> 汇总运行时到 $INSTALL"
rm -rf "$INSTALL"
mkdir -p "$INSTALL"
cp -r "$RUNTIME/." "$INSTALL/" || { echo "[ERROR] 运行时汇总失败"; exit 1; }
echo "     $INSTALL：$(ls "$INSTALL" | grep -c -E 'CarbonUI_app(\.exe)?$' | tr -d ' ') 个可执行文件（含 Resources 目录）"
echo "     运行前可执行：export LD_LIBRARY_PATH=\"\$PWD\""
