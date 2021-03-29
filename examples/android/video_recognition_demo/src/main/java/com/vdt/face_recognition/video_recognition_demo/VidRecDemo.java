package com.vdt.face_recognition.video_recognition_demo;

import java.io.*;
import java.lang.Math;
import java.nio.ByteBuffer;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.SortedSet;
import java.util.TreeSet;
import java.util.Vector;

import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Handler;
import android.util.Log;
import android.util.Pair;
import android.widget.ImageView;

import com.vdt.face_recognition.sdk.ActiveLiveness;
import com.vdt.face_recognition.sdk.Capturer;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.MatchFoundCallbackData;
import com.vdt.face_recognition.sdk.Point;
import com.vdt.face_recognition.sdk.RawImage;
import com.vdt.face_recognition.sdk.RawSample;
import com.vdt.face_recognition.sdk.Recognizer;
import com.vdt.face_recognition.sdk.TrackingCallbackData;
import com.vdt.face_recognition.sdk.TrackingLostCallbackData;
import com.vdt.face_recognition.sdk.StiPersonOutdatedCallbackData;
import com.vdt.face_recognition.sdk.VideoWorker;
import com.vdt.face_recognition.sdk.utils.Converter_YUV_NV_2_ARGB;
import com.vdt.face_recognition.sdk.AgeGenderEstimator;
import com.vdt.face_recognition.sdk.EmotionsEstimator;

import com.vdt.face_recognition.video_recognition_demo.Database;
import com.vdt.face_recognition.video_recognition_demo.DrawingData;
import com.vdt.face_recognition.video_recognition_demo.FaceData;
import com.vdt.face_recognition.video_recognition_demo.TheCameraPainter;



public class VidRecDemo implements TheCameraPainter{

	private static String TAG = "VidRecDemo";
	private static final String SETTINGS_NAME = "SETTINGS";

	private MainActivity activity;
	private ImageView mainImageView = null;
	private Vector<ImageView> vid_faces_image_views = new Vector<ImageView>();
	private Vector<ImageView> db_faces_image_views = new Vector<ImageView>();

	private Capturer capturer = null;
	private VideoWorker videoWorker = null;
	private Database db =  null;

	//This thread used for background async initialization of some Face SDK components for speedup.
	private Thread init_thread;

	private String method_recognizer;
	private float threshold;

	private LinkedBlockingQueue<Pair<Integer, Bitmap>> frames = new LinkedBlockingQueue<Pair<Integer, Bitmap>>();
	private int stream_id = 0;
	private int tracking_callback_id;
	private int tracking_lost_callback_id;
	private int match_found_callback_id;
	private int sti_person_outdated_callback_id;

	Thread drawThread = null;
	private DrawingData drawingData = new DrawingData();
	private long alive_lost_time = 5000; //milliseconds


