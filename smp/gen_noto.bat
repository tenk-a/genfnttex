set "OPT=%OPT% -cluttype=1 -resizemode=2 -addascii"

set Sub=Sans
set Sub=Serif
set "TTFNAME=Noto %Sub%"
set baseNm=Noto-%Sub%

::
call :conv1 24 24  2304
call :conv1 24 256 256

call :conv1 20 20  1920
call :conv1 20 256 256

call :conv1 16 256 128
call :conv1 16 16 1536

call :conv1 12 12 1152
call :conv1 12 128 128

call :conv1 8 128 64
call :conv1 8 8  768

call :conv1 6 96 36
call :conv1 6 6 576

call :conv1 6 128 64
call :conv1 6 8 768

goto END

:conv1
set FSZ=%1
set TW=%2
set TH=%3
set name=%baseNm%_%FSZ%_%TW%x%TH%
if not exist %name% mkdir %name%
pushd %name%
genfnttex -ttf="%TTFNAME%" %OPT% ../smp_text.txt -fs%FSZ% -o%baseNm%_%TW%x%TH%  -ts%TW%:%TH%
bmptg :png -b4 *.tga
popd
exit /b 0

:END
