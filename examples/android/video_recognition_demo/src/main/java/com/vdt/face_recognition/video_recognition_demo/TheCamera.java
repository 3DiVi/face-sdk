package com.vdt.face_recognition.video_recognition_demo;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.preference.PreferenceManager;
import android.util.Log;
import android.widget.ImageView;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.SDKException;

import com.vdt.face_recognition.video_recognition_demo.TheCameraPainter;


public class TheCamera implements Camera.PreviewCallback
{
	private static final String TAG = "TheCamera";
	private static final String SETTINGS_NAME = "SETTINGS";

	private Activity activity;

	private Camera camera = null;
	private TheCameraPainter painter = null;
	private int cam_id;
	private int width;
	private int height;
	private SurfaceTexture surfaceTexture = null;
	private int MAGIC_TEXTURE_ID = 10;

	private Bitmap bitmap = null;
	private byte[] buf = null;

	private boolean open_flag = false;


	public static class TheCameraInfo {
		public int id = 0; // id of camera
		public List<Size> resolutions = null;
	} 
	
	public static List<TheCamera.TheCameraInfo> getAvailableCameras() {
		List<TheCameraInfo> result = new ArrayList<TheCameraInfo>();
		int count = Camera.getNumberOfCameras();
		for (int i = 0; i < count; i++) {
			Camera cam = null;
			List<Size> resolutions = null; 
			try {
				cam = Camera.open(i);
				Parameters params = cam.getParameters();
				TheCameraInfo cam_info = new TheCameraInfo();
				cam_info.id = i;
				cam_info.resolutions = params.getSupportedPreviewSizes();
				result.add(cam_info);
			}
			catch (Exception e) {
				// No show camera radio button
				Log.e(TAG, "Can't open camera " + i);
				e.printStackTrace();
			}
			finally {
				if (cam != null) {
            		cam.release();
				}
			}
		}
		return result;
	}

	public TheCamera(Activity activity){
		this.activity = activity;
	}

	public synchronized void open(TheCameraPainter painter)
	{
		if (open_flag)
			return;

		//set painter
		this.painter = painter;
		
		//get settings
		SharedPreferences shared_settings = activity.getSharedPreferences(SETTINGS_NAME, 0);
		this.cam_id = shared_settings.getInt("camera id", 0);
		setResolution(shared_settings.getString("resolution", "640x480"));

		open_flag = true;

		if (camera != null)
			return;

		Log.i(TAG, "TheCamera open");

		try {
			camera = Camera.open(cam_id);
			Parameters cam_parameters = camera.getParameters();

			List<int[]> fps_ranges = cam_parameters.getSupportedPreviewFpsRange();
			int[] max_fps_range = fps_ranges.get(fps_ranges.size() - 1);
			Log.i(TAG, "Set FPS Range (" + max_fps_range[0] + "," + max_fps_range[1] + ")");

			cam_parameters.setPreviewFormat(ImageFormat.NV21);
			cam_parameters.setPreviewSize(width, height);
			cam_parameters.setPreviewFpsRange(max_fps_range[0], max_fps_range[1]);
			camera.setParameters(cam_parameters);


			cam_parameters = camera.getParameters();
			Size psize = cam_parameters.getPreviewSize();
			int size = psize.width * psize.height;
			size = size * ImageFormat.getBitsPerPixel(cam_parameters.getPreviewFormat()) / 8;
			buf = new byte[size];
		}
		catch (Exception e) {
			Log.e(TAG, "Can't open the camera " + cam_id);
			e.printStackTrace();
			close();
			return;
		}

		surfaceTexture = new SurfaceTexture(MAGIC_TEXTURE_ID);
		try{
			camera.setPreviewTexture(surfaceTexture);
		}catch(IOException e){
			e.printStackTrace();
		}
		camera.addCallbackBuffer(buf);
		camera.setPreviewCallbackWithBuffer(this);
		camera.setDisplayOrientation(0);

		camera.startPreview();

	}


	public synchronized void close()
	{
		if(!open_flag)
			return;

		open_flag = false;

		if (camera == null)
			return;

		Log.i(TAG, "TheCamera close");

		camera.setPreviewCallback(null);
		camera.stopPreview();
		camera.release();
		camera = null;
	}


	@Override
	public void onPreviewFrame(byte[] arg0, Camera arg1)
	{
		Camera.Parameters parameters = arg1.getParameters();
		Size size = parameters.getPreviewSize();

		try{
			painter.processingImage(arg0, size.width, size.height);
		}catch(Exception e){
			Intent toErrorIntent = new Intent(activity, ErrorActivity.class);
			toErrorIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			if (e instanceof SDKException){
				SDKException sdke = (SDKException) e;
				toErrorIntent.putExtra("error message", "code: " + String.format("0x%08X", sdke.code()) + "\n"+ sdke.getMessage());
			}else{
				toErrorIntent.putExtra("error message", e.getMessage());
			}
			e.printStackTrace();
			activity.startActivity(toErrorIntent);
			activity.finish();
			return;
		}
		
		camera.addCallbackBuffer(buf);
	}


	private void setResolution(String resolution){
    	String [] tempStr = resolution.split("x");
    	this.width = Integer.parseInt(tempStr[0]);
    	this.height = Integer.parseInt(tempStr[1]);
	}
}