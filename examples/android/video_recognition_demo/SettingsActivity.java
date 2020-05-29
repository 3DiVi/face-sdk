package com.vdt.face_recognition.video_recognition_demo;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;

import com.vdt.face_recognition.video_recognition_demo.TheCamera;


public class SettingsActivity extends Activity{

	private static final String TAG = "Settings Activity";
	private static final String SETTINGS_NAME = "SETTINGS";

	private ArrayList<RadioButton> rb_list = new ArrayList<RadioButton>();
	private RadioGroup radioGroup = null;
	private RadioButton rb0 = null;
	private RadioButton rb1 = null;
	private RadioButton rb2 = null;

	private Spinner resolution_spinner = null;
	private Spinner recognizer_spinner = null;

	private EditText threshold_editText = null;

	private ArrayList<ArrayList<String>> cams_resolutions = new ArrayList<ArrayList<String>>();

	private SharedPreferences shared_settings = null;
	private int camera_id;
	private String resolution;
	private String [] recognizer_methods = new String [3];
	private String [] thresholds = new String [3];
	private int recognizer_method_index;


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.settings);

		//radio buttons init
		radioGroup = (RadioGroup) findViewById(R.id.cameras_radio_group);
		rb0 = (RadioButton) findViewById(R.id.camera0_radio_button);
		rb1 = (RadioButton) findViewById(R.id.camera1_radio_button);
		rb2 = (RadioButton) findViewById(R.id.camera2_radio_button);

		rb_list.add(rb0);
		rb_list.add(rb1);
		rb_list.add(rb2);

		//get settings
		shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
		camera_id 				= shared_settings.getInt	("camera id", 0);
		resolution				= shared_settings.getString	("resolution", "640x480");
		recognizer_method_index = shared_settings.getInt	("method index", 0);
		recognizer_methods[0] 	= shared_settings.getString	("rec_method0", null);
		recognizer_methods[1] 	= shared_settings.getString	("rec_method1", null);
		recognizer_methods[2]   = shared_settings.getString	("rec_method2", null);
		thresholds[0] 			= shared_settings.getString	("threshold0", null);
		thresholds[1] 			= shared_settings.getString	("threshold1", null);
		thresholds[2] 			= shared_settings.getString	("threshold2", null);

		List<TheCamera.TheCameraInfo> availableCameras = TheCamera.getAvailableCameras();
		if (availableCameras.size() == 0) {
			Log.e(TAG, "No available cameras");
			Intent toErrorIntent = new Intent(this, ErrorActivity.class);
			toErrorIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
			toErrorIntent.putExtra("error message", "No available cameras");
			startActivity(toErrorIntent);
			finish();
			return;
		}

				
		for(int i = 0; i < rb_list.size(); i++) {
			cams_resolutions.add(new ArrayList<String>());
			rb_list.get(i).setVisibility(View.GONE);
		}

		boolean is_selected_available = false;
		for(int i = 0; i < availableCameras.size(); i++) {
			int cam_id = availableCameras.get(i).id;
			List<Size> resolutions = availableCameras.get(i).resolutions;
			Log.d(TAG, "Camera " + cam_id + " available. Resolutions:");
			if (cam_id < rb_list.size()) {
				rb_list.get(cam_id).setVisibility(View.VISIBLE);
				for (Size size: resolutions) {
					String resolution_string = Integer.toString(size.width) + "x" + Integer.toString(size.height);
					cams_resolutions.get(cam_id).add(resolution_string);
					Log.d(TAG, "    " + resolution_string);
				}
			}
			if (cam_id == camera_id) {
				is_selected_available = true;
			}
		}

		if (!is_selected_available) {
			camera_id = availableCameras.get(0).id;
		}

		//camera radio button
		RadioButton rb = (RadioButton) radioGroup.getChildAt(camera_id);
		rb.setChecked(true);

		//resolution spinner
        resolution_spinner = (Spinner) findViewById(R.id.resolution_spinner);
        setSpinnerResolutions(resolution);

        //recognizer spinner
        recognizer_spinner = (Spinner) findViewById(R.id.recognizer_spinner);
        setSpinnerRecognizer();
        recognizer_spinner.setOnItemSelectedListener(new OnItemSelectedListener() {

            @Override
            public void onItemSelected(
            	AdapterView<?> adapterView,
            	View view,
                int position,
                long id)
            {
                threshold_editText.setText(thresholds[position]);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {}
        });

        //recognizer threshold
		threshold_editText = (EditText) findViewById(R.id.threshold_editText);
		threshold_editText.setText(thresholds[recognizer_method_index]);

		//ok - button
		Button okButton = (Button) findViewById(R.id.settings_ok_button);
		okButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

            	int new_method_index = recognizer_spinner.getSelectedItemPosition();

            	shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
				SharedPreferences.Editor editor = shared_settings.edit();

				editor.putInt 		("camera id", camera_id);
				editor.putString 	("resolution", resolution_spinner.getSelectedItem().toString());
				editor.putInt 		("method index", new_method_index);
				switch(new_method_index){
					case 0:
						editor.putString("threshold0", threshold_editText.getText().toString());
						break;
					case 1:
						editor.putString("threshold1", threshold_editText.getText().toString());
						break;
					case 2:
						editor.putString("threshold2", threshold_editText.getText().toString());
						break;
				}

				//check updates
				if(recognizer_method_index != new_method_index){
					//new recognizier
					editor.putBoolean("recognizer_updated", true);
				}else{
					if(!thresholds[recognizer_method_index].equals(threshold_editText.getText().toString())) {
						//new threshold
						editor.putBoolean("recognizer_updated", true);
					}
				}

				if(!resolution.equals(resolution_spinner.getSelectedItem().toString())) {
					//new capturer
					editor.putBoolean("capturer_updated", true);
				}

				editor.commit();

            	finish();
            }
        });

	}


	public void onRadioClicked(View view) {

		switch (view.getId()) {

			case R.id.camera0_radio_button:
				camera_id = 0;
				setSpinnerResolutions("640x480");
				break;

			case R.id.camera1_radio_button:
				camera_id = 1;
				setSpinnerResolutions("640x480");
				break;

			case R.id.camera2_radio_button:
				camera_id = 2;
				setSpinnerResolutions("640x480");
				break;
		}
	}


	private void setSpinnerResolutions(String resolution){
		
		ArrayAdapter<String> resolution_spinnerAdapter = new ArrayAdapter<String>(
			this,
			android.R.layout.simple_spinner_item,
			cams_resolutions.get(camera_id).toArray(new String[0]));
        resolution_spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        resolution_spinner.setAdapter(resolution_spinnerAdapter);

        int spinner_index = cams_resolutions.get(camera_id).indexOf(resolution);
        spinner_index = (spinner_index == -1) ? 0 : spinner_index;
        resolution_spinner.setSelection(spinner_index);
	}


	private void setSpinnerRecognizer(){

		ArrayAdapter<String> recognizer_spinnerAdapter = new ArrayAdapter<String>(
			this,
			android.R.layout.simple_spinner_item,
			recognizer_methods);
        recognizer_spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        recognizer_spinner.setAdapter(recognizer_spinnerAdapter);

        recognizer_spinner.setSelection(recognizer_method_index);
	}

}