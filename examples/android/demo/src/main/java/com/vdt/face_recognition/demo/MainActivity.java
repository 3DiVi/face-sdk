package com.vdt.face_recognition.demo;

import java.io.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import android.graphics.Color;
import android.Manifest;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;
import android.content.pm.PackageManager;

import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.SDKException;

import com.vdt.face_recognition.demo.TheCamera;



public class MainActivity extends Activity
{
	private static final int REQUEST_SETTINGS = 1;
	private static final int REQUEST_OPTIONS = 2;

	private static final String TAG = "MainActivity face_sdk_demo";

	private TheCamera camera = null;
	private Demo demo = null;

	//Settings
	private int camera_id = 1;
	private int im_width = 640;
	private int im_height = 480;

	//Options
	private boolean [] flags;
	private int faceCutTypeId;

	String online_licence_dir = null;




	private String[] permissions_str = new String[] {
		Manifest.permission.CAMERA,
		Manifest.permission.READ_EXTERNAL_STORAGE,
		Manifest.permission.WRITE_EXTERNAL_STORAGE,
		Manifest.permission.READ_PHONE_STATE,
	};

	private int[] permissions_tv_id = new int[] {
		R.id.camera_perm_status,
		R.id.read_storage_perm_status,
		R.id.write_storage_perm_status,
		R.id.read_phone_perm_status,
	};


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// if directory with online licence exists
		// then use it
		// otherwise use default offline licence
		String buf = "/sdcard/face_recognition/online_license";
		if ((new File(buf)).exists()) {
			online_licence_dir = buf;
		}
		else {
			online_licence_dir = "";
		}


		// view persmissions status
		setContentView(R.layout.permissions);

		// check and request permissions
		int granted_count = 0;

		for(int i = 0; i < permissions_str.length; ++i)
		{
			String perstr = permissions_str[i];

			if(ContextCompat.checkSelfPermission(this, perstr) == PackageManager.PERMISSION_GRANTED)
			{
				TextView tv = (TextView) findViewById(permissions_tv_id[i]);
				tv.setText(" granted ");
				tv.setTextColor(Color.GREEN);
				++granted_count;
			}
		}

		if(granted_count < permissions_str.length)
		{
			ActivityCompat.requestPermissions(this, permissions_str, 0);
		}
		else
		{
			starting();
		}
	}

	private void starting()
	{
		setContentView(R.layout.splash);

		FacerecService service = FacerecService.createService(
			getApplicationInfo().nativeLibraryDir + "/libfacerec.so",
			"/sdcard/face_recognition/conf/facerec",
			online_licence_dir);

		FacerecService.LicenseState license_state = service.getLicenseState();
		Log.i(TAG, "license_state.online            = " + Boolean.toString(license_state.online));
		Log.i(TAG, "license_state.android_app_id    = " + license_state.android_app_id);
		Log.i(TAG, "license_state.ios_app_id        = " + license_state.ios_app_id);
		Log.i(TAG, "license_state.hardware_reg      = " + license_state.hardware_reg);


		new Thread(new LoadThread(this, service)).start();
	}

	@Override
	public void onRequestPermissionsResult(
		int requestCode,
		String[] permissions,
		int[] grantResults)
	{
		boolean ask_again = false;

		for(int i = 0; i < permissions.length; ++i)
		{
			if(grantResults[i] == PackageManager.PERMISSION_GRANTED)
			{
				TextView tv = (TextView) findViewById(permissions_tv_id[i]);
				tv.setText(" granted ");
				tv.setTextColor(Color.GREEN);
			}
			else if(!permissions[i].equals(Manifest.permission.READ_PHONE_STATE))  // READ_PHONE_STATE is optional
			{
				ask_again = true;
			}
		}

		if(ask_again)
		{
			final Activity this_activity = this;
			new Timer().schedule(new TimerTask(){@Override public void run() {
				ActivityCompat.requestPermissions(this_activity, permissions_str, 0);
			}}, 2000);
		}
		else
		{
			starting();
		}
	}


	@Override
	protected void onResume() {
		super.onResume();

		if (camera != null && demo != null){
			camera.open(demo, camera_id, im_width, im_height);
		}
	}


	@Override
	protected void onPause() {
		if(camera != null) camera.close();

		super.onPause();
	}


	@Override
	protected void onDestroy() {
		if(camera != null) camera.close();

		if(demo != null) demo.dispose();

		super.onDestroy();
	}


	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {

		if(resultCode == RESULT_OK){
			switch(requestCode){

				case REQUEST_SETTINGS:

			        camera_id = data.getIntExtra("cam_id", camera_id);
					String stemp = data.getStringExtra("selected_resolution");

					if (!stemp.equals(getStringResolution())){
						demo.updateCapturer();
						setNewResolution(stemp);
					}
					

					break;

				case REQUEST_OPTIONS:

					flags = data.getBooleanArrayExtra("flags");
					faceCutTypeId = data.getIntExtra("faceCutTypeId", 0);

					demo.setOptions(flags, faceCutTypeId);

					break;
			}
		}
	}


	private static class LoadThread implements Runnable{

		MainActivity ma;
		FacerecService service;

		public LoadThread(MainActivity ma, FacerecService service)
		{
			this.ma = ma;
			this.service = service;
		}

		public void run(){
			try{
				final TheCamera camera = new TheCamera(ma);

				final Demo demo = new Demo(ma, service);

				ma.flags = demo.getFlags();
				ma.faceCutTypeId = demo.getFaceCutTypeId();

				ma.runOnUiThread(new Runnable() {
					public void run() {
						ma.demo = demo;
						ma.camera = camera;
						ma.showForm();
					}
				});
			}catch(Exception e){
				exceptionHappensDo(ma, e);
				return;
			}
		}
	}


	public void showForm(){
		
		setContentView(R.layout.main);
		
		TextView textView = (TextView) findViewById(R.id.textView);
		textView.setMovementMethod(new ScrollingMovementMethod());

		demo.setTextView();

		Button quitButton = (Button) findViewById(R.id.quit_button);
		quitButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                finishAffinity();
            }
        });

        Button optionsButton = (Button) findViewById(R.id.options_button);
		optionsButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent toOptionsIntent = new Intent(getApplicationContext(), OptionsActivity.class);
        		toOptionsIntent.putExtra("flags", flags);
        		toOptionsIntent.putExtra("faceCutTypeId", faceCutTypeId);
				startActivityForResult(toOptionsIntent, REQUEST_OPTIONS);
            }
        });

        Button settingsButton = (Button) findViewById(R.id.settings_button);
		settingsButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Intent toSettingsIntent = new Intent(getApplicationContext(), SettingsActivity.class);

                toSettingsIntent.putExtra("selected_camera_id", camera_id);
                toSettingsIntent.putExtra("selected_resolution", getStringResolution());

				startActivityForResult(toSettingsIntent, REQUEST_SETTINGS);
            }
        });

        camera.open(demo, camera_id, im_width, im_height);
	
	}


    private void setNewResolution(String resol){

    	String [] tempStr = resol.split("x");
    	im_width = Integer.parseInt(tempStr[0]);
    	im_height = Integer.parseInt(tempStr[1]);

	}


	private String getStringResolution(){
		return Integer.toString(im_width)+"x"+Integer.toString(im_height);
	}


	private static void exceptionHappensDo(
		final Activity activity,
		final Exception e)
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
	}

}
