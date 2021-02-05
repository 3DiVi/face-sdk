package com.vdt.face_recognition.demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.vdt.face_recognition.demo.TheCamera;

import com.vdt.face_recognition.sdk.RawSample;

import android.widget.Toast;

public class OptionsActivity extends Activity
{
	RadioGroup radioGroup = null;

	private boolean flag_rectangle = false;
	private boolean flag_angles = false;
	private boolean flag_quality = false;
	private boolean flag_liveness = false;
	private boolean flag_age_and_gender = false;
	private boolean flag_points = false;
	private boolean flag_face_quality = false;
	private boolean flag_angles_vectors = false;
	private boolean flag_emotions = false;
	private boolean flag_face_mask = false;

	private int faceCutTypeId = 0;


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.options);

		CheckBox cb_rectangle = (CheckBox) findViewById(R.id.rectangle);
		CheckBox cb_angles = (CheckBox) findViewById(R.id.angles);
		CheckBox cb_quality = (CheckBox) findViewById(R.id.quality);
		CheckBox cb_liveness = (CheckBox) findViewById(R.id.liveness);
		CheckBox cb_age_and_gender = (CheckBox) findViewById(R.id.age_and_gender);
		CheckBox cb_points = (CheckBox) findViewById(R.id.points);
		CheckBox cb_face_quality = (CheckBox) findViewById(R.id.face_quality);
		CheckBox cb_angles_vectors = (CheckBox) findViewById(R.id.angles_vectors);
		CheckBox cb_emotions = (CheckBox) findViewById(R.id.emotions);
		CheckBox cb_face_mask = (CheckBox) findViewById(R.id.face_mask);

		Intent getFlagsIntent = getIntent();
		boolean [] flags = getFlagsIntent.getBooleanArrayExtra("flags");
		faceCutTypeId = getFlagsIntent.getIntExtra("faceCutTypeId", 0);

		radioGroup = (RadioGroup) findViewById(R.id.face_cut_radio_group);
		RadioButton rb = (RadioButton) radioGroup.getChildAt(faceCutTypeId);
		rb.setChecked(true);

		cb_rectangle.setChecked(flags[0]);
		cb_angles.setChecked(flags[1]);
		cb_quality.setChecked(flags[2]);
		cb_liveness.setChecked(flags[3]);
		cb_age_and_gender.setChecked(flags[4]);
		cb_points.setChecked(flags[5]);
		cb_face_quality.setChecked(flags[6]);
		cb_angles_vectors.setChecked(flags[7]);
		cb_emotions.setChecked(flags[8]);
		cb_face_mask.setChecked(flags[9]);

		flag_rectangle = flags[0];
		flag_angles = flags[1];
		flag_quality = flags[2];
		flag_liveness = flags[3];
		flag_age_and_gender = flags[4];
		flag_points = flags[5];
		flag_face_quality = flags[6];
		flag_angles_vectors = flags[7];
		flag_emotions = flags[8];
		flag_face_mask = flags[9];


		//ok - button
		Button okButton = (Button) findViewById(R.id.options_ok_button);
		okButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {

            	Intent setValuesIntent = new Intent();

            	boolean [] flags = {
            		flag_rectangle,
            		flag_angles,
            		flag_quality,
            		flag_liveness,
            		flag_age_and_gender,
            		flag_points,
            		flag_face_quality,
            		flag_angles_vectors,
            		flag_emotions,
            		flag_face_mask
            	};
            	setValuesIntent.putExtra( "flags", flags);

            	RadioButton cur_rb = (RadioButton) findViewById(radioGroup.getCheckedRadioButtonId());
            	switch(cur_rb.getId()){

            		case R.id.none_radio_button:
            			faceCutTypeId = 0;
            			break;

            		case R.id.base_cut_radio_button:
            			faceCutTypeId = 1;
            			break;

            		case R.id.full_cut_radio_button:
            			faceCutTypeId = 2;
            			break;

            		case R.id.token_cut_radio_button:
            			faceCutTypeId = 3;
            			break;
            	}
            	setValuesIntent.putExtra( "faceCutTypeId", faceCutTypeId);

            	setResult(RESULT_OK, setValuesIntent);
            	finish();
            }
        });

	}


	public void onCheckboxClicked(View view) {
	    boolean checked = ((CheckBox) view).isChecked();

	    switch(view.getId()) {

	        case R.id.rectangle:
	            if (checked)
	            	flag_rectangle = true;
	            else
	            	flag_rectangle = false;
	            break;

	        case R.id.angles:
	            if (checked)
	            	flag_angles = true;
	            else
	            	flag_angles = false;
	            break;

	        case R.id.quality:
	            if (checked)
	            	flag_quality = true;
	            else
	            	flag_quality = false;
	            break;

	        case R.id.liveness:
	            if (checked)
	            	flag_liveness = true;
	            else
	            	flag_liveness = false;
	            break;

	        case R.id.age_and_gender:
	            if (checked)
	            	flag_age_and_gender = true;
	            else
	            	flag_age_and_gender = false;
	            break;

	        case R.id.points:
	            if (checked)
	            	flag_points = true;
	            else
	            	flag_points = false;
	            break;

	        case R.id.face_quality:
	            if (checked)
	            	flag_face_quality = true;
	            else
	            	flag_face_quality = false;
	            break;

	        case R.id.angles_vectors:
	            if (checked)
	            	flag_angles_vectors = true;
	            else
	            	flag_angles_vectors = false;
	            break;

	        case R.id.emotions:
	            if (checked)
	            	flag_emotions = true;
	            else
	            	flag_emotions = false;
	            break;

	        case R.id.face_mask:
	            if (checked)
	            	flag_face_mask = true;
	            else
	            	flag_face_mask = false;
	            break;

    	}
	}

}