@echo off

echo Bulding project files

@echo on
cd ..\VeiNetRoute\
call .\Programs\ThirdParty\premake\premake5.exe vs2022
@echo off

PAUSE
