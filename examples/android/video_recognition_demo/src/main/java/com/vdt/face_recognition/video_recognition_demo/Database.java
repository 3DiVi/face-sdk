package com.vdt.face_recognition.video_recognition_demo;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Scanner;
import java.util.Vector;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;
import android.widget.ImageView;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.Capturer;
import com.vdt.face_recognition.sdk.Point;
import com.vdt.face_recognition.sdk.RawImage;
import com.vdt.face_recognition.sdk.RawSample;
import com.vdt.face_recognition.sdk.Recognizer;
import com.vdt.face_recognition.sdk.Template;
import com.vdt.face_recognition.sdk.VideoWorker;

import static android.os.ParcelFileDescriptor.MODE_WORLD_READABLE;


public class Database{

	private static final String TAG = "Database";

	private MainActivity activity;
	private ImageView image = null;

	public Vector<VideoWorker.DatabaseElement> vw_elements = new Vector<VideoWorker.DatabaseElement>();
	public Vector<RawSample> raw_samples = new Vector<RawSample>();
	public Vector<Bitmap> thumbnails = new Vector<Bitmap>();
	public Vector<String> names = new Vector<String>();

	private boolean isImage( String filename ){
		final String[] okFileExtensions = new String[] {
				"jpg",
				"png",
				"gif",
				"bmp",
				"jpeg"
		};
		for (String extension: okFileExtensions) {
			if (filename.toLowerCase().endsWith(extension)) {
				return true;
			}
		}
		return false;
	}

	public Database(
		final MainActivity activity,
		final String db_dir,
		Recognizer recognizer,
		final Capturer manual_capturer,
		final Capturer capturer,
		final float distance_threshold)
	{
		this.activity = activity;

		int element_id_counter = 0;

		//check db dir
		File db_dir_file = new File(db_dir);
		if(!db_dir_file.exists()){
			db_dir_file.mkdir();
		}

		//get dirs for each person		
		Vector<File> person_dirs = new Vector<File>();
		for (File file : db_dir_file.listFiles()){
			if (file.isDirectory()){
				person_dirs.add(file);
			}
		}

		//prcess each person
		for (File dir : person_dirs){

			//read name of person
			String name = null;
			for (File file : dir.listFiles()){

				if (file.getName().equals("name.txt")){
					Scanner scanner = null;
					try{
						scanner = new Scanner(file);
					}catch(FileNotFoundException e){
						e.printStackTrace();
					}
					name = scanner.useDelimiter("\\A").next();
					scanner.close();

				}
			}

			Log.i(TAG, "Processing "+name);


			for (File file : dir.listFiles()){

				final String filename = file.getName();

				if( !isImage(filename) )
					continue;

				// get sample via manualCaptures or capture
				final File points_file = new File(file.getAbsolutePath() + ".detected_points.txt");
				final File template_file = new File(file.getAbsolutePath() + ".template_" + recognizer.getMethodName() );
				RawSample sample;
				byte[] byte_image;
				try {
					byte_image = readImage(file);
				} catch(IOException e) {
					showErrorMessage("Can not open " + file.getName() + "\nErrorMessage: " + e.getMessage());
					continue;
				}

				if(points_file.exists())
				{
					Scanner points_scan = null;
					try{
						points_scan = new Scanner(points_file);
					}catch(FileNotFoundException e){
						e.printStackTrace();
					}
					Vector<Point> points = new Vector<Point>();
					while(points_scan.hasNext())
					{
						String str_buf  = points_scan.nextLine();
						String[] str_points = str_buf.split(" ");
						Point point = new Point(
							Float.parseFloat(str_points[0]),
							Float.parseFloat(str_points[1]));
						points.add(point);
					}
					points_scan.close();

					try {
						sample = manual_capturer.manualCapture(byte_image, points);
					} catch(Exception e) {
						showErrorMessage(
							"File: " + file.getName() + "\n"
							+ e.getMessage());
						continue;
					}

				}else{

					Vector<RawSample> samples_buf = null;
					try {
						samples_buf = capturer.capture(byte_image);	
					} catch(Exception e) {
						showErrorMessage(
							"File: " + file.getName() + "\n"
							+ e.getMessage());
						continue;
					}
					if(samples_buf.size() != 1){
						showErrorMessage(
							"File: " + file.getName() + "\n"
							+ Integer.toString(samples_buf.size())
							+ " faces detected, but expected 1\nIgnore image");
						continue;
					}

					sample = samples_buf.get(0);
					Vector<Point> lm_points = sample.getLandmarks();

					// save points
					try
					{
						PrintWriter points_writer = new PrintWriter(points_file.getAbsolutePath(),"UTF-8");
						for(Point p : lm_points)
						{
							points_writer.print(p.x);
							points_writer.print(" ");
							points_writer.println(p.y);
						}
						points_writer.close();
					} catch(IOException e){
						e.printStackTrace();
					}
				}

				Template templ = null;
 				if(!template_file.exists()){
					showErrorMessage("Can not open " + template_file.getName() + "\nPlease clear the Database(" + db_dir +  ") and register users again!");
					continue;
				} else {
					try {
						FileInputStream stream = new FileInputStream(template_file);
						templ = recognizer.loadTemplate(stream);
					} catch( FileNotFoundException e) {
						e.printStackTrace();
					}
				}


				//prepare data for VideoWorker
				VideoWorker.DatabaseElement vw_elem = new VideoWorker.DatabaseElement(
					element_id_counter++,
					person_dirs.indexOf(dir),
					templ,
					distance_threshold);

				vw_elements.add(vw_elem);
				raw_samples.add(sample);
				names.add(name);
				thumbnails.add(makeThumbnail(sample, name));
			}
		}

	}


