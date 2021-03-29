package com.vdt.face_recognition.telpo_liveness_video_recognition;

import com.common.thermalimage.CalibrationCallBack;
import com.common.thermalimage.HotImageCallback;
import com.common.thermalimage.TemperatureBitmapData;
import com.common.thermalimage.TemperatureData;
import com.common.thermalimage.ThermalImageUtil;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.SharedPreferences;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.Manifest;
import android.hardware.Camera;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.util.ArrayDeque;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.TreeMap;
import java.util.Vector;


import com.vdt.face_recognition.sdk.DepthLivenessEstimator;
import com.vdt.face_recognition.sdk.DepthMapRaw;
import com.vdt.face_recognition.sdk.IRLivenessEstimator;
import com.vdt.face_recognition.sdk.IRFrameRaw;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.MatchFoundCallbackData;
import com.vdt.face_recognition.sdk.Point;
import com.vdt.face_recognition.sdk.RawImage;
import com.vdt.face_recognition.sdk.RawSample;
import com.vdt.face_recognition.sdk.Recognizer;
import com.vdt.face_recognition.sdk.SDKException;
import com.vdt.face_recognition.sdk.Template;
import com.vdt.face_recognition.sdk.TrackingCallbackData;
import com.vdt.face_recognition.sdk.TrackingLostCallbackData;
import com.vdt.face_recognition.sdk.utils.Converter_YUV_NV_2_ARGB;
import com.vdt.face_recognition.sdk.VideoWorker;


import org.openni.android.OpenNIHelper;
import org.openni.Device;
import org.openni.DeviceInfo;
import org.openni.ImageRegistrationMode;
import org.openni.OpenNI;
import org.openni.PixelFormat;
import org.openni.SensorType;
import org.openni.VideoFrameRef;
import org.openni.VideoMode;
import org.openni.VideoStream;


import com.vdt.face_recognition.telpo_liveness_video_recognition.TheCamera;

