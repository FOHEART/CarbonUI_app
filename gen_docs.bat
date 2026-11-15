@echo off
setlocal
chcp 65001 >nul
rem Doxygen 路径来自 D:\autotools 工具清单（docs/AutoTools.md）
set DOXYGEN=D:\autotools\doxygen-1.11.0\doxygen.exe
set ROOT=%~dp0

if not exist "%DOXYGEN%" (
    echo [ERROR] doxygen not found: %DOXYGEN%
    exit /b 2
)

pushd "%ROOT%"
"%DOXYGEN%" Doxyfile
if errorlevel 1 (
    echo.
    echo [FAILED] doxygen reported warnings, see doxygen_warnings.log
    type doxygen_warnings.log
    popd
    exit /b 1
)

echo.
echo [OK] html generated: %ROOT%docs\html\index.html
popd
endlocal
