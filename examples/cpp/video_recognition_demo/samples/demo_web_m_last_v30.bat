
dir base /A-D /B /S > ./base.list.txt

video_recognition_demo ^
  0 ^
  --frame_fps_limit 25 ^
  --vw_config_file video_worker_fdatracker.xml ^
  --fullscreen yes ^
  --license_dir ../license ^
  --config_dir ../conf/facerec/ ^
  --dll_path facerec.dll ^
  --database_list_filepath ./base.list.txt ^
  --method_config method9v30_recognizer.xml ^
  --recognition_distance_threshold 6800.0
