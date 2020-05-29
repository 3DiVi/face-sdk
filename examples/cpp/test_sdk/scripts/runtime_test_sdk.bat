

REM PARAMETERS
SET DLL_PATH=facerec.dll
SET SDK_CONFIG_DIR=../conf/facerec
SET DATASET_ROOT_DIR=/path/to/data
SET DATASET_CONFIG_LFW_SIMPLE_FORMAT=lfw_simple_format.txt
SET DATASET_CONFIG=dataset_config.txt

SET CAPTURER_CONFIG=common_capturer4_lbf.xml
SET DETECTION_RESULT_FILE=detection_result.txt
SET PROCESSING_RESULT_FILE_PREFIX=processing_result_

SET RECOGNIZER_METHOD_LIST=6v6 7v6
SET RANDOM_TEMPLATES_COUNT=100


REM RUN TEST

REM convert format
test_sdk.exe ^
	--mode convert_config_format ^
	--result_dataset_config %DATASET_CONFIG% ^
	%DATASET_CONFIG_LFW_SIMPLE_FORMAT%

REM detection
test_sdk.exe ^
	--mode                    detection ^
	--dll_path                %DLL_PATH% ^
	--sdk_config_dir          %SDK_CONFIG_DIR% ^
	--capturer_config         %CAPTURER_CONFIG% ^
	--dataset_config          %DATASET_CONFIG% ^
	--dataset_root_dir        %DATASET_ROOT_DIR% ^
	--detection_result_file   %DETECTION_RESULT_FILE% ^
	--use_cpu_cores_count     4 ^
	--runtime_log_file        detection_runtime_log.txt ^

for %%R in (%RECOGNIZER_METHOD_LIST%) do (

	SET RECOGNIZER_CONFIG=method%%R_recognizer.xml
	SET RANDOM_TEMPLATES_FILE=random_templates_%%R_%RANDOM_TEMPLATES_COUNT%.bin
	SET PROCESSING_RESULT_FILE=%PROCESSING_RESULT_FILE_PREFIX%_%%R.bin
	
	REM processing
	test_sdk.exe ^
		--mode                             processing ^
		--dll_path                         %DLL_PATH% ^
		--sdk_config_dir                   %SDK_CONFIG_DIR% ^
		--recognizer_config                %RECOGNIZER_CONFIG% ^
		--dataset_config                   %DATASET_CONFIG% ^
		--dataset_root_dir                 %DATASET_ROOT_DIR% ^
		--processing_result_file           %PROCESSING_RESULT_FILE% ^
		--use_cpu_cores_count              4 ^
		--runtime_log_file                 processing_runtime_log_%%R.txt ^
		%DETECTION_RESULT_FILE%

	REM recognition_test_11
	test_sdk.exe ^
		--mode                             recognition_test_11 ^
		--dll_path                         %DLL_PATH% ^
		--sdk_config_dir                   %SDK_CONFIG_DIR% ^
		--recognizer_config                %RECOGNIZER_CONFIG% ^
		--dataset_config                   %DATASET_CONFIG% ^
		--result_closest_mismatches_file   closest_mismatches_result_%%R.txt ^
		--result_roc_file                  roc_%%R.txt ^
		--use_cpu_cores_count              4 ^
		--runtime_log_file                 recognition_11_test_runtime_log_%%R.txt ^
		%PROCESSING_RESULT_FILE%

	REM generate random templates
	echo;
	echo "Generating random templates..."
	template_generator.exe ^
		%%R ^
		%RANDOM_TEMPLATES_COUNT% ^
		%RANDOM_TEMPLATES_FILE%
		
	REM search_speed_test
	test_sdk.exe ^
		--mode                             search_speed_test ^
		--dll_path                         %DLL_PATH% ^
		--sdk_config_dir                   %SDK_CONFIG_DIR% ^
		--recognizer_config                %RECOGNIZER_CONFIG% ^
		%RANDOM_TEMPLATES_FILE%

)