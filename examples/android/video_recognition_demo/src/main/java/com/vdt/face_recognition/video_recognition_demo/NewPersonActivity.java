package com.vdt.face_recognition.video_recognition_demo;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Vector;

import android.app.Activity;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.Capturer;
import com.vdt.face_recognition.sdk.Recognizer;
import com.vdt.face_recognition.sdk.FaceQualityEstimator;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Point;
import com.vdt.face_recognition.sdk.RawImage;
import com.vdt.face_recognition.sdk.RawSample;
import com.vdt.face_recognition.sdk.Template;
import com.vdt.face_recognition.sdk.utils.Converter_YUV_NV_2_ARGB;

import com.vdt.face_recognition.video_recognition_demo.TheCamera;
import com.vdt.face_recognition.video_recognition_demo.TheCameraPainter;


public class NewPersonActivity extends Activity implements TheCameraPainter{

	private static final String TAG = "NewPerson";
	private static final String SETTINGS_NAME = "SETTINGS";

	private TheCamera camera = null;
	private Capturer capturer = null;
	private Recognizer recognizer = null;
	private FaceQualityEstimator faceQualityEstimator = null;

	private ImageView mainImageView = null;
	private ImageView currentFaceImageView = null;
	private ImageView bestFaceImageView = null;

	private Button addButton = null;
	private EditText nameEditText = null;

	private Bitmap bestImage = null;
	private Bitmap bestFaceCut = null;
	private float bestFaceQuality = -Float.MAX_VALUE;
	private Vector<Point> bestFacePoints = null;
	private Template bestFaceTemplate = null;

