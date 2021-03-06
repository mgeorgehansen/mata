@REM Taken from https://github.com/pal1000/mesa-dist-win

@echo off

@set mesaloc="%TEMP%\mesa3d"
@mkdir "%mesaloc%" || exit /b 1

@echo Downloading mesa windows binaries...
@powershell -Command "Invoke-WebRequest https://github.com/pal1000/mesa-dist-win/releases/download/20.1.2/mesa3d-20.1.2-release-msvc.7z -OutFile mesa3d-20.1.2-release-msvc.7z" ^
  || exit /b 1
7z x mesa3d-20.1.2-release-msvc.7z -o"%mesaloc%" -y || exit /b 1
@echo Download complete.

@echo Installing Mesa binaries...
@cd /d "%mesaloc%" || exit /b 1
dir

@set mesainstalled=1
@IF NOT EXIST "%windir%\System32\mesadrv.dll" IF NOT EXIST "%windir%\System32\graw.dll" IF NOT EXIST "%windir%\System32\osmesa.dll" set mesainstalled=0

@IF /I %PROCESSOR_ARCHITECTURE%==X86 IF EXIST "%mesaloc%\x86\osmesa.dll" copy "%mesaloc%\x86\osmesa.dll" "%windir%\System32" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 IF EXIST "%mesaloc%\x86\osmesa.dll" copy "%mesaloc%\x86\osmesa.dll" "%windir%\SysWOW64" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 IF EXIST "%mesaloc%\x64\osmesa.dll" copy "%mesaloc%\x64\osmesa.dll" "%windir%\System32" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==X86 IF EXIST "%mesaloc%\x86\libglapi.dll" copy "%mesaloc%\x86\libglapi.dll" "%windir%\System32" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 IF EXIST "%mesaloc%\x86\libglapi.dll" copy "%mesaloc%\x86\libglapi.dll" "%windir%\SysWOW64" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 IF EXIST "%mesaloc%\x64\libglapi.dll" copy "%mesaloc%\x64\libglapi.dll" "%windir%\System32" || exit /b 1
set osmesatype=gallium
@IF /I %PROCESSOR_ARCHITECTURE%==X86 copy "%mesaloc%\x86\osmesa-%osmesatype%\osmesa.dll" "%windir%\System32" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 copy "%mesaloc%\x86\osmesa-%osmesatype%\osmesa.dll" "%windir%\SysWOW64" || exit /b 1
@IF /I %PROCESSOR_ARCHITECTURE%==AMD64 copy "%mesaloc%\x64\osmesa-%osmesatype%\osmesa.dll" "%windir%\System32" || exit /b 1
@echo.
@echo Installed MESA binaries.
