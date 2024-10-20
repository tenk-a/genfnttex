setlocal
pushd %~dp0

call :init
set "OPT=%OPT% -cluttype=1:0x80 -resizemode=2 -addascii -bpp4"

set weight=light
::set weight=thin
::set weight=heavy
::set weight=medium
::set weight=black
rem 16*4=96

set "TTFNAME=M+ 1c %weight%"
set baseNm=mplus1c-%weight%

::
::call :conv1 24 384 144
::call :conv1 24 24  2304
::call :conv1 24 240 240
call :conv1 24 256 256

::call :conv1 20 320 120
::call :conv1 20 20  1920
::call :conv1 20 240 160
call :conv1 20 256 256

::call :conv1 16 256 96
::call :conv1 16 16 1536
call :conv1 16 256 128

::call :conv1 12 192 72
::call :conv1 12 12 1152
::call :conv1 12 120 120
call :conv1 12 128 128

::call :conv1 8 8  768
call :conv1 8 128 64

::call :conv1 6 6 576
call :conv1 6 96 36

::call :conv1 6 8 768
call :conv1 6 128 64

goto END

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
set name=%baseNm%_%FSZ%_%TW%x%TH%
if not exist %name% mkdir %name%
pushd %name%
%Exe% -ttf="%TTFNAME%" %OPT% ../smp_text.txt -fs%FSZ% -o%baseNm%_%TW%x%TH%  -ts%TW%:%TH%
bmptg :png -b4 *.tga
bmptg :bmp -b4 *.tga
popd
exit /b 0

:END
popd
endlocal
