package com.vdt.face_recognition.video_recognition_pb_demo;

import android.graphics.Bitmap;

class FaceData {

    public long track_id;
    public float[] bbox;
    public float[] landmarks;
    public boolean lost;
    public boolean weak;
    public int frame_id;
    public long lost_time;
    public long match_db_index;
    public String match_uuid;
    public Bitmap faceCrop;

    public FaceData(long track_id) {
        this.track_id = track_id;
        this.match_db_index = -1;
        this.lost = false;
        this.weak = false;
    }
}
