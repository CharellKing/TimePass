@echo off
for /r . %%i in (Debug) do (
rd "%%i" /s /q
)
pause