// activity that manage authentication and registration
public class AuthActivity
	extends Activity
	implements
		TheCamera.FramesProcessor,
		VideoWorker.TrackingCallbackU,
		VideoWorker.TrackingLostCallbackU,
		VideoWorker.MatchFoundCallbackU,
		OpenNIHelper.DeviceOpenListener,
		VideoStream.NewFrameListener
{
	// logcat tag
	private static final String TAG = "AuthActivity";

	// camera settings name
	private static final String SETTINGS_NAME = "SETTINGS";

	// our rgb frames source
	private TheCamera rgb_camera = null;

	// our rgb frames source
	private TheCamera ir_camera = null;

	// face sdk objects
	private FacerecService facerec_service = null;
	private Recognizer recognizer = null;
	private VideoWorker video_worker = null;

	// persons database
	private static class DBSample
	{
		public String name;
		public long person_id;
		public Template template;
		public Bitmap thumbnail;
	}

	private String database_directory;
	private int database_persons_count;
	private Vector<DBSample> database;

	// recognizer, video_worker, database will be created asynchronously, so we use atomic flag to control that all is set up
	private AtomicBoolean recognition_set = new AtomicBoolean(false);

	// recognizer config
	// use method9v30mask_recognizer.xml for mask-wearing face identification
	//private final String recognizer_config = "method9v30mask_recognizer.xml";
	private final String recognizer_config = "method9v30_recognizer.xml";

	private String video_worker_config = "";

	private String camera_type = "";

	private boolean depth_camera_found = false;
	private boolean ir_camera_found = false;
	private boolean termo_camera_found = false;

	private int base_angle;

	// desired FAR - false acceptance rate
	private final double recognition_far_threshold = 1e-6;

	// distance threshold, computed from desired FAR
	private double recognition_distance_threshold;

	// thread that will create videoworker
	private Thread vw_start_thread = null;

	// here we work in two modes - authentication and registration
	//  so work_mode_registration is set to false for authentication, and to true for registration
	private AtomicBoolean work_mode_registration = new AtomicBoolean(false);

	// in registration mode we choose the "best" face sample
	//   and store it in registration_current_sample
	//   and its quality in registration_current_sample_quality
	private RawSample registration_current_sample = null;   // access only in ui thread
	private float registration_current_sample_quality = 0;  // access only in ui thread
	private boolean registration_name_valid = false;

	private ThermalImageUtil temperatureUtil;

	// current tracks info
	private static class TrackInfo
	{
		Bitmap video_sample_thumbnail = null;

		boolean matched = false;
		String name = null;
		Bitmap db_sample_thumbnail = null;

		String temperature_text = "";
	};
	private Map<Integer, TrackInfo> tracks_info = new TreeMap<Integer, TrackInfo>();

	TrackInfo getTrackInfo(int track_id)
	{
		final TrackInfo tmp1 = new TrackInfo();
		final TrackInfo tmp2 = tracks_info.putIfAbsent(track_id, tmp1);
		return (tmp2 != null) ? tmp2 : tmp1;
	}



	private OpenNIHelper openni_helper;

	private VideoStream depth_stream;

	// load openni libs
	static{
		System.loadLibrary("usb");
		System.loadLibrary("orbbecusb2");
		System.loadLibrary("orbbec");
		System.loadLibrary("OniFile");
		System.loadLibrary("OpenNI2");
		System.loadLibrary("OpenNI2.jni");
		// System.loadLibrary("PS1080");
		// System.loadLibrary("PSLink");
	}



	// needed runtime permissions
	private String[] permissions_str = new String[] {
		Manifest.permission.CAMERA,
		Manifest.permission.READ_EXTERNAL_STORAGE,
		Manifest.permission.WRITE_EXTERNAL_STORAGE,
		Manifest.permission.READ_PHONE_STATE,
	};

	// text views corresponding to runtime permissions status
	private int[] permissions_tv_id = new int[] {
		R.id.camera_perm_status,
		R.id.read_storage_perm_status,
		R.id.write_storage_perm_status,
		R.id.read_phone_perm_status,
	};


	// extend SurfaceView in order to draw on it
	public static class DrawingSurfaceView extends SurfaceView
	{
		// x, y, radius, color for each face circle
		public Vector<Float> circles_data = new Vector<Float>();

		public int image_width = 1;

		public DrawingSurfaceView(Context context, AttributeSet attrs)
		{
			super(context, attrs);
			setWillNotDraw(false);
		}

		@Override
		protected void onDraw(Canvas canvas)
		{
			super.onDraw(canvas);

			Paint paint0 = new Paint();
			paint0.setStyle(Paint.Style.STROKE);
			paint0.setStrokeWidth(2);
			paint0.setColor(0xffff0000);

			Paint paint1 = new Paint();
			paint1.setStyle(Paint.Style.STROKE);
			paint1.setStrokeWidth(2);
			paint1.setColor(0xff00ff00);

			synchronized(circles_data)
			{
				// SurfaceView have its own size not equal to the image's
				float scale = getWidth() / (float) image_width;

				for(int i = 0; i + 3 < circles_data.size(); i += 4)
				{
					canvas.drawCircle(
						circles_data.get(i + 0) * scale,
						circles_data.get(i + 1) * scale,
						circles_data.get(i + 2) * scale,
						circles_data.get(i + 3) > 0 ? paint1 : paint0);
				}
			}
		}
	}


	private Bitmap makeSampleThumbnail(RawSample sample)
	{
		ByteArrayOutputStream image_stream = new ByteArrayOutputStream();

		sample.cutFaceImage(
			image_stream,
			RawSample.ImageFormat.IMAGE_FORMAT_BMP,
			RawSample.FaceCutType.FACE_CUT_BASE);

		final byte[] image_bmp = image_stream.toByteArray();

		Bitmap bitmap = BitmapFactory.decodeByteArray(image_bmp, 0, image_bmp.length);

		// Rotate thumbnail
		Matrix matrix = new Matrix();
		matrix.postRotate((float)base_angle);
		bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
		return bitmap;
	}

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		// view persmissions status
		setContentView(R.layout.permissions);

		// check and request permissions
		int granted_count = 0;

		for(int i = 0; i < permissions_str.length; ++i)
		{
			String perstr = permissions_str[i];

			if(ContextCompat.checkSelfPermission(this, perstr) == PackageManager.PERMISSION_GRANTED)
			{
				TextView tv = (TextView) findViewById(permissions_tv_id[i]);
				tv.setText(" granted ");
				tv.setTextColor(Color.GREEN);
				++granted_count;
			}
		}

		temperatureUtil = new ThermalImageUtil(this);

		if(isInstalled(AuthActivity.this, "com.telpo.temperatureservice")){
			termo_camera_found = true;
			Log.i("TEMPERATURE", "TempatureServices.apk found");
		}else
			Log.e("TEMPERATURE", "not install TempatureServices.apk");

		if(granted_count < permissions_str.length)
		{
			ActivityCompat.requestPermissions(this, permissions_str, 0);
		}
		else
		{
			starting_openni();
		}
	}

	@Override
	public void onRequestPermissionsResult(
		int requestCode,
		String[] permissions,
		int[] grantResults)
	{
		boolean ask_again = false;

		for(int i = 0; i < permissions.length; ++i)
		{
			if(grantResults[i] == PackageManager.PERMISSION_GRANTED)
			{
				TextView tv = (TextView) findViewById(permissions_tv_id[i]);
				tv.setText(" granted ");
				tv.setTextColor(Color.GREEN);
			}
			else if(!permissions[i].equals(Manifest.permission.READ_PHONE_STATE))  // READ_PHONE_STATE is optional
			{
				ask_again = true;
			}
		}

		if(ask_again)
		{
			final Activity this_activity = this;
			new Timer().schedule(new TimerTask(){@Override public void run() {
				ActivityCompat.requestPermissions(this_activity, permissions_str, 0);
			}}, 2000);
		}
		else
		{
			starting_openni();
		}
	}

	// init openni
	// If it is not initialized we will try to initialize IR
	private void starting_openni()
	{
		// fixed camera settings
		SharedPreferences shared_settings = getSharedPreferences(SETTINGS_NAME, 0);
		SharedPreferences.Editor editor = shared_settings.edit();
		editor.putString("resolution", "640x480");

		try
		{
			// init openni
			{
				openni_helper = new OpenNIHelper(this);
				OpenNI.setLogAndroidOutput(true);
				OpenNI.setLogMinSeverity(0);
				OpenNI.initialize();

				List<DeviceInfo> devices = OpenNI.enumerateDevices();

				if(devices.size() == 1) {
					camera_type = "depth map";
					video_worker_config = "video_worker_fdatracker_telpo_depth_liveness.xml";
					base_angle = 0;
					editor.putInt("base_angle", base_angle);
					while(!editor.commit());
					openni_helper.requestDeviceOpen(devices.get(0).getUri(), this);
					depth_camera_found = true;
				}else {
					camera_type = "IR frame";
					video_worker_config = "video_worker_fdatracker_telpo_ir_liveness.xml";
					base_angle = 90;
					editor.putInt("base_angle", base_angle);
					while(!editor.commit());
					starting_ir();
					ir_camera_found = true;
				}

				if (!depth_camera_found && !ir_camera_found)
					throw new Exception("Depth or IR camera not found");
			}
		}
		catch(Exception e)
		{
			exceptionHappensDo(this, e);
		}
	}

	@Override
	public void onDeviceOpenFailed(String uri)
	{
		exceptionHappensDo(this, new Exception("Failed to open openni device " + uri));
	}

	@Override
	public void onDeviceOpened(Device aDevice)
	{
		depth_stream = VideoStream.create(aDevice, SensorType.DEPTH);

		VideoMode vm = depth_stream.getVideoMode();

		vm.setResolution(640, 400);

		depth_stream.setVideoMode(vm);

		aDevice.setImageRegistrationMode(ImageRegistrationMode.DEPTH_TO_COLOR);

		depth_stream.addNewFrameListener(this);

		depth_stream.start();

		starting();
	}


	// init RGB camera and face sdk objects
	// (this is called in onDeviceOpened - after openni init)
	// (This is also used at the end of the starting_ir method)
	private void starting()
	{
		try
		{
			setContentView(R.layout.auth);

			rgb_camera = new TheCamera(this, Camera.CameraInfo.CAMERA_FACING_BACK);

			DrawingSurfaceView dsv = (DrawingSurfaceView) findViewById(R.id.authSurfaceView);

			rgb_camera.open(
				this,
				(SurfaceView) dsv,
				findViewById(R.id.authSurfaceViewLL));

			float scale = dsv.getScaleX();
			dsv.image_width = rgb_camera.getWidth();

			if( base_angle == 90 ) {
				//Rotate drawing surface in case of preview rotation
				dsv.setRotation(base_angle);

				//Mirror in vertical direction
				dsv.setScaleY(-1f);
			}

			if( depth_camera_found ){
				if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.O){
					//Camera mirrored like a frontal camera
					dsv.setScaleX(-1f);
				}
			}


			// sometimes telpo camera do not work after first open..
			rgb_camera.close();
			rgb_camera.open(
				this,
				(SurfaceView) findViewById(R.id.authSurfaceView),
				findViewById(R.id.authSurfaceViewLL));

			facerec_service = FacerecService.createService(
				getApplicationInfo().nativeLibraryDir + "/libfacerec.so",  // = dll_path
				getApplicationInfo().dataDir + "/fsdk/conf/facerec",       // = conf_dir_path
				getApplicationInfo().dataDir + "/fsdk/license");           // = license_dir

			FacerecService.LicenseState license_state;
			try
			{
				license_state = facerec_service.getLicenseState();
			}
			catch(Exception e)
			{
				// just ignore any exception here
				//  this is workaround of rare error caused by incorret
				//  previous shutdown of the application
				Log.i(TAG, "workaround catch '" + e.getMessage() + "'");
				license_state = facerec_service.getLicenseState();
			}

			//repeat again
			Log.i(TAG, "license_state.online            = " + Boolean.toString(license_state.online));
			Log.i(TAG, "license_state.android_app_id    = " + license_state.android_app_id);
			Log.i(TAG, "license_state.ios_app_id        = " + license_state.ios_app_id);
			Log.i(TAG, "license_state.hardware_reg      = " + license_state.hardware_reg);

			setUiListeners();

			// creating VideoWorker will take some time, so create it in another thread
			final AuthActivity this_activity = this;
			vw_start_thread = new Thread(new Runnable(){public void run(){
				try
				{
					// recognizer to load existing templates and create new after registration
					this_activity.recognizer = facerec_service.createRecognizer(
						recognizer_config,
						true,    // = processing
						false,   // = matching
						false);  // = processing_less_memory_consumption

					recognition_distance_threshold = recognizer.getROCCurvePointByFAR(recognition_far_threshold).distance;

					// load database
					{
						int person_id = 0;
						this_activity.database_directory = getApplicationInfo().dataDir + "/database/";

						this_activity.database = new Vector<DBSample>();

						File db_dir = new File(this_activity.database_directory);

						if(db_dir.exists() && db_dir.isDirectory())
						{
							for(File person_dir : db_dir.listFiles())
							{
								if(!person_dir.isDirectory())
									continue;

								++person_id;

								String name = person_dir.getName();

								for(File file : person_dir.listFiles())
								{
									final FileInputStream file_stream = new FileInputStream(file);

									DBSample new_db_sample = new DBSample();

									new_db_sample.person_id = person_id;

									new_db_sample.name = name;

									RawSample sample = facerec_service.loadRawSample(file_stream);

									new_db_sample.thumbnail = makeSampleThumbnail(sample);

									new_db_sample.template = recognizer.loadTemplate(file_stream);

									this_activity.database.add(new_db_sample);

									file_stream.close();
								}
							}
						}

						this_activity.database_persons_count = person_id;
					}

					if(video_worker_config.isEmpty())
						throw new Exception("Config for VideoWorker not specified");

					// create video worker
					this_activity.video_worker =
						facerec_service.createVideoWorker(
							new VideoWorker.Params()
								.video_worker_config( facerec_service.new Config(video_worker_config) )
								.recognizer_ini_file(recognizer_config)
								.streams_count(1)
								.processing_threads_count(1)
								.matching_threads_count(1));

					this_activity.video_worker.addTrackingCallbackU(this_activity);

					this_activity.video_worker.addTrackingLostCallbackU(this_activity);

					this_activity.video_worker.addMatchFoundCallbackU(this_activity);

					this_activity.recognition_set.set(true);

					setAuthenticationMode();
				}
				catch(Exception e)
				{
					exceptionHappensDo(this_activity, e);
				}
			}});
			vw_start_thread.start();

		}
		catch(Exception e)
		{
			exceptionHappensDo(this, e);
		}
	}

	// init IR camera
	private void starting_ir()
	{
		try {
			setContentView(R.layout.auth);

			ir_camera = new TheCamera(this, Camera.CameraInfo.CAMERA_FACING_FRONT);

			ir_camera.open(
					this,
					null,
					null);

			// sometimes telpo camera do not work after first open..
			ir_camera.close();
			ir_camera.open(
					this,
					null,
					null);
		}
		catch(Exception e)
		{
			exceptionHappensDo(this, e);
		}

		starting();
	}

	@Override
	protected void onResume()
	{
		super.onResume();

		try
		{
			if(rgb_camera != null)
			{
				if(recognition_set.get())
				{
					video_worker.resetStream(0);

					synchronized(tracks_info)
					{
						tracks_info.clear();
					}

					resetMatchesImageView();
				}

				rgb_camera.open(
					this,
					(SurfaceView) findViewById(R.id.authSurfaceView),
					findViewById(R.id.authSurfaceViewLL));

				if (ir_camera_found)
				{
					ir_camera.open(
							this,
							null,
							null);
				}
			}

			if(depth_stream != null)
				depth_stream.start();
		}
		catch(Exception e)
		{
			exceptionHappensDo(this, e);
		}
	}


	@Override
	protected void onPause()
	{
		if(rgb_camera != null) rgb_camera.close();

		if(ir_camera != null) ir_camera.close();

		if(depth_stream != null) depth_stream.stop();

		super.onPause();
	}


	@Override
	protected void onDestroy()
	{
		if(depth_stream != null)
			depth_stream.destroy();

		if(openni_helper != null)
			openni_helper.shutdown();

		OpenNI.shutdown();

		if(vw_start_thread != null)
		{
			try
			{
				vw_start_thread.join();
			}
			catch(Exception e)
			{
				exceptionHappensDo(this, e);
			}
		}

		if(rgb_camera != null)
			rgb_camera.close();

		if(ir_camera != null)
			ir_camera.close();

		if(facerec_service != null)
			facerec_service.dispose();


		if(recognition_set.get())
		{
			recognizer.dispose();

			video_worker.dispose();
		}

		super.onDestroy();
	}


	// enable authentication mode - set layout, enable recognition
	private void setAuthenticationMode()
	{
		// not checking recognition_set, because setAuthenticationMode can be called only after

		Vector<VideoWorker.DatabaseElement> vwdb;

		synchronized(database)
		{
			vwdb = new Vector<VideoWorker.DatabaseElement>();

			for(int i = 0; i < database.size(); ++i)
			{
				DBSample db_sample = database.get(i);

				vwdb.add(new VideoWorker.DatabaseElement(
					i,  // = element_id
					db_sample.person_id,
					db_sample.template,
					(float) recognition_distance_threshold));
			}
		}

		final int database_size = vwdb.size();

		video_worker.setDatabase(vwdb, Recognizer.SearchAccelerationType.SEARCH_ACCELERATION_1);

		video_worker.enableProcessingOnStream(0);

		work_mode_registration.set(false);

		runOnUiThread(new Runnable(){public void run(){
			((TextView) findViewById(R.id.authDBStatus)).setText("Database size: " + database_size + ", persons count: " + database_persons_count);
			findViewById(R.id.authAuthenticationLL).setVisibility(View.VISIBLE);
			findViewById(R.id.authProgressLL).setVisibility(View.GONE);
			findViewById(R.id.authRegistrationLL).setVisibility(View.GONE);
		}});
	}


	// enable registration mode - set layout, disable recognition
	private void setRegistrationMode()
	{
		video_worker.disableProcessingOnStream(0);

		work_mode_registration.set(true);

		runOnUiThread(new Runnable(){public void run(){

			registration_current_sample = null;
			registration_current_sample_quality = 0;

			((TextView) findViewById(R.id.authFaceProblemText)).setText("");

			((ImageView) findViewById(R.id.authRegistrationImageView)).setImageBitmap(null);

			findViewById(R.id.authRegisterOkButton).setEnabled(false);
			findViewById(R.id.authRegisterResetButton).setEnabled(false);

			findViewById(R.id.authAuthenticationLL).setVisibility(View.GONE);
			findViewById(R.id.authProgressLL).setVisibility(View.GONE);
			findViewById(R.id.authRegistrationLL).setVisibility(View.VISIBLE);
		}});
	}


	// set listeners for every button
	private void setUiListeners()
	{
		final EditText name_text_edit = (EditText) findViewById(R.id.authRegistrationNameEditText);

		// register button must be enabled only with non-empty name
		name_text_edit.addTextChangedListener(
			new TextWatcher()
		{
			public void afterTextChanged(Editable s)
			{
				final String name = name_text_edit.getText().toString();

				registration_name_valid = !name.isEmpty();

				for(char c : name.toCharArray())
					if( (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') && c != ' ')
						registration_name_valid = false;


				findViewById( R.id.authRegisterOkButton ).setEnabled(
					registration_name_valid && registration_current_sample != null);

				String text;

				if(!registration_name_valid)
					text = "Invalid name '" + name + "'.";
				else
				{
					boolean found = false;

					synchronized(database)
					{
						for(DBSample sample : database)
							if(name.equals(sample.name))
								found = true;
					}

					if(found)
						text = "Register new sample for person '" + name + "'.";
					else
						text = "Register new person '" + name + "'";
				}

				((TextView) findViewById(R.id.authRegistrationStatus)).setText(text);
			}

			public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
			public void onTextChanged(CharSequence s, int start, int before, int count) {}
		});



		findViewById( R.id.authRegistrationModeButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			setRegistrationMode();
		}});

		findViewById( R.id.authRegisterOkButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			try
			{
				if(registration_current_sample == null)
					return;

				final String name = name_text_edit.getText().toString();

				if(name.isEmpty())
					return;

				final Template template = recognizer.processing(registration_current_sample);

				// create this person directory if necessary
				new File(database_directory + "/" + name).mkdirs();

				for(int i = 0; ; ++i)
				{
					File file = new File(database_directory + "/" + name + "/" + i);

					// we search for a free file name
					if(file.exists())
						continue;

					FileOutputStream file_stream = new FileOutputStream(file);

					registration_current_sample.downscaleToPreferredSize().save(
						file_stream,
						RawSample.ImageFormat.IMAGE_FORMAT_JPG);

					template.save(file_stream);

					file_stream.close();

					break;
				}


				DBSample new_db_sample = new DBSample();

				new_db_sample.name = name;

				new_db_sample.thumbnail = makeSampleThumbnail(registration_current_sample);

				new_db_sample.template = template;

				synchronized(database)
				{
					// determine person_id before add to existing database
					//   if there is sample with the same name, we must use its person_id
					//   otherwise we set unique person_id

					new_db_sample.person_id = -1;

					long max_person_id = -1;

					for(DBSample db_sample : database)
					{
						if(name.equals(db_sample.name))
						{
							// found person with the same name, use its id
							new_db_sample.person_id = db_sample.person_id;
							break;
						}

						if(db_sample.person_id > max_person_id)
							max_person_id = db_sample.person_id;
					}

					// there is no person with the same name, set unique id
					if(new_db_sample.person_id < 0)
					{
						new_db_sample.person_id = max_person_id + 1;
						++database_persons_count;
					}

					// add to the database
					database.add(new_db_sample);
				}

				registration_current_sample = null;

				setAuthenticationMode();
			}
			catch(Exception e)
			{
				exceptionHappensDo(AuthActivity.this, e);
			}
		}});

		findViewById( R.id.authRegisterResetButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			setRegistrationMode();
		}});

		findViewById( R.id.authRegisterCancelButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			setAuthenticationMode();
		}});

		findViewById( R.id.authQuitButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			finishAffinity();
		}});

		findViewById( R.id.authAboutButton )
			.setOnClickListener(new View.OnClickListener(){ public void onClick(View v)
		{
			startActivity(new Intent(AuthActivity.this, AboutActivity.class));
		}});
	}


	// in case of fatal error: start activity with error message
	public static void exceptionHappensDo(Activity activity, Exception e){
		Intent toErrorIntent = new Intent(activity, ErrorActivity.class);
		toErrorIntent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
		if (e instanceof SDKException){
			SDKException sdke = (SDKException) e;
			toErrorIntent.putExtra("error message", "code: " + String.format("0x%08X", sdke.code()) + "\n"+ sdke.getMessage());
		}else{
			toErrorIntent.putExtra("error message", e.getMessage());
		}
		e.printStackTrace();
		activity.startActivity(toErrorIntent);
		activity.finish();
	}


	// camera object will call this when new frame arrive
	@Override
	public void processingImage(byte[] data, int width, int height, int cam_id) throws Exception{
		if (cam_id == Camera.CameraInfo.CAMERA_FACING_BACK)
		{
			processRGBFrame(data, width, height);
		}else
		if (cam_id == Camera.CameraInfo.CAMERA_FACING_FRONT)
		{
			processIRFrame(data, width, height);
		}else throw new Exception("Unknown camera ID!");;
	}


	public void processRGBFrame(byte[] data, int width, int height)
	{
		if(width != 640 || height != 480)
			exceptionHappensDo(this, new Exception("bad image frame resolution " + width + "x" + height));

		if(recognition_set.get())
		{
			// do the job if we have videoworker

			final long timestamp = System.nanoTime();

			final RawImage image = new RawImage(
				width,
				height,
				RawImage.Format.FORMAT_YUV_NV21,
				data);

			video_worker.addVideoFrame(
				image,
				0,                  // = stream_id
				timestamp / 1000);  // = timestamp_microsec
		}
	}


	public void processIRFrame(byte[] data, int width, int height) throws Exception {
		if(width != 640 || height != 480)
			exceptionHappensDo(this, new Exception("bad IR frame resolution " + width + "x" + height));

		final ByteBuffer bb =  ByteBuffer.allocateDirect(data.length * Byte.SIZE);
		bb.put(data);

		if(!bb.isDirect())
			throw new Exception(camera_type + " buffer must be direct");

		if(recognition_set.get()) {
			// do the job if we have videoworker

			final long timestamp = System.nanoTime();

			IRFrameRaw ifr = new IRFrameRaw();
			ifr.ir_frame_rows = height;
			ifr.ir_frame_cols = width;
			ifr.ir_frame_2_image_offset_x = 0;
			ifr.ir_frame_2_image_offset_y = 0;
			ifr.ir_frame_2_image_scale_x = 1;
			ifr.ir_frame_2_image_scale_y = 1;
			ifr.ir_horizontal_fov = (float)1.0;
			ifr.ir_vertical_fov = (float)1.0;

			// access c-pointer of the direct buffer
			java.lang.reflect.Field address_field = java.nio.Buffer.class.getDeclaredField("address");
			address_field.setAccessible(true);

			ifr.ir_frame_data_ptr = address_field.getLong(bb);

			ifr.ir_data_stride_in_bytes = width * 2;

			video_worker.addIRFrame(
				ifr,                 // = IR_frame
				0,                  // = stream_id
				timestamp / 1000);  // = timestamp_microsec
		}
	}


	// depth frame is ready
	@Override
	public void onFrameReady(VideoStream stream)
	{
		try
		{
			final VideoFrameRef frame = stream.readFrame();

			final long timestamp = System.nanoTime();

			if(frame.getWidth() != 640 || frame.getHeight() != 400)
				throw new Exception("bad " + camera_type + " resolution " + frame.getWidth() + "x" + frame.getHeight());

			final ByteBuffer bb = frame.getData();

			if(!bb.isDirect())
				throw new Exception(camera_type + " buffer must be direct");

			// add depth info in VideoWorker
			if(recognition_set.get())
			{
				DepthMapRaw dmr = new DepthMapRaw();
				dmr.depth_map_rows             = frame.getHeight();
				dmr.depth_map_cols             = frame.getWidth();
				dmr.depth_map_2_image_offset_x = 0;
				dmr.depth_map_2_image_offset_y = 0;
				dmr.depth_map_2_image_scale_x  = 1;
				dmr.depth_map_2_image_scale_y  = 1;
				dmr.horizontal_fov             = (float) (stream.getHorizontalFieldOfView() * 180 / Math.PI);
				dmr.vertical_fov               = (float) (stream.getVerticalFieldOfView() * 180 / Math.PI);

				dmr.depth_data_stride_in_bytes = frame.getWidth() * 2;

				PixelFormat pf = frame.getVideoMode().getPixelFormat();

				if(pf == PixelFormat.DEPTH_1_MM)
					dmr.depth_unit_in_millimeters = 1;
				else if(pf == PixelFormat.DEPTH_100_UM)
					dmr.depth_unit_in_millimeters = 0.1f;
				else
					throw new Exception("unknown depth pixel format " + pf);

				// access c-pointer of the direct buffer
				java.lang.reflect.Field address_field = java.nio.Buffer.class.getDeclaredField("address");
				address_field.setAccessible(true);

				dmr.depth_data_ptr = address_field.getLong(bb);

				video_worker.addDepthFrame(
					dmr,                // = depth_frame
					0,                  // = stream_id
					timestamp / 1000);  // = timestamp_microsec
			}

		}
		catch(Exception e)
		{
			exceptionHappensDo(this, e);
		}
	}


	class TimeLevelText
	{
		long timestamp;
		int level;
		String text;
	}

	private ArrayDeque<TimeLevelText> problem_text_data = new ArrayDeque<TimeLevelText>();
	private int problem_text_data_track_id = -1;

	// callback when frame is processed by videoworker
	@Override
	public void call(final TrackingCallbackData data)
	{
		boolean good_one_face = false;

		// set face problem text
		TimeLevelText text = new TimeLevelText();
		text.timestamp = System.nanoTime();
		text.level = 0;

		if(data.samples.size() == 0)
			text.text = "no faces";
		else if(data.samples.size() > 1)
			text.text = "more than one face";
		else if(!data.samples_weak.get(0))
		{
			text.text = "good face";
			text.level = 100;
			good_one_face = true;

			if(!work_mode_registration.get())
				text.text += ", recognition in progress";
		}
		else if(data.samples_good_angles.get(0) == TrackingCallbackData.SampleCheckStatus.FAILED)
		{
			text.text = "bad face foreshortening";
			text.level = 1;
		}
		else if(data.samples_good_face_size.get(0) == TrackingCallbackData.SampleCheckStatus.FAILED)
		{
			text.text = "bad face size";
			text.level = 2;
		}
		else if(data.samples_detector_confirmed.get(0) == TrackingCallbackData.SampleCheckStatus.FAILED)
		{
			text.text = "bad face score";
			text.level = 3;
		}
		else if(data.samples_good_light_and_blur.get(0) == TrackingCallbackData.SampleCheckStatus.FAILED)
		{
			text.text = "bad face light or blur";
			text.level = 4;
		}
		else if(depth_camera_found &&
				(data.samples_depth_liveness_confirmed.get(0) != DepthLivenessEstimator.Liveness.REAL))
		{
			text.text = "good face, but liveness is not confirmed";
			text.level = 5;
		}
		else if(ir_camera_found &&
				(data.samples_ir_liveness_confirmed.get(0) != IRLivenessEstimator.Liveness.REAL))
		{
			boolean a = (data.samples_ir_liveness_confirmed.get(0) == IRLivenessEstimator.Liveness.NOT_COMPUTED);
			text.text = "good face, but liveness is not confirmed";
			text.level = 5;
		}
		else
		{
			text.text = "something bad";
			text.level = 0;
		}


		if(data.samples.size() == 1)
		{
			final int track_id = data.samples_track_id.get(0);

			boolean matched;

			if(work_mode_registration.get())
				matched = false;
			else
			{
				synchronized(tracks_info)
				{
					matched = getTrackInfo(track_id).matched;
				}
			}

			if(matched)
			{
				text.text = "identified";

				if(termo_camera_found) text.text += ". " + getTrackInfo(track_id).temperature_text;
			}
			else
			{
				if(track_id != problem_text_data_track_id)
				{
					problem_text_data_track_id = track_id;
					problem_text_data.clear();
				}

				// remove status older than 1 second
				while(problem_text_data.size() > 0 && problem_text_data.peekFirst().timestamp < text.timestamp - 1000000000)
					problem_text_data.removeFirst();

				problem_text_data.addLast(text);

				for(TimeLevelText tlt : problem_text_data)
					if(tlt.level > text.level)
						text = tlt;
			}
		}


		final String final_text = text.text;

		runOnUiThread(new Runnable(){public void run(){
			((TextView) findViewById(R.id.authFaceProblemText)).setText(final_text);
		}});





		if(good_one_face && work_mode_registration.get())
		{
			// registration mode job

			final RawSample sample = data.samples.get(0);
			final float quality = data.samples_quality.get(0);

			runOnUiThread(new Runnable(){public void run(){

				if(registration_current_sample != null && registration_current_sample_quality >= quality)
					return;

				registration_current_sample = sample;
				registration_current_sample_quality = quality;

				findViewById(R.id.authRegisterOkButton).setEnabled(registration_name_valid);
				findViewById(R.id.authRegisterResetButton).setEnabled(true);

				((ImageView) findViewById(R.id.authRegistrationImageView)).setImageBitmap(
					makeSampleThumbnail(sample));
			}});
		}


		// sample can be weak just because depth or ir liveness was not computed
		//   so we check subset of flags to determine if sample is really bad
		final Vector<Boolean> samples_bad = new Vector<Boolean>();

		for(int i = 0; i < data.samples.size(); ++i)
			samples_bad.add(false
				|| data.samples_good_face_size.get(i) != TrackingCallbackData.SampleCheckStatus.PASSED
				|| data.samples_detector_confirmed.get(i) != TrackingCallbackData.SampleCheckStatus.PASSED
				|| data.samples_good_light_and_blur.get(i) != TrackingCallbackData.SampleCheckStatus.PASSED
				);

		final Vector<Integer> sample_color = new Vector<Integer>();
		for(int i = 0; i < data.samples.size(); ++i)
			sample_color.add(0);

		if(!work_mode_registration.get())
		{
			// authentication mode job

			// set color for matched tracks
			synchronized(tracks_info)
			{
				for(int i = 0; i < data.samples.size(); ++i)
					if(getTrackInfo(data.samples_track_id.get(i)).matched)
						sample_color.set(i, 1);
			}

			runOnUiThread(new Runnable(){public void run()
			{
				final Vector<Bitmap> thumbnails = new Vector<Bitmap>();

				for(int i = 0; i < data.samples.size(); ++i)
					if(samples_bad.get(i))
						thumbnails.add(null);
					else
						thumbnails.add(makeSampleThumbnail(data.samples.get(i)));

				synchronized(tracks_info)
				{
					for(int i = 0; i < data.samples.size(); ++i)
					{
						if(thumbnails.get(i) == null)
							continue;

						getTrackInfo(data.samples_track_id.get(i)).video_sample_thumbnail = thumbnails.get(i);
					}
				}

				resetMatchesImageView();
			}});
		}


		// set cirlces to draw in any work mode
		DrawingSurfaceView dsv = (DrawingSurfaceView) findViewById(R.id.authSurfaceView);

		synchronized(dsv.circles_data)
		{
			if(rgb_camera == null)
				return;

			dsv.circles_data.clear();

			for(int i = 0; i < data.samples.size(); ++i)
			{
				if(samples_bad.get(i))
					continue;

				Vector<Point> points = data.samples.get(i).getLandmarks();

				Point center = new Point(0, 0);

				for(Point p : data.samples.get(i).getLandmarks())
				{
					center.x += p.x;
					center.y += p.y;
				}

				center.x /= points.size();
				center.y /= points.size();

				float radius = 0;

				for(Point p : points)
					radius += Math.sqrt(
						(p.x - center.x) * (p.x - center.x) +
						(p.y - center.y) * (p.y - center.y) );

				radius /= points.size();
				radius *= 2.1f;


				center.x = center.x;
				center.y = center.y;

				dsv.circles_data.add(center.x);
				dsv.circles_data.add(center.y);
				dsv.circles_data.add(radius);
				dsv.circles_data.add((float) sample_color.get(i));
			}
		}

		dsv.postInvalidate();
	}

	// callback when face is recognized by videoworker
	@Override
	public void call(MatchFoundCallbackData data)
	{
		final int track_id = data.sample.getID();
		final long element_id = data.search_result.get(0).element_id;

		runOnUiThread(new Runnable(){public void run()
		{
			String name;
			Bitmap db_sample_thumbnail;

			synchronized(database)
			{
				// database only grow, so there is no way element_id exceed vector size
				DBSample db_sample = database.get((int) element_id);

				name = db_sample.name;
				db_sample_thumbnail = db_sample.thumbnail;
			}

			synchronized(tracks_info)
			{
				final TrackInfo track_info = getTrackInfo(track_id);
				track_info.matched = true;
				track_info.name = name;
				track_info.db_sample_thumbnail = db_sample_thumbnail;

				if (termo_camera_found)
				{
					final float distance = 50;

					new Thread(new Runnable() {
						@Override
						public void run()
						{
							TemperatureData temperatureData = temperatureUtil.getDataAndBitmap(distance, true, new HotImageCallback.Stub() {
								@Override
								public void onTemperatureFail(String e)
								{
									track_info.temperature_text = "Temperature failed: " + e;
									Log.i("TEMPERATURE", e);
								}

								@Override
								public void getTemperatureBimapData(final TemperatureBitmapData data)
								{
									runOnUiThread(new Runnable() {
										@Override
										public void run() {
											// final Bitmap b = data.getBitmap();
											// ((ImageView) findViewById(R.id.Temperature)).setImageBitmap(b);
										}
									});
								}

							});

							if (temperatureData != null)
							{
								String text = "";
								if(temperatureData.isUnusualTem()) {
									text = "Temperature anormaly";
								}else {
									text = "Temperature normal";
								}
								text += ": " + temperatureData.getTemperature() + " ℃";
								track_info.temperature_text = text;
							}
						}
					}).start();
				}
			}

			resetMatchesImageView();
		}});
	}


	// callback when face is lost
	@Override
	public void call(TrackingLostCallbackData data)
	{
		final int track_id = data.track_id;

		runOnUiThread(new Runnable(){public void run()
		{
			synchronized(tracks_info)
			{
				tracks_info.remove(track_id);
			}

			resetMatchesImageView();
		}});
	}

	private void resetMatchesImageView()
	{
		final int[] video_sample_image_view = {
			R.id.authVideoSample1,
			R.id.authVideoSample2,
			R.id.authVideoSample3,
			R.id.authVideoSample4,
		};

		final int[] db_sample_image_view = {
			R.id.authDBSample1,
			R.id.authDBSample2,
			R.id.authDBSample3,
			R.id.authDBSample4,
		};

		final int[] db_sample_name_view = {
			R.id.authDBSample1Name,
			R.id.authDBSample2Name,
			R.id.authDBSample3Name,
			R.id.authDBSample4Name,
		};

		final int count = 4;

		int i = 0;

		synchronized(tracks_info)
		{
			final Iterator<Map.Entry<Integer, TrackInfo>> tracks_iterator = tracks_info.entrySet().iterator();

			for(; i < count && tracks_iterator.hasNext(); ++i)
			{
				final TrackInfo track_info = tracks_iterator.next().getValue();

				((ImageView) findViewById(video_sample_image_view[i]) ).setImageBitmap(track_info.video_sample_thumbnail);
				((ImageView) findViewById(db_sample_image_view[i]) ).setImageBitmap(track_info.db_sample_thumbnail);
				((TextView) findViewById(db_sample_name_view[i]) ).setText(track_info.name);
				findViewById(db_sample_name_view[i]).setVisibility(track_info.matched ? View.VISIBLE : View.INVISIBLE);

				// this can be if sample was always bad
				if(track_info.video_sample_thumbnail == null && track_info.db_sample_thumbnail == null)
					--i;
			}
		}

		for(; i < count; ++i)
		{
			((ImageView) findViewById(video_sample_image_view[i]) ).setImageBitmap(null);
			((ImageView) findViewById(db_sample_image_view[i]) ).setImageBitmap(null);
			findViewById(db_sample_name_view[i]).setVisibility(View.INVISIBLE);
		}
	}


    private boolean isInstalled(Context context, String packageName) {
        final PackageManager packageManager = context.getPackageManager();
        List<PackageInfo> pinfo = packageManager.getInstalledPackages(0);
        for (int i = 0; i < pinfo.size(); i++) {
            if (pinfo.get(i).packageName.equalsIgnoreCase(packageName))
                return true;
        }
        return false;
    }

}