	public VidRecDemo(
			final MainActivity activity)
	{
		this.activity = activity;

		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

		Log.v(TAG, "Start VidRecDemo" );
		SharedPreferences shared_settings = activity.getSharedPreferences(SETTINGS_NAME, 0);

		// get recognizer settings
		int method_index = shared_settings.getInt("method index", 0);
		switch (method_index){
			case 0:
				method_recognizer = shared_settings.getString("rec_method0", "method9v30_recognizer.xml" );
				threshold = Float.parseFloat(shared_settings.getString("threshold0", "6800"));
				break;
			case 1:
				method_recognizer = shared_settings.getString("rec_method1", "method9v30mask_recognizer.xml" );
				threshold = Float.parseFloat(shared_settings.getString("threshold1", "3600"));
				break;
			default:
				method_recognizer = shared_settings.getString("rec_method2", "method8v7_recognizer.xml" );
				threshold = Float.parseFloat(shared_settings.getString("threshold2", "7000" ));
				break;
		}

		final FacerecService service = MainActivity.getService();
		Log.v(TAG, "Loaded settings" );

		Log.v(TAG, "Starting init thread" );
		init_thread = new Thread(new Runnable() {
			public void run() {
				synchronized (init_thread){
					// create Capturer
					FacerecService.Config capturer_conf = service.new Config("common_capturer_blf_fda_front.xml");
					capturer = service.createCapturer(capturer_conf);
					Log.v(TAG, "init thread: created capturer1" );

					// create manual_capturer and recognizer
					FacerecService.Config manual_capturer_conf = service.new Config("manual_capturer_fda.xml");
					final Recognizer recognizer = service.createRecognizer(method_recognizer, false, false, false);
					Log.v(TAG, "init thread: created recognizer " + method_recognizer );

					final Capturer manual_capturer = service.createCapturer(manual_capturer_conf);
					Log.v(TAG, "init thread: created capturer2" );

					// create database
					db = new Database( activity, activity.getApplicationInfo().dataDir + "/fsdk/database", recognizer, manual_capturer, capturer, threshold);
					Log.v(TAG, "init thread: created db" );

					// free resources
					recognizer.dispose();
					manual_capturer.dispose();
					Log.v(TAG, "init thread: done" );
				}
			}
		});

		init_thread.start();

		/*
		Vector<ActiveLiveness.CheckType> checks = new Vector<ActiveLiveness.CheckType>();
		checks.add(ActiveLiveness.CheckType.BLINK);
		checks.add(ActiveLiveness.CheckType.TURN_DOWN);
		checks.add(ActiveLiveness.CheckType.TURN_UP);
		*/

		//create videoWorker
		Log.v(TAG, "creating worker");
		videoWorker = service.createVideoWorker(
				new VideoWorker.Params()
						.video_worker_config(
								service.new Config("video_worker_fdatracker_blf_fda_front.xml")
										.overrideParameter("search_k", 10)
										.overrideParameter("recognizer_processing_less_memory_consumption", 0)
										.overrideParameter("downscale_rawsamples_to_preferred_size", 0)
						)
						.recognizer_ini_file(method_recognizer)
						.streams_count(1)
						.processing_threads_count(1)
						.matching_threads_count(1)
						// .age_gender_estimation_threads_count(1)
						// .emotions_estimation_threads_count(1)
						.short_time_identification_enabled(true)
						.short_time_identification_distance_threshold(threshold)
						.short_time_identification_outdate_time_seconds(5)
						//.active_liveness_checks_order(checks)
		);

		//add callbacks
		tracking_callback_id = videoWorker.addTrackingCallbackU(new TrackingCallbacker());
		tracking_lost_callback_id = videoWorker.addTrackingLostCallbackU(new TrackingLostCallbacker());
		match_found_callback_id = videoWorker.addMatchFoundCallbackU(new MatchFoundCallbacker());
		sti_person_outdated_callback_id = videoWorker.addStiPersonOutdatedCallbackU(new StiPersonOutdatedCallbacker());
		Log.v(TAG, "creating worker done" );

		synchronized ( init_thread ){
			videoWorker.setDatabase(db.vw_elements, Recognizer.SearchAccelerationType.SEARCH_ACCELERATION_1);
		}
		Log.v(TAG, "setting worker db done" );
	}


	@Override
	public void processingImage(byte[] data, int width, int height){

		// get RawImage
		RawImage frame = new RawImage(
			width,
			height,
			RawImage.Format.FORMAT_YUV_NV21,
			data);

		int[] argb = Converter_YUV_NV_2_ARGB.convert_yuv_nv_2_argb(false, data, width, height);

		Bitmap immut_bitmap = Bitmap.createBitmap(
			argb,
			width,
			height,
			Bitmap.Config.ARGB_8888);
		Bitmap mut_bitmap = immut_bitmap.copy(Bitmap.Config.ARGB_8888, true);

		int frame_id = videoWorker.addVideoFrame(frame, stream_id);
		frames.offer(new Pair<Integer, Bitmap>(frame_id, mut_bitmap));

		videoWorker.checkExceptions();
	}


	private class TrackingCallbacker implements VideoWorker.TrackingCallback{

