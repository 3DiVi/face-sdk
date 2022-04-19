
dir base /A-D /B /S > ./base.list.txt

video_recognition_demo ^
  0 ^
  --frame_fps_limit 25 ^
  --vw_config_file video_worker_fdatracker_blf_fda.xml ^
  --fullscreen yes ^
  --license_dir ..\license ^
  --config_dir ..\conf\facerec\ ^
  --dll_path .\facerec.dll ^
  --database_list_filepath .\base.list.txt ^
  --method_config recognizer_latest_v300.xml ^
  --recognition_far_threshold 1e-6
