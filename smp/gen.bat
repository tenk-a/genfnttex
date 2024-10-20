setlocal
pushd %~dp0

call :init
set RszMode=1
set "OPT=%OPT% -mul3 -cluttype=1:0x80 -resizemode=%RszMode% -addascii -bpp4"

set Prefix=
set Prefix=%RszMode%

:: Noto
set Sub=Sans
::set Sub=Serif
set "TTFNAME=Noto %Sub%"
set baseNm=%Prefix%Noto-%Sub%
call :convSizes

:: M+
set weight=light
::set weight=thin
::set weight=heavy
::set weight=medium
::set weight=black
set "TTFNAME=M+ 1c %weight%"
set baseNm=%Prefix%mplus1c-%weight%
call :convSizes

goto END


:convSizes
::
::call :conv1 24 384 144
::call :conv1 24 24  2304
::call :conv1 24 240 240
call :conv1 24 256 256

::call :conv1 20 320 120
::call :conv1 20 20  1920
::call :conv1 20 240 160
call :conv1 20 256 256 24 24

::call :conv1 16 256 96
::call :conv1 16 16 1536
call :conv1 16 256 128

call :conv1 14 256 128 16 16

::call :conv1 12 192 72
::call :conv1 12 12 1152
::call :conv1 12 120 120
call :conv1 12 128 128

::call :conv1 8 8  768
call :conv1 8 128 64

::call :conv1 6 6 576
::call :conv1 6 96 36

::call :conv1 6 8 768
call :conv1 6 128 64 8 8

exit /b 0

:init
pushd ..
set DEMO_ROOT=%CD%
::set Exe=%DEMO_ROOT%\bin\genfnttex.exe
set Exe=genfnttex.exe
popd
exit /b 0

:conv1
set FSZ=%1
set TW=%2
set TH=%3
set CW=%4
set CH=%5
if "%CW%"=="" set CW=%FSZ%
if "%CH%"=="" set CH=%FSZ%
set name=%baseNm%_%FSZ%_%TW%x%TH%
if not exist %name% mkdir %name%
pushd %name%
rem %Exe% -ttf="%TTFNAME%" %OPT% ../smp_text.txt -o%baseNm%_%TW%x%TH% -fs%FSZ% -cs%CW%:%CH% -ts%TW%:%TH%
%Exe% -ttf="%TTFNAME%" %OPT% -o%baseNm%_%TW%x%TH% -fs%FSZ% -cs%CW%:%CH% -ts%TW%:%TH%
bmptg :png -b4 *.tga
bmptg :bmp -b4 *.tga
popd
exit /b 0

:END
popd
endlocal
