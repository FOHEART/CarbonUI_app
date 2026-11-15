:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  chcp 必须在任何多字节内容之前。
REM  lint.bat -- clang-format / clang-tidy 代码检查（LLVM 23，全链路统一版本）
REM  用法：scripts\lint.bat [changed|all] [base-ref]
REM    changed（默认）：只检查相对 base-ref（默认 origin/main）的改动行，需先 git add 暂存
REM    all            ：全量只读扫描（报告 out\tidy\report.txt，统计告警分布）
REM  前置：LLVM 23 工具直接取用本地 clangTidy 工具目录（默认 D:\autotools\clangTidy，
REM    含 clang-format-23/clang-tidy-23_windows-amd64.exe，可用 CLANGTIDY_DIR 覆盖），
REM    无需另行下载 clang 二进制；仅 cpp-linter（改动行分析）经 pip 安装：
REM    python -m pip install --user cpp-linter
setlocal enabledelayedexpansion
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
cd /d "%ROOT%"

set "MODE=%~1"
if "%MODE%"=="" set "MODE=changed"
set "BASE=%~2"
if "%BASE%"=="" set "BASE=origin/main"

set "LINT_BUILD=out\build\lint"
set "LINT_OUT=out\lint"
if not exist "%LINT_OUT%" md "%LINT_OUT%" 2>nul

if /i "%MODE%"=="all" goto :all_mode
if /i not "%MODE%"=="changed" (
    echo [ERROR] 未知模式 "%MODE%"，可用：changed all
    exit /b 2
)

REM ── 确保编译数据库（clang-tidy 需要每个文件的编译器标志）─────────────────────
call :ensure_compile_db
if errorlevel 1 exit /b 1

REM cpp-linter 定位（pip --user 安装的入口不在 PATH，动态取 Python 用户 Scripts 目录）
for /f "usebackq delims=" %%P in (`python -c "import sysconfig; print(sysconfig.get_path('scripts', 'nt_user'))"`) do set "PYSBIN=%%P"
if defined PYSBIN set "PATH=!PYSBIN!;%PATH%"
where cpp-linter >nul 2>nul || (
    echo [ERROR] 未找到 cpp-linter。安装：python -m pip install --user cpp-linter
    exit /b 2
)

REM LLVM 23 工具统一取自 CLANGTIDY_DIR（版本号命名）。cpp-linter 按无版本名在
REM PATH 上查找，因此在 out\lint\bin 生成固定名副本，不污染工具目录。
if not defined CLANGTIDY_DIR set "CLANGTIDY_DIR=D:\autotools\clangTidy"
if not exist "!CLANGTIDY_DIR!\clang-tidy-23_windows-amd64.exe" (
    echo [ERROR] 未找到 clang-tidy-23_windows-amd64.exe：!CLANGTIDY_DIR!（可设 CLANGTIDY_DIR 覆盖）
    exit /b 2
)
if not exist "%LINT_OUT%\bin" md "%LINT_OUT%\bin" 2>nul
if not exist "%LINT_OUT%\bin\clang-format.exe" copy /y "!CLANGTIDY_DIR!\clang-format-23_windows-amd64.exe" "%LINT_OUT%\bin\clang-format.exe" >nul
if not exist "%LINT_OUT%\bin\clang-tidy.exe" copy /y "!CLANGTIDY_DIR!\clang-tidy-23_windows-amd64.exe" "%LINT_OUT%\bin\clang-tidy.exe" >nul
set "PATH=%LINT_OUT%\bin;%PATH%"

REM 改动行检查依赖 git 暂存区；GITHUB_ACTIONS=false 强制读 git diff 而非 GitHub API
set "GITHUB_ACTIONS=false"
set "LOG=%LINT_OUT%\cpp_linter.log"
echo [1/2] cpp-linter（仅改动行，base=%BASE%）...
cpp-linter --version=23 --style=file --tidy-checks="" ^
    --database="%LINT_BUILD%" --lines-changed-only=true --diff-base="%BASE%" ^
    > "%LOG%" 2>&1

type "%LOG%"
set "FAILED=0"
for /f "tokens=3 delims=:" %%N in ('findstr /c:"checks-failed" "%LOG%"') do (
    for /f %%M in ("%%N") do set "FAILED=%%M"
)
if "%FAILED%"=="0" (
    echo [OK] 0 checks-failed
    exit /b 0
)
echo [FAILED] %FAILED% checks-failed（明细见 %LOG%）
exit /b 1

REM ── 全量只读扫描：复用 clangTidy 技能的 check.bat（同一套 LLVM 23 工具）────────
:all_mode
call :ensure_compile_db
if errorlevel 1 exit /b 1
if not defined CLANGTIDY_DIR set "CLANGTIDY_DIR=D:\autotools\clangTidy"
set "SRC=%ROOT%"
set "BUILD=%ROOT%\%LINT_BUILD%"
echo [1/2] 全量 clang-tidy 扫描（排除 third_party/_deps/autogen）...
call "%CLANGTIDY_DIR%\scripts\check.bat"
exit /b %errorlevel%

