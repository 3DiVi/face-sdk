package com.vdt.face_recognition.video_recognition_demo;

import java.util.HashMap;
import android.graphics.Bitmap;


class DrawingData{

	public boolean updated;
	public Bitmap frame = null;
	public int frame_id;
	//Pair<track_id, face>
	public HashMap<Integer, FaceData> faces = new HashMap<Integer, FaceData>();

	public DrawingData(){
		this.updated = false;
	}
}