	//generate crop face image with name
	private Bitmap makeThumbnail(RawSample sample, String name){

		OutputStream os = new ByteArrayOutputStream();
		sample.cutFaceImage(os, RawSample.ImageFormat.IMAGE_FORMAT_BMP, RawSample.FaceCutType.FACE_CUT_TOKEN_FRONTAL);
		byte [] byte_crop = ((ByteArrayOutputStream) os).toByteArray();
		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inMutable = true;
		Bitmap bitmap_crop = BitmapFactory.decodeByteArray(byte_crop, 0, byte_crop.length, options);


		Canvas canvas = new Canvas(bitmap_crop);

		//draw text background
		int width = bitmap_crop.getWidth();
		int height = bitmap_crop.getHeight();

		double k = 0.87;
		Rect backgroundRect = new Rect(0, (int)(k*height), width, height);
		Paint backgroundPaint = new Paint();
		backgroundPaint.setColor(0xa0000000);
		backgroundPaint.setStyle(Paint.Style.FILL);
		canvas.drawRect(backgroundRect, backgroundPaint);

		//draw name
		Paint text_paint = new Paint();
		int font_size = height / 8;
		text_paint.setColor(0xffffffff);
		text_paint.setTextSize(font_size);
		int text_pad = (int) (height * (1 - k) / 10);

		canvas.drawText(name, text_pad, height - text_pad, text_paint);


		return bitmap_crop;
	}


	private byte[] getImageBytes(File file){

		Bitmap bitmap = BitmapFactory.decodeFile(file.getAbsolutePath());

		ByteArrayOutputStream baos = new ByteArrayOutputStream();
		bitmap.compress(Bitmap.CompressFormat.PNG,100,baos);
		byte[] buf = baos.toByteArray();

		return buf;
	}


	private byte[] readImage(final File file) throws IOException
	{
		int size = (int) file.length();
		byte[] bytes = new byte[size];

		BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
		buf.read(bytes, 0, bytes.length);
		buf.close();

		return bytes;
	}


	private void showErrorMessage(final String message)
	{
		activity.runOnUiThread(new Runnable()
		{
			public void run()
			{
				Toast.makeText(activity, message, Toast.LENGTH_LONG).show();
			}
		});
	}

}