:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前。
REM  build.bat -- 配置 + 编译 + 部署（按配置循环）
REM  用法：scripts\build.bat [2022|2026] [Debug|Release]
setlocal EnableDelayedExpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "VS_VER=2022"
set "CONFIG=all"
if not "%~1"=="" for %%A in (%*) do (
    if /i "%%~A"=="2022" set "VS_VER=2022"
    if /i "%%~A"=="2026" set "VS_VER=2026"
    if /i "%%~A"=="Debug" set "CONFIG=Debug"
    if /i "%%~A"=="Release" set "CONFIG=Release"
)

if "%VS_VER%"=="2026" (
    set "GENERATOR=Visual Studio 18 2026"
    set "VS_TAG=Visual Studio 2026"
) else (
    set "GENERATOR=Visual Studio 17 2022"
    set "VS_TAG=Visual Studio 2022"
)

REM Qt 前缀：环境变量 QT_PREFIX 可覆盖（bin 目录会加入 PATH，供 windeployqt/测试使用）
if not defined QT_PREFIX set "QT_PREFIX=D:/Qt5.15.2/5.15.2/msvc2019_64"
set "PATH=%QT_PREFIX%\bin;%PATH%"

echo [1/2] 构建（%VS_TAG%，配置：%CONFIG%）...
if /i "%CONFIG%"=="all" (
    call :build_config Debug
    if errorlevel 1 goto :build_failed
    call :build_config Release
    if errorlevel 1 goto :build_failed
) else (
    call :build_config %CONFIG%
    if errorlevel 1 goto :build_failed
)

echo [2/2] 完成：out\build\x64-*（运行时已汇总到 out\install\x64-*）
exit /b 0

REM ── 配置 + 编译 + 部署指定配置 ───────────────────────────────────────────
:build_config
set "CFG=%~1"
set "BDIR=out\build\x64-%CFG%"
echo   -^> 配置 %BDIR%
cmake -S "%ROOT%" -B "%BDIR%" -G "%GENERATOR%" -A x64 ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -DCMAKE_PREFIX_PATH="%QT_PREFIX%"
if errorlevel 1 (
    echo CMake configuration failed, exit code: !errorlevel!
    exit /b 1
)

echo   -^> 编译 %CFG%
cmake --build "%BDIR%" --config %CFG%
if errorlevel 1 (
    echo Build failed, exit code: !errorlevel!
    exit /b 1
)

REM 构建成功后立刻转调 deploy.bat，"构建可用"与"部署可运行"始终同步
call "%~dp0deploy.bat" %CFG%
exit /b !errorlevel!

:build_failed
echo.
echo 构建未完成，请检查上面的错误输出。
exit /b 1

#__SH__
# POSIX 段（Linux：CMake 默认生成器，单配置）
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIGS=""
for arg in "$@"; do
    case "$arg" in
        [Dd]ebug)   CONFIGS="$CONFIGS Debug" ;;
        [Rr]elease) CONFIGS="$CONFIGS Release" ;;
        2022|2026)  echo "[INFO] Linux 下忽略参数 $arg（使用 CMake 默认生成器）" ;;
        *)          echo "[WARN] 忽略未知参数: $arg" ;;
    esac
done
[ -z "$CONFIGS" ] && CONFIGS="Release"

JOBS=$( (command -v nproc >/dev/null 2>&1 && nproc) || echo 4 )

build_one() {
    cfg=$1
    bdir="out/build/x64-$cfg"
    echo "  -> 配置 $bdir"
    cmake -S "$ROOT" -B "$bdir" -DCMAKE_BUILD_TYPE="$cfg" || return 1
    echo "  -> 编译 $cfg"
    cmake --build "$bdir" -j"$JOBS" || return 1
    sh "$(dirname "$0")/deploy.bat" "$cfg" || return 1
}

for cfg in $CONFIGS; do
    build_one "$cfg" || { echo ""; echo "构建未完成，请检查上面的错误输出。"; exit 1; }
done

echo "[2/2] 完成：out/build/x64-*（运行时已汇总到 out/install/x64-*）"
