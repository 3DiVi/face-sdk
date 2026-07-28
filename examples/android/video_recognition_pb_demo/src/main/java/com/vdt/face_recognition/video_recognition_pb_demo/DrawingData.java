package com.vdt.face_recognition.video_recognition_pb_demo;

import java.util.HashMap;
import android.graphics.Bitmap;


class DrawingData{

	public boolean updated;
	public Bitmap frame = null;
	public int frame_id;
	//Pair<track_id, face>
	public HashMap<Long, FaceData> faces = new HashMap<Long, FaceData>();

	public DrawingData(){
		this.updated = false;
	}
}