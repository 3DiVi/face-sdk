package com.vdt.face_recognition.telpo_liveness_video_recognition;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;

import com.vdt.face_recognition.sdk.SDKException;

import com.vdt.face_recognition.telpo_liveness_video_recognition.AuthActivity;


public class TheCamera implements Camera.PreviewCallback, SurfaceHolder.Callback
{
	public static interface FramesProcessor{
		void processingImage(byte[] data, int width, int height, int cam_id) throws Exception;
	}

	private static final String TAG = "TheCamera";
	private static final String SETTINGS_NAME = "SETTINGS";

	private Activity activity;

	private Camera camera = null;
	private FramesProcessor frames_processor = null;
	private int cam_id;
	private int width;
	private int height;

	private int base_angle;
	private byte[] buf = null;

	private boolean open_flag = false;

	private SurfaceTexture surfaceTexture = null;
	private int MAGIC_TEXTURE_ID = 10;
	SurfaceView surf_view;
	View surf_view_ll;

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

	public TheCamera(Activity activity, int cam_id){
		this.cam_id = cam_id;
		this.activity = activity;
	}

	public int getWidth()
	{
		return width;
	}

	public int getHeight()
	{
		return height;
	}

	public int getCamID()
	{
		return cam_id;
	}

	public void setCameraDisplayOrientation(){
		android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
		android.hardware.Camera.getCameraInfo(cam_id, info);

		int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
		int degrees = 0;
		switch (rotation) {
			case Surface.ROTATION_0:
				degrees = 0;
				break;
			case Surface.ROTATION_90:
				degrees = 90;
				break;
			case Surface.ROTATION_180:
				degrees = 180;
				break;
			case Surface.ROTATION_270:
				degrees = 270;
				break;
		}

		int result;
		if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
				result = (info.orientation + degrees) % 360;
				result = (360 - result) % 360;  // compensate the mirror
		} else {  // back-facing
				result = (info.orientation - degrees + 360) % 360;
		}
		camera.setDisplayOrientation(result);
	}


	public synchronized void open(FramesProcessor frames_processor, SurfaceView surf_view, View surf_view_ll) throws IOException
	{
		if (open_flag)
			return;

		this.frames_processor = frames_processor;

		//get settings
		SharedPreferences shared_settings = activity.getSharedPreferences(SETTINGS_NAME, 0);
		this.base_angle = shared_settings.getInt("base_angle", 0);
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

		this.surf_view = surf_view;
		this.surf_view_ll = surf_view_ll;

		if (surf_view == null) {
			surfaceTexture = new SurfaceTexture(MAGIC_TEXTURE_ID);
			try{
				camera.setPreviewTexture(surfaceTexture);
			}catch(IOException e){
				e.printStackTrace();
			}
		}else {
			surf_view.getHolder().addCallback(this);
		}

		camera.addCallbackBuffer(buf);
		camera.setPreviewCallbackWithBuffer(this);

		setCameraDisplayOrientation();
		camera.startPreview();
	}


	@Override
	public void surfaceCreated(SurfaceHolder holder) {

	}


	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
	{
		try
		{
			if(camera != null && open_flag)
			{
				camera.stopPreview();
				if (surf_view != null) {
					int sv_width = surf_view_ll.getWidth();
					int sv_height = surf_view_ll.getHeight();
					float scale;

					android.view.ViewGroup.LayoutParams lp = surf_view.getLayoutParams();

					//different scale calculation for different camera orientation
					if(  (base_angle %180) == 0 ) {
						scale = Math.min(sv_width / (float) width, sv_height / (float) height);
					} else {
						scale = Math.min(sv_width / (float) height, sv_height / (float) width);
					}
					lp.width = (int) (width * scale);
					lp.height = (int) (height * scale);

					surf_view.setLayoutParams(lp);
				}
				camera.setPreviewDisplay(holder);
				camera.setPreviewCallback(this);
				camera.startPreview();
			}
		}
		catch(Exception e)
		{
			AuthActivity.exceptionHappensDo(activity, e);
		}
	}


	@Override
	public void surfaceDestroyed(SurfaceHolder holder)
	{
		close();
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

		try
		{
			frames_processor.processingImage(arg0, size.width, size.height, cam_id);
		}
		catch(Exception e)
		{
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