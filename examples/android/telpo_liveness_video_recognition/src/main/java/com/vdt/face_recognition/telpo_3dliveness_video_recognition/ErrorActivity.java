package com.vdt.face_recognition.telpo_liveness_video_recognition;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;

import com.vdt.face_recognition.sdk.SDKException;

// activity shown in case of fatal error
public class ErrorActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.error);

		Intent getValuesIntent = getIntent();
		final String message = getValuesIntent.getStringExtra("error message");

		AlertDialog.Builder builder = new AlertDialog.Builder(this);

		builder
			.setTitle("Error!")
			.setMessage(message)
			.setCancelable(false)
			.setNegativeButton(
				"Close",
				new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface dialog, int id) {
						dialog.cancel();
						finishAffinity();
					}
				});

		AlertDialog alert = builder.create();

		alert.show();
    }

}