		public void call(TrackingCallbackData data){

			if (data.stream_id != stream_id)
				return;

			//get frame
			Bitmap frame;
			while(true){

				if(frames.size() == 0){
					return;
				}

				if(frames.peek().first == data.frame_id){
					frame = frames.poll().second;
					break;
				}else{
					Log.v(TAG, "Skiped " + Integer.toString(stream_id) + ": " + Integer.toString(frames.poll().first));
				}
			}

			//update data
			synchronized(drawingData){

				drawingData.frame = frame;
				drawingData.frame_id = data.frame_id;
				drawingData.updated = true;

				for (int i = 0; i < data.samples.size(); i++){

					RawSample sample = data.samples.get(i);
					int id = sample.getID();
					/*
					ActiveLiveness.ActiveLivenessStatus st = data.samples_active_liveness_status.get(i);
					Log.d("liveness", "Check: "+  st.check_type.toString() +
							". Status: " + st.verdict.toString() +
							". Progress: "+ st.progress_level );
					*/

					if(!drawingData.faces.containsKey(id)){
						FaceData faceData = new FaceData(sample);
						drawingData.faces.put(id, faceData);
					}

					FaceData face = drawingData.faces.get(id);

					face.frame_id = sample.getFrameID();
					face.lost = false;
					face.weak = data.samples_weak.get(i);
					face.sample = sample;

					/*
					// just print age gender and emotions
					Log.i(TAG, "  age gender set: " + Boolean.toString(data.samples_track_age_gender_set.get(i)));
					Log.i(TAG, "  emotions set: " + Boolean.toString(data.samples_track_emotions_set.get(i)));

					if(data.samples_track_age_gender_set.get(i))
					{
						Log.i(TAG, "  age:       " + data.samples_track_age_gender.get(i).age.name());
						Log.i(TAG, "  gender:    " + data.samples_track_age_gender.get(i).gender.name());
						Log.i(TAG, "  age_years: " + Float.toString(data.samples_track_age_gender.get(i).age_years));
					}

					if(data.samples_track_emotions_set.get(i))
					{
						for(int j = 0; j < data.samples_track_emotions.get(i).size(); ++j)
							Log.i(TAG,
								"  emotion: " + data.samples_track_emotions.get(i).get(j).emotion.name() +
								" confidence: " + Float.toString(data.samples_track_emotions.get(i).get(j).confidence) );
					}
					*/
				}
			}
		}
	}


	private class TrackingLostCallbacker implements VideoWorker.TrackingLostCallback{
	
		public void call(TrackingLostCallbackData data){

			if (data.stream_id != stream_id)
				return;

			Log.i(TAG, "tracking lost callback: "
				+ "  track_id: " + Integer.toString(data.track_id)
				+ "  sti_person_id_set: " + Boolean.toString(data.sti_person_id_set)
				+ "  sti_person_id: " + Integer.toString(data.sti_person_id)
				);

			synchronized(drawingData){
				
				if(drawingData.faces.isEmpty()){
					return;
				}

				FaceData face = drawingData.faces.get(data.track_id);

				face.lost = true;
				face.lost_time = System.currentTimeMillis();

				if (data.best_quality_sample != null)
				{
					face.sample = data.best_quality_sample;
				}

				drawingData.updated = true;
			}
		}
	}

	private class StiPersonOutdatedCallbacker implements VideoWorker.StiPersonOutdatedCallbackU{

		public void call(StiPersonOutdatedCallbackData data){

			if (data.stream_id != stream_id)
				return;

			Log.i(TAG, "sti person outdated callback: "
				+ "  sti_person_id: " + Integer.toString(data.sti_person_id)
				);
		}
	}

	private class MatchFoundCallbacker implements VideoWorker.MatchFoundCallback{
	
		public void call(MatchFoundCallbackData data){

			if (data.stream_id != stream_id)
				return;

			int id = data.sample.getID();

			synchronized(drawingData){
				
				if(drawingData.faces.isEmpty()){
					return;
				}

				drawingData.faces.get(id).match_db_index = data.search_result.get(0).element_id;
				drawingData.updated = true;
			}
		}
	}


