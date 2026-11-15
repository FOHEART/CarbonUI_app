:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
setlocal
REM  gen_docs.bat -- Doxygen 文档生成（WARN_AS_ERROR，告警即失败）
REM  doxygen 路径：环境变量 DOXYGEN 可覆盖；Linux 用 PATH
if not defined DOXYGEN set "DOXYGEN=D:\autotools\doxygen-1.11.0\doxygen.exe"

for %%I in ("%~dp0..") do set "ROOT=%%~fI"
set "LOGFILE=%ROOT%\out\doxygen\doxygen_warnings.log"

if not exist "%DOXYGEN%" (
    echo [ERROR] doxygen not found: %DOXYGEN%
    exit /b 2
)

pushd "%ROOT%"
REM 告警日志是生成物，统一放 out\ 下（Doxyfile 的 WARN_LOGFILE 为相对路径）
if not exist "out\doxygen" md "out\doxygen" 2>nul
"%DOXYGEN%" Doxyfile
if errorlevel 1 (
    echo.
    echo [FAILED] doxygen reported warnings, see %LOGFILE%
    if exist "%LOGFILE%" type "%LOGFILE%"
    popd
    exit /b 1
)

echo.
echo [OK] html generated: %ROOT%\out\docs\html\index.html
start "" "%ROOT%\out\docs\html\index.html"
popd
exit /b 0

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

DOXYGEN_BIN=${DOXYGEN:-doxygen}
if ! command -v "$DOXYGEN_BIN" >/dev/null 2>&1; then
    echo "[ERROR] doxygen not found: $DOXYGEN_BIN" >&2
    echo "        请安装 doxygen（如 sudo apt install doxygen），或用 DOXYGEN=/path/to/doxygen 指定。" >&2
    exit 2
fi

mkdir -p out/doxygen
if ! "$DOXYGEN_BIN" Doxyfile; then
    echo ""
    echo "[FAILED] doxygen reported warnings, see out/doxygen/doxygen_warnings.log"
    [ -f out/doxygen/doxygen_warnings.log ] && cat out/doxygen/doxygen_warnings.log
    exit 1
fi

echo ""
echo "[OK] html generated: $ROOT/out/docs/html/index.html"
