package com.vdt.face_recognition.demo;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Intent;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.view.View;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Spinner;
import android.widget.TextView;

import com.vdt.face_recognition.demo.TheCamera;


public class SettingsActivity extends Activity{

	private static final String TAG = "SettingsActivity";
	private ArrayList<RadioButton> rb_list = new ArrayList<RadioButton>();
	private RadioGroup radioGroup = null;
	private RadioButton rb0 = null;
	private RadioButton rb1 = null;
	private RadioButton rb2 = null;

	private Spinner spinner = null;

	private ArrayList<ArrayList<String>> cams_resolutions = new ArrayList<ArrayList<String>>();

	private int selected_camera_id = 0;
	private String selected_resolution = "640x480";


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.settings);

		//get previous or defualt values
		Intent getValuesIntent = getIntent();
		selected_camera_id = getValuesIntent.getIntExtra("selected_camera_id", selected_camera_id);

		//radio buttons init
		radioGroup = (RadioGroup) findViewById(R.id.cameras_radio_group);
		rb0 = (RadioButton) findViewById(R.id.camera0_radio_button);
		rb1 = (RadioButton) findViewById(R.id.camera1_radio_button);
		rb2 = (RadioButton) findViewById(R.id.camera2_radio_button);

		rb_list.add(rb0);
		rb_list.add(rb1);
		rb_list.add(rb2);

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
			if (cam_id == selected_camera_id) {
				is_selected_available = true;
			}
		}

		if (!is_selected_available) {
			selected_camera_id = availableCameras.get(0).id;
		}

		RadioButton rb = (RadioButton) radioGroup.getChildAt(selected_camera_id);
		rb.setChecked(true);

        spinner = (Spinner) findViewById(R.id.resolution_spinner);
        selected_resolution = getValuesIntent.getStringExtra("selected_resolution");
        setSpinnerResolutions(selected_resolution);
        spinner.setSelection(cams_resolutions.get(selected_camera_id).indexOf(selected_resolution));


		//ok - button
		Button okButton = (Button) findViewById(R.id.settings_ok_button);
		okButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

            	Intent setValuesIntent = new Intent();

            	setValuesIntent.putExtra("cam_id", selected_camera_id);
            	setValuesIntent.putExtra("selected_resolution", spinner.getSelectedItem().toString());

            	setResult(RESULT_OK, setValuesIntent);
            	finish();
            }
        });

	}


	public void onRadioClicked(View view) {

		switch (view.getId()) {

			case R.id.camera0_radio_button:
				selected_camera_id = 0;
				setSpinnerResolutions("640x480");
				break;

			case R.id.camera1_radio_button:
				selected_camera_id = 1;
				setSpinnerResolutions("640x480");
				break;

			case R.id.camera2_radio_button:
				selected_camera_id = 2;
				setSpinnerResolutions("640x480");
				break;
		}
	}


	private void setSpinnerResolutions(String defualt_resolution){
		
		ArrayAdapter<String> spinnerAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, cams_resolutions.get(selected_camera_id).toArray(new String[0]));
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(spinnerAdapter);

        int spinner_index = cams_resolutions.get(selected_camera_id).indexOf(defualt_resolution);
        spinner_index = (spinner_index == -1) ? 0 : spinner_index;
        spinner.setSelection(spinner_index);
	}

}