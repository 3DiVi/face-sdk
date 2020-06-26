#!/bin/sh

find ./base -type f > ./base.list.txt

./video_recognition_demo \
  0 \
  --frame_fps_limit 25 \
  --vw_config_file video_worker_fdatracker.xml \
  --fullscreen yes \
  --license_dir ../license \
  --config_dir ../conf/facerec/ \
  --dll_path ../lib/libfacerec.so \
  --database_list_filepath ./base.list.txt \
  --method_config method9v1000_recognizer.xml \
  --recognition_distance_threshold 7200.0