REM ── 编译数据库生成：Ninja + vcvars（VS 生成器不支持导出 compile_commands.json）──
:ensure_compile_db
if exist "%LINT_BUILD%\compile_commands.json" (
    echo [INFO] 编译数据库已存在：%LINT_BUILD%\compile_commands.json
    exit /b 0
)
if not defined QT_PREFIX set "QT_PREFIX=D:/Qt5.15.2/5.15.2/msvc2019_64"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSROOT="
set "VSW_OUT=%TEMP%\lint_vswhere.txt"
if exist "%VSWHERE%" (
    "%VSWHERE%" -prerelease -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSW_OUT%" 2>nul
    for /f "usebackq delims=" %%I in ("%VSW_OUT%") do if not defined VSROOT set "VSROOT=%%I"
    del "%VSW_OUT%" 2>nul
)
if not defined VSROOT (
    set "VSROOT=C:\Program Files\Microsoft Visual Studio\2022\Community"
)
set "VSDEV=%VSROOT%\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VSDEV%" (
    echo [ERROR] vcvars64.bat not found: %VSDEV%
    exit /b 2
)
call "%VSDEV%" >nul
if errorlevel 1 (
    echo [ERROR] failed to load Visual Studio environment
    exit /b 2
)

echo [INFO] 生成编译数据库：%LINT_BUILD%
cmake -S "%ROOT%" -B "%LINT_BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_PREFIX_PATH="%QT_PREFIX%"
if errorlevel 1 (
    echo [ERROR] cmake configure failed
    exit /b 1
)
if not exist "%LINT_BUILD%\compile_commands.json" (
    echo [ERROR] compile_commands.json was not generated
    exit /b 1
)
exit /b 0

#__SH__
# POSIX 段（WSL/Linux）
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

MODE=${1:-changed}
BASE=${2:-origin/main}
LINT_BUILD="out/build/lint"
LINT_OUT="out/lint"
mkdir -p "$LINT_OUT"

# 编译数据库（Linux/WSL 上 Ninja 直接可用）
if [ ! -f "$LINT_BUILD/compile_commands.json" ]; then
    echo "[INFO] 生成编译数据库：$LINT_BUILD"
    cmake -S "$ROOT" -B "$LINT_BUILD" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON || { echo "[ERROR] cmake configure failed"; exit 1; }
fi

if ! command -v cpp-linter >/dev/null 2>&1; then
    echo "[ERROR] 未找到 cpp-linter。安装：" >&2
    echo "        pipx install cpp-linter && pipx install clang-tools && clang-tools install clang-format clang-tidy --version 23" >&2
    exit 2
fi

if [ "$MODE" = "all" ]; then
    echo "[1/2] 全量 clang-tidy 扫描（排除 third_party/_deps/autogen）..."
    python3 - "$LINT_BUILD/compile_commands.json" > "$LINT_OUT/files.txt" <<'PYEOF'
import json, sys
cc = json.load(open(sys.argv[1], encoding='utf-8'))
files = sorted({e['file'] for e in cc if 'third_party' not in e['file'] and '_deps' not in e['file'] and 'autogen' not in e['file']})
print('\n'.join(files))
PYEOF
    clang-tidy -p "$LINT_BUILD" "@$LINT_OUT/files.txt" > "$LINT_OUT/report.txt" 2>&1 || true
    w=$(grep -c ': warning: ' "$LINT_OUT/report.txt" || true)
    e=$(grep -c ': error: ' "$LINT_OUT/report.txt" || true)
    echo "     warnings: $w  errors: $e  报告：$LINT_OUT/report.txt"
    exit 0
fi

if [ "$MODE" != "changed" ]; then
    echo "[ERROR] 未知模式 \"$MODE\"，可用：changed all" >&2
    exit 2
fi

echo "[1/2] cpp-linter（仅改动行，base=$BASE）..."
GITHUB_ACTIONS=false cpp-linter --version=23 --style=file --tidy-checks='' \
    --database="$LINT_BUILD" --lines-changed-only=true --diff-base="$BASE" \
    2>&1 | tee "$LINT_OUT/cpp_linter.log"

# cpp-linter 失败也退出 0，必须解析日志计数
failed=$(sed -n 's/.*:\([0-9]\+\) checks-failed.*/\1/p' "$LINT_OUT/cpp_linter.log" | tail -n 1)
if [ "${failed:-0}" -eq 0 ]; then
    echo "[OK] 0 checks-failed"
    exit 0
fi
echo "[FAILED] ${failed} checks-failed（明细见 $LINT_OUT/cpp_linter.log）"
exit 1