	private String points_postfix = ".detected_points.txt";


	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.new_person);

		//get ImageViews
		mainImageView 			= (ImageView) findViewById(R.id.mainImageView);
		currentFaceImageView 	= (ImageView) findViewById(R.id.currentFaceImageView);
		bestFaceImageView 		= (ImageView) findViewById(R.id.bestFaceImageView);

		final FacerecService service = MainActivity.getService();

		final SharedPreferences shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
		//init Recognizer
		int method_index = shared_settings.getInt("method index", 0);
		String method_recognizer = null;
		switch (method_index){
			case 0:
				method_recognizer = shared_settings.getString("rec_method0", null);
				break;
			case 1:
				method_recognizer = shared_settings.getString("rec_method1", null);
				break;
		}
		recognizer = service.createRecognizer(method_recognizer, true, false, false);

		//init capturer
		FacerecService.Config capturer_conf = service.new Config("fda_tracker_capturer.xml");
		capturer_conf.overrideParameter("downscale_rawsamples_to_preferred_size", 0);
		capturer = service.createCapturer(capturer_conf);
		//create face quality estimator
		faceQualityEstimator = service.createFaceQualityEstimator("face_quality_estimator.xml");

		//get name EditText
		nameEditText = (EditText) findViewById(R.id.name_editText);

		//set button onClick listener
		final Handler handler = new Handler();
		addButton = (Button) findViewById(R.id.add_button);
		addButton.setOnClickListener(new View.OnClickListener() {
			public void onClick(View v) {

				String name = nameEditText.getText().toString();
				if(name.equals("")){
					Toast.makeText(getApplicationContext(), "Add name", Toast.LENGTH_SHORT).show();
					return;
				}

				try{
					final String DATABASE_DIR = getApplicationInfo().dataDir + "/fsdk/database/";

					//directory new person
					File new_person_dir = new File(DATABASE_DIR,name);

					//check contains new dir in db
					final File db_file = new File(DATABASE_DIR);
					final File [] persons_array = db_file.listFiles();
					if(Arrays.asList(persons_array).contains(new_person_dir)){
						Toast.makeText(getApplicationContext(), "This person exists\nAdding new photo", Toast.LENGTH_SHORT).show();
					}else{
						//create person folder
						Toast.makeText(getApplicationContext(), "Create new person", Toast.LENGTH_SHORT).show();
						new_person_dir.mkdir();

						//create name file
						File name_file = new File(new_person_dir, "name.txt");
						PrintWriter writer = new PrintWriter(name_file.getAbsolutePath(),"UTF-8");
						writer.println(name);
						writer.close();
					}

					// generate image filename
					int images_count = -1;
					File new_photo_file;
					File new_points_file;
					do {
						images_count++;
						final String short_new_photo_filename = Integer.toString(images_count) + ".jpg";
						new_photo_file = new File(new_person_dir, short_new_photo_filename);
						new_points_file = new File(new_person_dir, short_new_photo_filename + points_postfix);
					} while(new_photo_file.exists() || new_points_file.exists());

					final String new_photo_filename = new_photo_file.getAbsolutePath();

					//save new face points
					final String points_filename = new_photo_filename + points_postfix;
					PrintWriter points_writer = new PrintWriter(points_filename,"UTF-8");
					for(Point p : bestFacePoints)
					{
						points_writer.print(p.x);
						points_writer.print(" ");
						points_writer.println(p.y);
					}
					points_writer.close();

					FileOutputStream out = new FileOutputStream(new_photo_filename);
					bestImage.compress(Bitmap.CompressFormat.JPEG, 100, out);

					if( bestFaceTemplate != null ){
						final String template_filename = new_photo_file.getAbsolutePath()  + ".template_" + recognizer.getMethodName();
						FileOutputStream template_file = new FileOutputStream(template_filename);
						bestFaceTemplate.save(template_file);
					}
				}catch(IOException e){
					e.printStackTrace();
				}

				//update flag
				final SharedPreferences shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
				SharedPreferences.Editor editor = shared_settings.edit();
				editor.putBoolean("recognizer_updated", true);
				editor.commit();

				//close activity
				handler.postDelayed(new Runnable(){
					@Override
					public void run(){
						finish();
					}
				}, Toast.LENGTH_SHORT);
			}
		});

		//open camera
		camera = new TheCamera(this);
		camera.open(this);
	}


	@Override
	public void processingImage(byte[] data, int width, int height){
		
		// get RawImage
		RawImage rawImage = new RawImage(
			width,
			height,
			RawImage.Format.FORMAT_YUV_NV21,
			data);

		//get face cut
		Vector<RawSample> samples = capturer.capture(rawImage);

		//set main ImageView
		int[] argb = Converter_YUV_NV_2_ARGB.convert_yuv_nv_2_argb(false, data, width, height);

		Bitmap immut_bitmap = Bitmap.createBitmap(
			argb,
			width,
			height,
			Bitmap.Config.ARGB_8888);
		mainImageView.setImageBitmap(immut_bitmap);

		//check samples
		if(samples.size() != 1){
			//not processing
			return;
		}

		//get current face
		RawSample.FaceCutType faceCutType = RawSample.FaceCutType.FACE_CUT_TOKEN_FRONTAL;
		RawSample sample = samples.get(0);
		OutputStream os = new ByteArrayOutputStream();
		sample.cutFaceImage(os, RawSample.ImageFormat.IMAGE_FORMAT_BMP, faceCutType);
		byte [] byte_crop = ((ByteArrayOutputStream) os).toByteArray();
		Bitmap current_crop = BitmapFactory.decodeByteArray(byte_crop, 0, byte_crop.length);

		//set current face ImageView
		currentFaceImageView.setImageBitmap(current_crop);

		//update best face cut
		float currentFaceQuality = faceQualityEstimator.estimateQuality(sample);
		if (currentFaceQuality > bestFaceQuality){
			bestFaceQuality = currentFaceQuality;
			bestFaceCut = current_crop.copy(current_crop.getConfig(), true);
			bestFacePoints = sample.getLandmarks();
			bestImage = immut_bitmap.copy(immut_bitmap.getConfig(), true);
			bestFaceImageView.setImageBitmap(bestFaceCut);
			bestFaceTemplate = recognizer.processing(sample);
		}
	}


	@Override
	protected void onPause() {
		camera.close();
		super.onPause();

	}


	@Override
	protected void onDestroy() {
		camera.close();

		finish();
		super.onDestroy();
	}
}