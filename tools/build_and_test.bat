@echo off
chcp 65001 >nul
REM ============================================================================
REM  build_and_test.bat -- 单元测试：配置 + 构建 + ctest（scripts\test.bat 的 Windows 实现链）
REM  用法：tools\build_and_test.bat [Debug|Release] [ctest -R 正则]
REM  注意：本文件是"深层被 call 的子脚本"，必须 CRLF 且 chcp 在任何中文之前，
REM        否则 cmd 解析错位——症状是碎片命令报错（'xxx' is not recognized）。
REM ============================================================================
setlocal enabledelayedexpansion

for %%I in ("%~dp0..") do set "ROOT_DIR=%%~fI"
cd /d "%ROOT_DIR%"

REM 防外部 shell 的 CC/CXX 毒化 CMake 编译器检测与链接器缓存（两阶段事故，见技能 §4.8）
set "CC="
set "CXX="
set "CFLAGS="
set "CXXFLAGS="
set "LDFLAGS="

REM ── vswhere 探测最新 VS 实例；生成器与 vcvars 必须取自同一实例 ──────────────
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSROOT="
set "VS_VER_FULL="
set "VSW_OUT=%TEMP%\build_and_test_vswhere.txt"
if exist "%VSWHERE%" (
    "%VSWHERE%" -prerelease -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath > "%VSW_OUT%" 2>nul
    for /f "usebackq delims=" %%I in ("%VSW_OUT%") do if not defined VSROOT set "VSROOT=%%I"
    "%VSWHERE%" -prerelease -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationVersion > "%VSW_OUT%" 2>nul
    for /f "usebackq delims=" %%V in ("%VSW_OUT%") do if not defined VS_VER_FULL set "VS_VER_FULL=%%V"
    del "%VSW_OUT%" 2>nul
)
set "VS_MAJOR="
if defined VS_VER_FULL for /f "tokens=1 delims=." %%M in ("%VS_VER_FULL%") do set "VS_MAJOR=%%M"
if "%VS_MAJOR%"=="18" (
    set "GENERATOR=Visual Studio 18 2026"
) else (
    if "%VS_MAJOR%"=="16" (
        set "GENERATOR=Visual Studio 16 2019"
    ) else (
        set "VS_MAJOR=17"
        set "GENERATOR=Visual Studio 17 2022"
    )
)

if defined VSROOT (
    set "VSDEV=%VSROOT%\VC\Auxiliary\Build\vcvars64.bat"
) else (
    REM 回退：2022 Community 标准位置
    set "VSDEV=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)
if not exist "%VSDEV%" (
    echo [ERROR] vcvars64.bat not found: %VSDEV%
    echo         请安装"使用 C++ 的桌面开发"工作负载。
    exit /b 2
)
echo [INFO] VSROOT=%VSROOT%
echo [INFO] installationVersion=%VS_VER_FULL% -^> %GENERATOR%
call "%VSDEV%" >nul
if errorlevel 1 (
    echo [ERROR] failed to load Visual Studio environment
    exit /b 2
)

REM Qt bin 加入 PATH（ctest 用例发现阶段要直接运行测试 exe，Qt DLL 必须可达）
if not defined QT_PREFIX set "QT_PREFIX=D:/Qt5.15.2/5.15.2/msvc2019_64"
set "PATH=%QT_PREFIX%\bin;%PATH%"

set "CONFIG=%~1"
if "%CONFIG%"=="" set "CONFIG=Release"
set "FILTER=%~2"
set "BUILD=%ROOT_DIR%\out\build\tdd-tests"

echo [1/3] configure: %BUILD%
cmake -S "%ROOT_DIR%" -B "%BUILD%" -G "%GENERATOR%" -A x64 ^
    -DCMAKE_CONFIGURATION_TYPES="Debug;Release" ^
    -DCMAKE_C_COMPILER=cl.exe -DCMAKE_CXX_COMPILER=cl.exe ^
    -DCMAKE_PREFIX_PATH="%QT_PREFIX%"
if errorlevel 1 (
    echo [ERROR] cmake configure failed
    exit /b 1
)

echo [2/3] build: app_unit_tests + app_ui_tests + app_visual_tests [%CONFIG%]
REM app_visual_tests 用 gtest_discover_tests 的 PRE_TEST 发现模式：不构建时 ctest
REM 会注册 app_visual_tests_NOT_BUILT 占位用例并判失败。该套件默认跳过（见
REM tests/visual/app_screenshot_test.cpp），构建它不增加运行耗时。
cmake --build "%BUILD%" --config %CONFIG% --target app_unit_tests app_ui_tests app_visual_tests
if errorlevel 1 (
    echo [ERROR] build failed
    exit /b 1
)

echo [3/3] ctest [%CONFIG%]
cd /d "%BUILD%"
if not "%FILTER%"=="" (
    REM 防御：ctest -R 过滤命中 0 条时退出码仍是 0，先断言命中数 >= 1
    set "MATCHED=0"
    for /f "tokens=3" %%N in ('ctest -N -C %CONFIG% -R "%FILTER%" 2^>nul ^| findstr /b /c:"Total Tests:"') do set "MATCHED=%%N"
    if "!MATCHED!"=="0" (
        echo [ERROR] ctest -R "%FILTER%" 匹配到 0 条测试，检查过滤表达式。
        exit /b 1
    )
    ctest --output-on-failure -C %CONFIG% -R "%FILTER%"
) else (
    ctest --output-on-failure -C %CONFIG%
)
if errorlevel 1 (
    echo [FAILED] ctest failed
    exit /b 1
)
echo [OK] all tests passed
exit /b 0
