@echo off
setlocal enabledelayedexpansion

REM Assign the arguments to variables
set "RESULT_DIR=%~1"

echo RESULT_DIR: %RESULT_DIR%

set "ASSETS=%RESULT_DIR%\examples\flutter\processing_block_demo\assets"
mkdir "%ASSETS%\share\faceanalysis";
mkdir "%ASSETS%\share\processing_block\face_detector\uld";
mkdir "%ASSETS%\share\processing_block\face_fitter\tddfa_faster";
mkdir "%ASSETS%\share\processing_block\quality_face_estimation\estimation";
mkdir "%ASSETS%\share\faceattributes";
mkdir "%ASSETS%\share\quality_iso";

xcopy /e /i /y "%RESULT_DIR%\conf" "%ASSETS%";
xcopy /e /i /y "%RESULT_DIR%\license" "%ASSETS%";

xcopy /e /i /y "%RESULT_DIR%\share\processing_block\face_detector\uld" "%ASSETS%\share\processing_block\face_detector\uld";
xcopy /e /i /y "%RESULT_DIR%\share\processing_block\face_fitter\tddfa_faster" "%ASSETS%\share\processing_block\face_fitter\tddfa_faster";
xcopy /e /i /y "%RESULT_DIR%\share\processing_block\quality_face_estimation\estimation" "%ASSETS%\share\processing_block\quality_face_estimation\estimation";

xcopy /e /i /y "%RESULT_DIR%\share\faceanalysis" "%ASSETS%\share\faceanalysis";
xcopy /e /i /y "%RESULT_DIR%\share\faceattributes" "%ASSETS%\share\faceattributes";
xcopy /e /i /y "%RESULT_DIR%\share\quality_iso" "%ASSETS%\share\quality_iso";
