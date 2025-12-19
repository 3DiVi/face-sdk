package com.vdt.face_recognition.processing_block_demo;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.os.Bundle;



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
		builder.setTitle("Error!")
				.setMessage(message)
				.setCancelable(false)
				.setNegativeButton("Close",
                        (dialog, id) -> {
                            dialog.cancel();
                            finishAffinity();
                        });
		AlertDialog alert = builder.create();
		alert.show();
    }



}