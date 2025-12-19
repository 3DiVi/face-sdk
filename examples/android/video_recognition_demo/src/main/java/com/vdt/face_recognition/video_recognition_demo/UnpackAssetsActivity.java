package com.vdt.face_recognition.video_recognition_demo;

import android.app.Activity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayDeque;
import java.util.Queue;


// activity to upack all assets
public class UnpackAssetsActivity extends Activity
{
	Thread the_thread = null;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		setContentView(R.layout.unpack_assets);

		// unpack in different thread to let the progressbar spin
		the_thread = new Thread(() -> {
		try
		{
			final AssetManager asman = getAssets();

			// read first line from assets-hash.txt
			final String new_hash = new BufferedReader(new InputStreamReader(asman.open("assets-hash.txt"))).readLine();

			// and compare it with what we have already
			SharedPreferences shpr = getSharedPreferences("fe9733f0bfb7", 0);

			final String prev_hash = shpr.getString("assets-hash", null);

			// unpack everything again, if something changes
			if(prev_hash == null || !prev_hash.equals(new_hash))
			{
				final byte[] buffer = new byte[10240];

				final String persistent_dir = getApplicationInfo().dataDir;

				Queue<String> queue = new ArrayDeque<>();
				queue.add("conf");
				queue.add("share");
				queue.add("license");

				while(!queue.isEmpty())
				{
					final String path = queue.element();
					queue.remove();

					final String[] list = asman.list(path);

					if(list.length == 0)
					{
						final InputStream file_stream = asman.open(path);

						final String full_path = persistent_dir + "/fsdk/" + path;

						new File(full_path).getParentFile().mkdirs();

						final FileOutputStream out_file = new FileOutputStream(full_path);

						for(;;)
						{
							final int read = file_stream.read(buffer);

							if(read <= 0)
								break;

							out_file.write(buffer, 0, read);
						}

						file_stream.close();
						out_file.close();
					}
					else
					{
						for(String p : list)
							queue.add(path + "/" + p);
					}
				}

				SharedPreferences.Editor editor = shpr.edit();
				editor.putString("assets-hash", new_hash);
				while(!editor.commit());
			}

			Intent intent = new Intent(getApplicationContext(), MainActivity.class);
			startActivity(intent);
			finish();
		}
		catch(Exception e)
		{
			Log.e("UnpackAssetsActivity", e.getMessage());
			e.printStackTrace();
			finishAffinity();
		}});

		the_thread.start();
	}


	@Override
	protected void onDestroy()
	{
		if(the_thread != null)
		{
			try
			{
				the_thread.join();
			}
			catch(Exception e)
			{
				Log.i("UnpackAssetsActivity", e.getMessage());
			}
		}

		super.onDestroy();
	}
}
