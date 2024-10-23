@echo off
setlocal enabledelayedexpansion

REM Assign the arguments to variables
set "RESULT_DIR=%~1"

echo RESULT_DIR: %RESULT_DIR%

set "CUR=%RESULT_DIR%\examples\flutter\demo\assets"
mkdir "%CUR%\share\facerec\recognizers" 2>nul
mkdir "%CUR%\share\facedetectors" 2>nul

xcopy /e /i /y "%RESULT_DIR%\conf" "%CUR%\conf"
xcopy /e /i /y "%RESULT_DIR%\license" "%CUR%\license"

xcopy /e /i /y "%RESULT_DIR%\share\face_quality" "%CUR%\share\face_quality"
xcopy /e /i /y "%RESULT_DIR%\share\faceanalysis" "%CUR%\share\faceanalysis"
xcopy /e /i /y "%RESULT_DIR%\share\faceattributes" "%CUR%\share\faceattributes"
xcopy /e /i /y "%RESULT_DIR%\share\fda" "%CUR%\share\fda"

xcopy /e /i /y "%RESULT_DIR%\share\facedetectors\blf" "%CUR%\share\facedetectors\blf"

xcopy /e /i /y "%RESULT_DIR%\share\facerec\recognizers\method12v30\*.enc" "%CUR%\share\facerec\recognizers\method12v30"
