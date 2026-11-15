@echo off
REM  pull_artifacts.bat - entry wrapper for pull_artifacts.ps1 (exit code passthrough)
REM
REM  Usage:
REM    scripts\ci\pull_artifacts.bat                          latest run, all artifacts
REM    scripts\ci\pull_artifacts.bat -RunId 123456            given run, all artifacts
REM    scripts\ci\pull_artifacts.bat -Name ui-artifacts-*     only matching artifacts
REM    scripts\ci\pull_artifacts.bat -Dest D:\tmp\art         custom output directory
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0pull_artifacts.ps1" %*
exit /b %ERRORLEVEL%
