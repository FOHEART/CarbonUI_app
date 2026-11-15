@echo off
REM  rerun_failed.bat - entry wrapper for rerun_failed.ps1 (exit code passthrough)
REM
REM  Usage:
REM    scripts\ci\rerun_failed.bat                 re-run failed jobs of the latest run
REM    scripts\ci\rerun_failed.bat -RunId 123456   re-run failed jobs of a given run
REM    scripts\ci\rerun_failed.bat -DryRun         only print what would happen
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0rerun_failed.ps1" %*
exit /b %ERRORLEVEL%
