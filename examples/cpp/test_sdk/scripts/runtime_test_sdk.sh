#!/bin/bash

# PARAMETERS
DLL_PATH=../lib/libfacerec.so
SDK_CONFIG_DIR=../conf/facerec
DATASET_ROOT_DIR=/path/to/data
DATASET_CONFIG_LFW_SIMPLE_FORMAT=lfw_simple_format.txt
DATASET_CONFIG=dataset_config.txt

CAPTURER_CONFIG=common_capturer4_lbf.xml
DETECTION_RESULT_FILE=detection_result.txt
PROCESSING_RESULT_FILE_PREFIX=processing_result_

RECOGNIZER_METHODS_LIST="6v6 7v6"
RANDOM_TEMPLATES_COUNT=100


# RUN TEST

# convert format
./test_sdk \
	--mode convert_config_format \
	--result_dataset_config $DATASET_CONFIG \
	$DATASET_CONFIG_LFW_SIMPLE_FORMAT

# detection
./test_sdk \
	--mode                    detection \
	--dll_path                $DLL_PATH \
	--sdk_config_dir          $SDK_CONFIG_DIR \
	--capturer_config         $CAPTURER_CONFIG \
	--dataset_config          $DATASET_CONFIG \
	--dataset_root_dir        $DATASET_ROOT_DIR \
	--detection_result_file   $DETECTION_RESULT_FILE \
	--use_cpu_cores_count     4 \
	--runtime_log_file        detection_runtime_log.txt \

for REC_METHOD in $RECOGNIZER_METHODS_LIST;
do

	RECOGNIZER_CONFIG=method${REC_METHOD}_recognizer.xml
	RANDOM_TEMPLATES_FILE=random_templates_${REC_METHOD}_${RANDOM_TEMPLATES_COUNT}.bin
	PROCESSING_RESULT_FILE=${PROCESSING_RESULT_FILE_PREFIX}_${REC_METHOD}.bin
	
	# processing
	./test_sdk \
		--mode                             processing \
		--dll_path                         $DLL_PATH \
		--sdk_config_dir                   $SDK_CONFIG_DIR \
		--recognizer_config                $RECOGNIZER_CONFIG \
		--dataset_config                   $DATASET_CONFIG \
		--dataset_root_dir                 $DATASET_ROOT_DIR \
		--processing_result_file           $PROCESSING_RESULT_FILE \
		--use_cpu_cores_count              4 \
		--runtime_log_file                 processing_runtime_log_${REC_METHOD}.txt \
		$DETECTION_RESULT_FILE

	# recognition_test_11
	./test_sdk \
		--mode                             recognition_test_11 \
		--dll_path                         $DLL_PATH \
		--sdk_config_dir                   $SDK_CONFIG_DIR \
		--recognizer_config                $RECOGNIZER_CONFIG \
		--dataset_config                   $DATASET_CONFIG \
		--result_closest_mismatches_file   closest_mismatches_result_${REC_METHOD}.txt \
		--result_roc_file                  roc_${REC_METHOD}.txt \
		--use_cpu_cores_count              4 \
		--runtime_log_file                 recognition_11_test_runtime_log_${REC_METHOD}.txt \
		$PROCESSING_RESULT_FILE

	# generate random templates
	echo "Generating random templates..."
	./template_generator \
		$REC_METHOD \
		$RANDOM_TEMPLATES_COUNT \
		$RANDOM_TEMPLATES_FILE
		
	# search_speed_test
	./test_sdk \
		--mode                             search_speed_test \
		--dll_path                         $DLL_PATH \
		--sdk_config_dir                   $SDK_CONFIG_DIR \
		--recognizer_config                $RECOGNIZER_CONFIG \
		$RANDOM_TEMPLATES_FILE

done