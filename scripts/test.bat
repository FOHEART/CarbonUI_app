:; exec sh -c "$(sed -n '/^#__SH__/,$p' "$0" | tr -d '\r')" "$0" "$@" #
@echo off
chcp 65001 >nul
REM  test.bat -- 构建并运行单元/界面测试（独立构建树 out\build\tdd-tests）
REM  用法：scripts\test.bat [Debug|Release] [ctest -R 正则]
setlocal
for %%I in ("%~dp0..") do set "ROOT=%%~fI"
call "%ROOT%\tools\build_and_test.bat" %*
exit /b %errorlevel%

#__SH__
# POSIX 段
ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd) || exit 1
cd "$ROOT" || exit 1

CONFIG=${1:-Release}
FILTER=${2:-}
BUILD="out/build-linux/tdd-tests"
JOBS=$( (command -v nproc >/dev/null 2>&1 && nproc) || echo 4 )

echo "[1/3] configure: $BUILD"
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE="$CONFIG" || { echo "[ERROR] cmake configure failed"; exit 1; }

echo "[2/3] build: app_unit_tests + app_ui_tests + app_visual_tests [$CONFIG]"
# app_visual_tests 必须一起构建：它用 gtest_discover_tests 的 PRE_TEST 发现模式，
# 不构建时 ctest 会注册一个 *_NOT_BUILT 占位用例并判失败。该套件本地默认跳过
# （见 tests/visual/app_screenshot_test.cpp 的开关说明），构建它不增加耗时。
cmake --build "$BUILD" -j"$JOBS" --target app_unit_tests app_ui_tests app_visual_tests || { echo "[ERROR] build failed"; exit 1; }

echo "[3/3] ctest [$CONFIG]"
cd "$BUILD" || exit 1
if [ -n "$FILTER" ]; then
    # 防御：ctest -R 过滤命中 0 条时退出码仍是 0，先断言命中数 >= 1
    total=$(ctest -N -C "$CONFIG" -R "$FILTER" 2>/dev/null | sed -n 's/^Total Tests: //p' | tr -d ' ')
    if [ "${total:-0}" -lt 1 ]; then
        echo "[ERROR] ctest -R \"$FILTER\" 匹配到 0 条测试，检查过滤表达式。"
        exit 1
    fi
    ctest --output-on-failure -C "$CONFIG" -R "$FILTER"
else
    ctest --output-on-failure -C "$CONFIG"
fi
rc=$?
if [ "$rc" -ne 0 ]; then
    echo "[FAILED] ctest exit code = $rc"
    exit "$rc"
fi
echo "[OK] all tests passed"
