package com.vdt.face_recognition.demo;

import java.io.ByteArrayOutputStream;
import java.io.OutputStream;
import java.lang.Math;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Vector;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.AgeGenderEstimator;
import com.vdt.face_recognition.sdk.Capturer;
import com.vdt.face_recognition.sdk.EmotionsEstimator;
import com.vdt.face_recognition.sdk.FaceQualityEstimator;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.LivenessEstimator;
import com.vdt.face_recognition.sdk.Point;
import com.vdt.face_recognition.sdk.QualityEstimator;
import com.vdt.face_recognition.sdk.RawImage;
import com.vdt.face_recognition.sdk.RawSample;
import com.vdt.face_recognition.sdk.SDKException;


public class Demo{

	private static String TAG = "Demo";

	private Activity activity;
	private TextView textView = null;

	private FacerecService service = null;
	private Capturer capturer = null;
	private QualityEstimator qualityEstimator = null;
	private AgeGenderEstimator ageGenderEstimator = null;
	private EmotionsEstimator emotionsEstimator = null;
	private FaceQualityEstimator faceQualityEstimator = null;

	private boolean flag_rectangle = true;
	private boolean flag_angles = true;
	private boolean flag_quality = false;
	private boolean flag_liveness = false;
	private boolean flag_age_and_gender = false;
	private boolean flag_points = true;
	private boolean flag_face_quality = false;
	private boolean flag_angles_vectors = true;
	private boolean flag_emotions = false;

	private HashMap<Integer, LivenessEstimator> id2le = new HashMap<Integer, LivenessEstimator>();
	private RawSample.FaceCutType faceCutType = null;


	public Demo(Activity activity, FacerecService service){
		this.activity = activity;

		this.service = service;

		FacerecService.Config capturer_conf = service.new Config("fda_tracker_capturer_mesh.xml");
		capturer_conf.overrideParameter("downscale_rawsamples_to_preferred_size", 0);
		capturer = service.createCapturer(capturer_conf);
		qualityEstimator = service.createQualityEstimator("quality_estimator_iso.xml");
		ageGenderEstimator = service.createAgeGenderEstimator("age_gender_estimator.xml");
		emotionsEstimator = service.createEmotionsEstimator("emotions_estimator.xml");
		faceQualityEstimator = service.createFaceQualityEstimator("face_quality_estimator.xml");
	}


	public void updateCapturer(){
		// force free resources
		// otherwise licence error may occur
		// when create sdk object in next time 
		if (capturer != null) {
			capturer.dispose();
		}

		FacerecService.Config capturer_conf = service.new Config("fda_tracker_capturer_mesh.xml");
		capturer_conf.overrideParameter("downscale_rawsamples_to_preferred_size", 0);
		capturer = service.createCapturer(capturer_conf);
	}


	public void setTextView(){
		textView = (TextView) activity.findViewById(R.id.textView);
	}

	private void setWeight(int id, float weight)
	{
		View view = activity.findViewById(id);
		LinearLayout.LayoutParams p = (LinearLayout.LayoutParams) view.getLayoutParams();
		p.weight = weight;
		view.setLayoutParams(p);
	}


