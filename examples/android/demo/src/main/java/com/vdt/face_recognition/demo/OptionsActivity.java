package com.vdt.face_recognition.demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;
import android.widget.RadioGroup;


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
	private boolean flag_eyes_openness = false;

	private int faceCutTypeId = 0;


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.options);

		CheckBox cb_rectangle = findViewById(R.id.rectangle);
		CheckBox cb_angles = findViewById(R.id.angles);
		CheckBox cb_quality = findViewById(R.id.quality);
		CheckBox cb_liveness = findViewById(R.id.liveness);
		CheckBox cb_age_and_gender = findViewById(R.id.age_and_gender);
		CheckBox cb_points = findViewById(R.id.points);
		CheckBox cb_face_quality = findViewById(R.id.face_quality);
		CheckBox cb_angles_vectors = findViewById(R.id.angles_vectors);
		CheckBox cb_emotions = findViewById(R.id.emotions);
		CheckBox cb_face_mask = findViewById(R.id.face_mask);
		CheckBox cb_eyes_openness = findViewById(R.id.eyes_openness);

		Intent getFlagsIntent = getIntent();
		boolean [] flags = getFlagsIntent.getBooleanArrayExtra("flags");
		faceCutTypeId = getFlagsIntent.getIntExtra("faceCutTypeId", 0);

		radioGroup = findViewById(R.id.face_cut_radio_group);
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
		cb_eyes_openness.setChecked(flags[10]);

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
		flag_eyes_openness = flags[10];


		//ok - button
		Button okButton = findViewById(R.id.options_ok_button);
		okButton.setOnClickListener(v -> {

			Intent setValuesIntent = new Intent();

			boolean [] flags1 = {
				flag_rectangle,
				flag_angles,
				flag_quality,
				flag_liveness,
				flag_age_and_gender,
				flag_points,
				flag_face_quality,
				flag_angles_vectors,
				flag_emotions,
				flag_face_mask,
				flag_eyes_openness
			};
			setValuesIntent.putExtra( "flags", flags1);

			RadioButton cur_rb = findViewById(radioGroup.getCheckedRadioButtonId());
			int id = cur_rb.getId();
			if (id == R.id.none_radio_button)
			{
				faceCutTypeId = 0;
			}
			else if(id == R.id.base_cut_radio_button)
			{
				faceCutTypeId = 1;
			}
			else if(id == R.id.full_cut_radio_button)
			{
				faceCutTypeId = 2;
			}
			else if(id == R.id.token_cut_radio_button)
			{
				faceCutTypeId = 3;
			}

			setValuesIntent.putExtra( "faceCutTypeId", faceCutTypeId);

			setResult(RESULT_OK, setValuesIntent);
			finish();
		});

	}


	public void onCheckboxClicked(View view) {
		boolean checked = ((CheckBox) view).isChecked();
		int id = view.getId();
		if (id == R.id.rectangle)
		{
			flag_rectangle = checked;
		}
		else if (id == R.id.angles)
		{
			flag_angles = checked;
		}
		else if (id == R.id.quality)
		{
			flag_quality = checked;
		}
		else if (id == R.id.liveness)
		{
			flag_liveness = checked;
		}
		else if (id == R.id.age_and_gender)
		{
			flag_age_and_gender = checked;
		}
		else if (id == R.id.points)
		{
			flag_points = checked;
		}
		else if (id == R.id.face_quality)
		{
			flag_face_quality = checked;
		}
		else if (id == R.id.angles_vectors)
		{
			flag_angles_vectors = checked;
		}
		else if (id == R.id.emotions)
		{
			flag_emotions = checked;
		}
		else if (id == R.id.face_mask)
		{
			flag_face_mask = checked;
		}		
		else if (id == R.id.eyes_openness)
		{
			flag_eyes_openness = checked;
		}
	}

}