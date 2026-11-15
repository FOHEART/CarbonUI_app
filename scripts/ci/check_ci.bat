@echo off
REM  check_ci.bat - entry wrapper for check_ci.ps1 (exit code passthrough)
REM
REM  Usage:
REM    scripts\ci\check_ci.bat                     list recent workflow runs
REM    scripts\ci\check_ci.bat -Watch              wait for the latest run to finish
REM    scripts\ci\check_ci.bat -Watch -Workflow cmake-multi-platform.yml
REM    scripts\ci\check_ci.bat -RunId 123456       failed-log summary of one run
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0check_ci.ps1" %*
exit /b %ERRORLEVEL%