	public void startDrawThread(final Handler handler){

		if (drawThread != null){
			closeDrawThread();
		}

		mainImageView = (ImageView) activity.findViewById(R.id.mainImageView);

		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView0));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView1));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView2));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView3));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView4));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView5));
		vid_faces_image_views.add(	(ImageView) activity.findViewById(R.id.video_faces_ImageView6));

		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView0));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView1));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView2));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView3));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView4));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView5));
		db_faces_image_views.add(	(ImageView) activity.findViewById(R.id.db_faces_ImageView6));


		drawThread = new Thread(new Runnable(){
			
			public void run(){
			
				while(true){

					try {
						Thread.sleep(20);
					} catch(InterruptedException ex) {
						Thread.currentThread().interrupt();
						break;
					}

					if(drawingData.updated){
						handler.sendEmptyMessage(0);
					}

				}
			}

		});
		drawThread.start();
	}


	public void updateImageViews(){		
		synchronized(drawingData){

			//draw only first 7 faces
			int count = 0;
			SortedSet<Integer> keys = new TreeSet<Integer>(drawingData.faces.keySet());

			for(Integer key : keys){

				FaceData face = drawingData.faces.get(key);

				if(face.frame_id == drawingData.frame_id && !face.lost){

					//get points
					Vector<Point> points = face.sample.getLandmarks();

					//compute center point
					Point center_point = new Point(0, 0);
					for(Point p : points){
						center_point.x += p.x;
						center_point.y += p.y;
					}

					center_point.x /= points.size();
					center_point.y /= points.size();

					//compute radius
					float radius = 0;
					for(Point p : points){
						radius += distance(p, center_point);
					}
					radius /= points.size();
					radius *= 2;

					//set paint
					Paint paint = new Paint();
					paint.setStyle(Paint.Style.STROKE);
					paint.setStrokeWidth(2);

					//dash weak faces
					if (face.weak){
						DashPathEffect dashPath = new DashPathEffect(new float[]{20,10}, 0);
						paint.setPathEffect(dashPath);
					}

					if(face.match_db_index >= 0){
						paint.setColor(0xff00ff00);
					}else{
						paint.setColor(0xffff0000);
					}

					//draw
					Canvas main_canvas = new Canvas(drawingData.frame);
					main_canvas.drawCircle(center_point.x, center_point.y, radius, paint);
				}


				//RawSample to Bitmap
				OutputStream os = new ByteArrayOutputStream();
				face.sample.cutFaceImage(os, RawSample.ImageFormat.IMAGE_FORMAT_JPG, RawSample.FaceCutType.FACE_CUT_BASE);
				byte [] byte_sample = ((ByteArrayOutputStream) os).toByteArray();
				Bitmap thumbnail_from_frame = BitmapFactory.decodeByteArray(byte_sample, 0, byte_sample.length);

				//if exist get thumbnail from db else empty
				Bitmap thumbnail_from_db;
				if(face.match_db_index >= 0){
					thumbnail_from_db = db.thumbnails.get((int) face.match_db_index);
				}else{
					thumbnail_from_db = Bitmap.createBitmap(thumbnail_from_frame.getWidth(), thumbnail_from_frame.getHeight(), Bitmap.Config.ARGB_8888);
					Canvas canvas = new Canvas(thumbnail_from_db);
					Paint paint = new Paint();
					paint.setColor(0xff555555);
					canvas.drawRect(0, 0, thumbnail_from_db.getWidth(), thumbnail_from_db.getHeight(), paint);
				}

				//processing lost face
				if(face.lost){

					long cur_time = System.currentTimeMillis();

					if(cur_time - face.lost_time > alive_lost_time){
						//delete old face
						drawingData.faces.remove(key);
						clearImageViews(count);
						continue;
					}else{
						//fade lost face
						int alpha = (int) ((1 - (1.0*cur_time - face.lost_time)/alive_lost_time) * 255);
						vid_faces_image_views.get(count).setImageAlpha(alpha);
						db_faces_image_views.get(count).setImageAlpha(alpha);
					}
				}

				//draw thumbnail from frame
				vid_faces_image_views.get(count).setImageBitmap(thumbnail_from_frame);
				//if exist draw face from database else empty
				db_faces_image_views.get(count).setImageBitmap(thumbnail_from_db);



				if (count >= 6)
					break;

				count++;

			}

			mainImageView.setImageBitmap(drawingData.frame);
			drawingData.updated = false;			
		}
	}


	public void closeDrawThread(){
		if (drawThread != null){
			drawThread.interrupt();
		}
	}


	private float distance(Point p1, Point p2){
		return (float) Math.sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
	}


	private void clearImageViews(int currentImageViewID){
		for(int i = currentImageViewID; i < vid_faces_image_views.size(); i++){

			vid_faces_image_views.get(i).setImageAlpha(0xff);
			vid_faces_image_views.get(i).setImageResource(android.R.color.transparent);

			db_faces_image_views.get(i).setImageAlpha(0xff);
			db_faces_image_views.get(i).setImageResource(android.R.color.transparent);
		}
	}


	private void clearDrawingData(){
		synchronized(drawingData){
			drawingData = new DrawingData();
		}
	}

	public void dispose()
	{
		capturer.dispose();
		videoWorker.dispose();
	}

}
