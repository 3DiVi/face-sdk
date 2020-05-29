package com.vdt.face_recognition.video_recognition_demo;

import java.io.File;

import java.util.Timer;
import java.util.TimerTask;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import android.widget.TextView;
import android.graphics.Color;
import android.Manifest;
import androidx.core.content.ContextCompat;
import androidx.core.app.ActivityCompat;
import android.content.pm.PackageManager;

import com.vdt.face_recognition.sdk.SDKException;

import com.vdt.face_recognition.video_recognition_demo.VidRecDemo;
import com.vdt.face_recognition.video_recognition_demo.TheCamera;

//all init parameters in init_settings
public class MainActivity extends Activity
{
	private static final String TAG = "MainActivity";
	private static final String SETTINGS_NAME = "SETTINGS";

	SharedPreferences shared_settings = null;
	Handler handler = null;

	private VidRecDemo vr_demo = null;
	private TheCamera camera = null;

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

		init_settings();

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
		new Thread(new LoadThread(this)).start();
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

		if(vr_demo == null){
			//creating activity, nothing update
			return;
		}else{

			//check update exist
			SharedPreferences shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
			if (shared_settings.getBoolean("recognizer_updated", true) ||
				shared_settings.getBoolean("capturer_updated", true))
			{
				setContentView(R.layout.splash);

				// force free resources
				// otherwise licence error may occur
				// when create sdk object in next time
				vr_demo.closeDrawThread();
				vr_demo.dispose();
				vr_demo = null;

				new Thread(new UpdateThread(this)).start();
			}else{
				camera.open(vr_demo);
			}

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

		if(vr_demo != null){
			vr_demo.closeDrawThread();
			vr_demo.dispose();
		}

		super.onDestroy();
	}


	private void init_settings(){

		shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
		SharedPreferences.Editor editor = shared_settings.edit();

		// init settings with default values, if keys is not represented yet
		editor.putInt 		("camera id", shared_settings.getInt("camera", 1));
		editor.putString 	("resolution", shared_settings.getString("resolution", "640x480"));
		editor.putString	("rec_method0", shared_settings.getString("rec_method0", "method6v7_recognizer.xml"));
		editor.putString	("rec_method1", shared_settings.getString("rec_method1", "method7v7_recognizer.xml"));
		editor.putString	("rec_method2", shared_settings.getString("rec_method2", "method8v7_recognizer.xml"));
		editor.putString	("threshold0", shared_settings.getString("threshold0", "7000"));
		editor.putString	("threshold1", shared_settings.getString("threshold1", "8000"));
		editor.putString	("threshold2", shared_settings.getString("threshold2", "8800"));
		editor.putInt 		("method index", shared_settings.getInt("method index", 2));		

		// init runtime variables
		editor.putBoolean	("recognizer_updated", false);
		editor.putBoolean	("capturer_updated", false);

		editor.commit();

	}


	private static class LoadThread implements Runnable{

		MainActivity ma;

		public LoadThread(MainActivity ma){
			this.ma = ma;
		}

		public void run(){
			try{
				final TheCamera camera = new TheCamera(ma);

				// pass absolute path for libfacerec.so
				final VidRecDemo vr_demo = new VidRecDemo(
					ma,
					ma.getApplicationInfo().nativeLibraryDir + "/libfacerec.so",
					"/sdcard/face_recognition/conf/facerec/",
					ma.online_licence_dir,
					"/sdcard/face_recognition/database/");

				ma.runOnUiThread(new Runnable(){
					public void run(){
						ma.camera = camera;
						ma.vr_demo = vr_demo;
						ma.showForm();
					}
				});
			}catch(Exception e){
				exceptionHappensDo(ma, e);
				return;
			}
		}
	}


	private static class UpdateThread implements Runnable{

		MainActivity ma;

		public UpdateThread(MainActivity ma){
			this.ma = ma;
		}

		public void run(){

			try{

				SharedPreferences.Editor editor = ma.shared_settings.edit();

				//can throw SDKException
				// pass absolute path for libfacerec.so
				final VidRecDemo vr_demo = new VidRecDemo(
					ma,
					ma.getApplicationInfo().nativeLibraryDir + "/libfacerec.so",
					"/sdcard/face_recognition/conf/facerec/",
					ma.online_licence_dir,
					"/sdcard/face_recognition/database/");
				editor.putBoolean("recognizer_updated", false);
				editor.putBoolean("capturer_updated", false);

				editor.commit();

				ma.runOnUiThread(new Runnable(){
					public void run(){
						ma.vr_demo = vr_demo;
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

		camera.open(vr_demo);

		Button new_personButton = (Button) findViewById(R.id.new_person_button);
		new_personButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				Intent newPersonIntent = new Intent(getApplicationContext(), NewPersonActivity.class);
				startActivity(newPersonIntent);
			}
		});

		Button settingsButton = (Button) findViewById(R.id.settings_button);
		settingsButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				Intent settingsIntent = new Intent(getApplicationContext(), SettingsActivity.class);
				startActivity(settingsIntent);
			}
		});

		Button quitButton = (Button) findViewById(R.id.quit_button);
		quitButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {
				finishAffinity();
			}
		});

		handler = new Handler(){
			public void handleMessage(Message msg){
				try{
					vr_demo.updateImageViews();
				}catch(Exception e){
					exceptionHappensDo(MainActivity.this, e);
				}
			}
		};

		vr_demo.startDrawThread(handler);
		
	}


	private static void exceptionHappensDo(Activity activity, Exception e){
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
