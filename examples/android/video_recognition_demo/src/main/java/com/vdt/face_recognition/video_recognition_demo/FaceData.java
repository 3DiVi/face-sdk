package com.vdt.face_recognition.video_recognition_demo;

import com.vdt.face_recognition.sdk.RawSample;


class FaceData{

	public RawSample sample;
	public boolean lost;
	public boolean weak;
	public int frame_id;
	public long lost_time;
	public long match_db_index;


	public FaceData(RawSample sample){
		this.match_db_index = -1;
		this.lost 			= false;
		this.sample 		= sample;
	}
}