@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

pushd %_CWD%
set _CWD=%CD%
popd

:: setup VS environment variables
call "%VS120COMNTOOLS%\vsvars32.bat"

:: restores package and sets up 7zip executable variable
.nuget\nuget restore
set _7z="%CD%\packages\7-Zip.CommandLine.9.20.0\tools\7za.exe"
echo \033[96m_7z = %_7z% | cmdcolor

:: sets up Git directory variable
if "%GIT_BIN_DIR%" == "" (
    if not "%ProgramFiles(x86)%" == "" (
        set GIT_BIN_DIR="%ProgramFiles(x86)%\Git\bin\"
    ) else (
        set GIT_BIN_DIR="%ProgramFiles%\Git\bin\"
    )
    echo \033[96mGIT_BIN_DIR = !GIT_BIN_DIR! | cmdcolor
)

:: get Git version tag
for /f "delims=" %%a in ('"!GIT_BIN_DIR!\git" describe --tags') do @set CHP_VERSION_BARE=%%a
if "!CHP_VERSION_BARE!" == "" (
    echo "No Git tag set, exiting..."
    pause
    exit
)
echo \033[96mCHP_VERSION_BARE = !CHP_VERSION_BARE! | cmdcolor
echo.
pause

:: build x64
set CHP_VERSION="CHP_VERSION=L"%CHP_VERSION_BARE%-x64""
msbuild /t:Rebuild /property:Configuration=Release;Platform=x64
if not %ERRORLEVEL% == 0 goto fail

echo \033[92mCopying x64 files and building 7zip archive | cmdcolor
rmdir Releasex64 /s /q
mkdir Releasex64
mkdir Releasex64\CLRHostPlugin

copy CLRHostPlugin\bin\x64\Release\CLRHostPlugin.dll Releasex64\
copy CLRHostInterop\bin\x64\Release\CLRHost.Interop.dll Releasex64\CLRHostPlugin\

cd Releasex64
%_7z% a CHP-%CHP_VERSION_BARE%-x64.7z .
move CHP-%CHP_VERSION_BARE%-x64.7z ..
cd ..

:: build x86
set CHP_VERSION="CHP_VERSION=L"%CHP_VERSION_BARE%-x86""
msbuild /t:Rebuild /property:Configuration=Release;Platform=win32
if not %ERRORLEVEL% == 0 goto fail

echo \033[92mCopying x64 files and building 7zip archive | cmdcolor
rmdir Releasex86 /s /q
mkdir Releasex86
mkdir Releasex86\CLRHostPlugin

copy CLRHostPlugin\bin\x86\Release\CLRHostPlugin.dll Releasex86\
copy CLRHostInterop\bin\x86\Release\CLRHost.Interop.dll Releasex86\CLRHostPlugin\

cd Releasex86
%_7z% a CHP-%CHP_VERSION_BARE%-x86.7z .
move CHP-%CHP_VERSION_BARE%-x86.7z ..
cd ..

:: build Samples
echo \033[92mCopying sample files and building 7zip archive | cmdcolor
rmdir Samples /s /q
mkdir Samples
mkdir Samples\CLRCSharpSamplePlugin

xcopy CLRCSharpSamplePlugin\* Samples\CLRCSharpSamplePlugin /s
rmdir /s /q Samples\CLRCSharpSamplePlugin\bin
rmdir /s /q Samples\CLRCSharpSamplePlugin\obj

cd Samples
%_7z% a CHP-%CHP_VERSION_BARE%-Samples.7z .
move CHP-%CHP_VERSION_BARE%-Samples.7z ..
cd ..

:: clean folders
rmdir Releasex64 /s /q
rmdir Releasex86 /s /q
rmdir Samples /s /q

echo.
echo All done!
pause
exit

:fail
echo.
echo Failed! Error = %ERRORLEVEL%
pause
exit