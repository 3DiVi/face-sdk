@echo off
setlocal enabledelayedexpansion

REM Assign the arguments to variables
set "RESULT_DIR=%~1"

echo RESULT_DIR: %RESULT_DIR%

set "ASSETS=%RESULT_DIR%\examples\flutter\isolates_demo\assets"
mkdir "%ASSETS%\share\faceanalysis"
mkdir "%ASSETS%\share\faceattributes"
mkdir "%ASSETS%\share\facedetectors\blf"
mkdir "%ASSETS%\share\face_quality"
mkdir "%ASSETS%\share\fda"
mkdir "%ASSETS%\share\processing_block\face_detector\blf_front"
mkdir "%ASSETS%\share\processing_block\face_template_extractor\30\1"
mkdir "%ASSETS%\share\processing_block\liveness_estimator\2d_light"
mkdir "%ASSETS%\share\quality_iso"

xcopy /e /i /y "%RESULT_DIR%\conf" "%ASSETS%\conf"
xcopy /e /i /y "%RESULT_DIR%\license" "%ASSETS%\license"

xcopy /e /i /y "%RESULT_DIR%\share\processing_block\face_detector\blf_front" "%ASSETS%\share\processing_block\face_detector\blf_front"
xcopy /e /i /y "%RESULT_DIR%\share\processing_block\face_template_extractor\30\1" "%ASSETS%\share\processing_block\face_template_extractor\30\1"
xcopy /e /i /y "%RESULT_DIR%\share\processing_block\liveness_estimator\2d_light\1.enc" "%ASSETS%\share\processing_block\liveness_estimator\2d_light"

xcopy /e /i /y "%RESULT_DIR%\share\facedetectors\blf\front_model.enc" "%ASSETS%\share\facedetectors\blf"
xcopy /e /i /y "%RESULT_DIR%\share\faceanalysis" "%ASSETS%\share\faceanalysis"
xcopy /e /i /y "%RESULT_DIR%\share\faceattributes" "%ASSETS%\share\faceattributes"
xcopy /e /i /y "%RESULT_DIR%\share\face_quality" "%ASSETS%\share\face_quality"
xcopy /e /i /y "%RESULT_DIR%\share\fda" "%ASSETS%\share\fda"
xcopy /e /i /y "%RESULT_DIR%\share\quality_iso" "%ASSETS%\share\quality_iso"