	public void processingImage(Canvas canvas, byte[] data, int width, int height){

		Paint paint = new Paint();
		paint.setColor(0xffff0000);
		paint.setStrokeWidth(3);
		paint.setStyle(Paint.Style.STROKE);

		String text = "";

		RawImage rawImage = new RawImage(
			width,
			height,
			RawImage.Format.FORMAT_YUV_NV21,
			data);
		Vector<RawSample> samples = capturer.capture(rawImage);

		if(samples.isEmpty())
			return;

		//output info for one person
		RawSample sample = samples.get(0);

		//face rectangle
		if (flag_rectangle){
			RawSample.Rectangle rect = sample.getRectangle();
			canvas.drawRect(rect.x, rect.y, rect.x+rect.width, rect.y+rect.height, paint);
		}

		//head angles
		if (flag_angles){
			RawSample.Angles angles = sample.getAngles();
			text += "Angles: \n\tyaw:\t" + angles.yaw +
								"\n\tpitch:\t" + angles.pitch +
								"\n\troll:\t" + angles.roll + 
								"\n";
		}

		//quality
		if (flag_quality){
			QualityEstimator.Quality quality = qualityEstimator.estimateQuality(sample);
			text += "Quality: \n\tlighting:\t" + quality.lighting +
								"\n\tnoise:\t" + quality.noise + 
								"\n\tsharpness:\t" + quality.sharpness + 
								"\n\tflare:\t" + quality.flare + 
								"\n";
		}

		//liveness
		if (flag_liveness)
		{
			// here we get/create the liveness estimator that work with this face
			final int id = sample.getID();
			if(!id2le.containsKey(id)){
				id2le.put(id, service.createLivenessEstimator());
			}
			final LivenessEstimator le = id2le.get(id);

			// add information to the estimator
			le.addSample(sample);

			// get liveness
			final LivenessEstimator.Liveness liveness = le.estimateLiveness();
			text += "Liveness: " + liveness.name() + "\n";
		}

		//age and gender
		if (flag_age_and_gender){
			AgeGenderEstimator.AgeGender ageGender = ageGenderEstimator.estimateAgeGender(sample);

			text += "Age: " + (int)(ageGender.age_years + 0.5) + " years - ";
			switch(ageGender.age)
			{
				case AGE_KID:    text += "kid\n";    break;
				case AGE_YOUNG:  text += "young\n";  break;
				case AGE_ADULT:  text += "adult\n";  break;
				case AGE_SENIOR: text += "senior\n"; break;
			}

			switch(ageGender.gender)
			{
				case GENDER_FEMALE: text += "Gender: female\n"; break;
				case GENDER_MALE:   text += "Gender: male\n";   break;
			}
		}

		//crops
		if (faceCutType != null){
			Paint bitmap_paint = new Paint(Paint.ANTI_ALIAS_FLAG);
			Paint cut_border_paint = new Paint();
			cut_border_paint.setColor(0xffff5555);
			cut_border_paint.setStrokeWidth(3);
			cut_border_paint.setStyle(Paint.Style.STROKE);

			OutputStream os = new ByteArrayOutputStream();
			sample.cutFaceImage(os, RawSample.ImageFormat.IMAGE_FORMAT_JPG, faceCutType);
			byte [] byte_crop = ((ByteArrayOutputStream) os).toByteArray();
			Bitmap bitmap_crop = BitmapFactory.decodeByteArray(byte_crop, 0, byte_crop.length);
			Rect srcRect = new Rect(0, 0, bitmap_crop.getWidth(), bitmap_crop.getHeight());

			double divider = 4;
			double dstWidth = width / divider;
			double k = dstWidth / bitmap_crop.getWidth();
			double dstHeight = bitmap_crop.getHeight() * k;

			Rect dstRect = new Rect(0, 0, (int) dstWidth, (int) dstHeight);
			canvas.drawBitmap(bitmap_crop, srcRect, dstRect, bitmap_paint);
			canvas.drawRect(dstRect, cut_border_paint);
		}

		//points
		//all points - red
		//left eye - green
		//right eye - yellow
		if (flag_points){
			Vector<Point> points = sample.getLandmarks();
			Point leftEye = sample.getLeftEye();
			Point rightEye = sample.getRightEye();
			paint.setStrokeWidth(3);

			for(Point point : points){
				canvas.drawCircle(point.x, point.y, 1, paint);
			}
			paint.setStrokeWidth(2);
			paint.setColor(0xff00ff00);
			canvas.drawCircle(leftEye.x, leftEye.y, 3, paint);
			paint.setColor(0xffffff00);
			canvas.drawCircle(rightEye.x, rightEye.y, 3, paint);
		}

		//face quality
		if (flag_face_quality){
			float faceQuality = faceQualityEstimator.estimateQuality(sample);
			text += "Face quality: " + faceQuality + "\n";
		}

		//angles vectors
		if (flag_angles_vectors){
			Point lEye = sample.getLeftEye();
			Point rEye = sample.getRightEye();
			RawSample.Angles head_angles = sample.getAngles();
			RawSample.Rectangle face_rect = sample.getRectangle();

			Point center = new Point((lEye.x + rEye.x)/2, (lEye.y + rEye.y)/2);
			float length = (face_rect.width + face_rect.height) * 0.3f;

			final float cos_yaw   = (float) Math.cos( head_angles.yaw   * Math.PI / 180 );
			final float sin_yaw   = (float) Math.sin( head_angles.yaw   * Math.PI / 180 );

			final float cos_pitch = (float) Math.cos( head_angles.pitch * Math.PI / 180 );
			final float sin_pitch = (float) Math.sin( head_angles.pitch * Math.PI / 180 );

			final float cos_roll  = (float) Math.cos( head_angles.roll  * Math.PI / 180 );
			final float sin_roll  = (float) Math.sin( head_angles.roll  * Math.PI / 180 );


			final Point [] xyz = {
				new Point(
					cos_yaw * cos_roll - sin_yaw * sin_pitch * sin_roll,
					sin_yaw * sin_pitch * cos_roll + cos_yaw * sin_roll),

				new Point(
					cos_pitch * sin_roll,
					-cos_pitch * cos_roll),

				new Point(
					sin_yaw * cos_roll + cos_yaw * sin_pitch * sin_roll,
					-cos_yaw * sin_pitch * cos_roll + sin_yaw * sin_roll),
			};


			paint.setStrokeWidth(3);
			paint.setColor(0xffffff00);
			canvas.drawLine(center.x, center.y, center.x + xyz[0].x * length, center.y + xyz[0].y * length, paint);
			paint.setColor(0xff00ff00);
			canvas.drawLine(center.x, center.y, center.x + xyz[1].x * length, center.y + xyz[1].y * length, paint);
			paint.setColor(0xffff0000);
			canvas.drawLine(center.x, center.y, center.x + xyz[2].x * length, center.y + xyz[2].y * length, paint);
		}

		//emotions
		if (flag_emotions)
		{
			Vector<EmotionsEstimator.EmotionConfidence> emotionsConf = emotionsEstimator.estimateEmotions(sample);

			activity.findViewById(R.id.emotions).setVisibility(View.VISIBLE);

			for(EmotionsEstimator.EmotionConfidence ec : emotionsConf)
			{
				switch(ec.emotion)
				{
					case EMOTION_NEUTRAL:
						setWeight(R.id.neutral1, ec.confidence);
						setWeight(R.id.neutral2, 1 - ec.confidence);
						break;
					case EMOTION_HAPPY:
						setWeight(R.id.happy1, ec.confidence);
						setWeight(R.id.happy2, 1 - ec.confidence);
						break;
					case EMOTION_ANGRY:
						setWeight(R.id.angry1, ec.confidence);
						setWeight(R.id.angry2, 1 - ec.confidence);
						break;
					case EMOTION_SURPRISE:
						setWeight(R.id.surprise1, ec.confidence);
						setWeight(R.id.surprise2, 1 - ec.confidence);
						break;
				}
			}
		}
		else
		{
			activity.findViewById(R.id.emotions).setVisibility(View.GONE);
		}

		textView.setText(text);
	}


	public void setOptions(boolean [] flags, int faceCutTypeId){

		if (flags != null){
			flag_rectangle = flags[0];
			flag_angles = flags[1];
			flag_quality = flags[2];
			flag_liveness = flags[3];
			flag_age_and_gender = flags[4];
			flag_points = flags[5];
			flag_face_quality = flags[6];
			flag_angles_vectors = flags[7];
			flag_emotions = flags[8];
		}

		faceCutType = (faceCutTypeId >0) ? RawSample.FaceCutType.values()[faceCutTypeId-1] : null;

	}


	public boolean [] getFlags(){
		boolean [] flags = {
			flag_rectangle,
			flag_angles,
			flag_quality,
			flag_liveness,
			flag_age_and_gender,
			flag_points,
			flag_face_quality,
			flag_angles_vectors,
			flag_emotions};

		return flags;
	}


	public int getFaceCutTypeId(){
		return (faceCutType != null) ? faceCutType.ordinal() : 0;
	}

	public void dispose()
	{
		service.dispose();
		capturer.dispose();
		qualityEstimator.dispose();
		ageGenderEstimator.dispose();
		emotionsEstimator.dispose();
		faceQualityEstimator.dispose();
	}

}