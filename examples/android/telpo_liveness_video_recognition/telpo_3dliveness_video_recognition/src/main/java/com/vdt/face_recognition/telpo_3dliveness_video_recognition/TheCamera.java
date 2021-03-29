package com.vdt.face_recognition.telpo_3dliveness_video_recognition;

import java.io.*;
import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.ImageFormat;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.hardware.Camera;
import android.util.Log;

import com.vdt.face_recognition.sdk.SDKException;

import com.vdt.face_recognition.telpo_3dliveness_video_recognition.AuthActivity;


public class TheCamera implements Camera.PreviewCallback, SurfaceHolder.Callback
{
	public static interface FramesProcessor{
		public void processRGBFrame(byte[] data, int width, int height);
		public void processIRFrame(byte[] data, int width, int height) throws Exception;
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

	private int frame_buffer_size = 0;

	private boolean open_flag = false;

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

	public synchronized void open(FramesProcessor frames_processor, SurfaceView surf_view, View surf_view_ll) throws IOException
	{
		if (open_flag)
			return;

		this.frames_processor = frames_processor;

		//get settings
		SharedPreferences shared_settings = activity.getSharedPreferences(SETTINGS_NAME, 0);
		this.base_angle = shared_settings.getInt("base_angle", 0);
		setResolution(shared_settings.getString("resolution", "640x480"));


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
			frame_buffer_size = psize.width * psize.height * ImageFormat.getBitsPerPixel(cam_parameters.getPreviewFormat()) / 8;
		}
		catch (Exception e) {
			Log.e(TAG, "Can't open the camera " + cam_id);
			e.printStackTrace();
			close();
			return;
		}

		this.surf_view = surf_view;
		this.surf_view_ll = surf_view_ll;

		if (surf_view != null)
			surf_view.getHolder().addCallback(this);

		camera.addCallbackBuffer(new byte[frame_buffer_size]);
		camera.setPreviewCallbackWithBuffer(this);
		camera.setDisplayOrientation(0);

		camera.startPreview();

		open_flag = true;
	}

	public void surfaceChanged(SurfaceHolder holder, int format, int w, int h)
	{
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
	}

	public void surfaceCreated(SurfaceHolder holder)
	{
		try
		{
			if(camera != null && open_flag)
				camera.setPreviewDisplay(holder);
		}
		catch(Exception e)
		{
			AuthActivity.exceptionHappensDo(activity, e);
		}
	}

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
			if (cam_id == 0)
				frames_processor.processRGBFrame(arg0, size.width, size.height);
			else
				if (cam_id == 1)
					frames_processor.processIRFrame(arg0, size.width, size.height);
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

		camera.addCallbackBuffer(new byte[frame_buffer_size]);
	}


	private void setResolution(String resolution){
		String [] tempStr = resolution.split("x");
		this.width = Integer.parseInt(tempStr[0]);
		this.height = Integer.parseInt(tempStr[1]);
	}
}