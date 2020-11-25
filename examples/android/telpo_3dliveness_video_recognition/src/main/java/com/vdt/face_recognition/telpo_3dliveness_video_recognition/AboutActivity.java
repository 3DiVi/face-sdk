package com.vdt.face_recognition.telpo_3dliveness_video_recognition;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;


public class AboutActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.about);


		findViewById( R.id.aboutOkButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			finish();
		}});

    }
}