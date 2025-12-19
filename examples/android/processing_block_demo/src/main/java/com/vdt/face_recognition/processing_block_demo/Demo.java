package com.vdt.face_recognition.processing_block_demo;

import java.lang.Math;
import java.util.Objects;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.LinearLayout;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;


public class Demo{

	private final String TAG = "Demo";

	private final Activity activity;
	private TextView textView = null;

	private FacerecService service = null;
	private ProcessingBlock faceDetector = null;
	private ProcessingBlock faceFitter = null;
	private ProcessingBlock qualityEstimator = null;
	private ProcessingBlock ageEstimator = null;
	private ProcessingBlock genderEstimator = null;
	private ProcessingBlock emotionsEstimator = null;
	private ProcessingBlock liveness2dEstimator = null;
	private ProcessingBlock faceMaskEstimator = null;
	private ProcessingBlock eyesOpennessEstimator = null;

	private boolean flag_rectangle = true;
	private boolean flag_angles = true;
	private boolean flag_quality = false;
	private boolean flag_liveness = false;
	private boolean flag_age_and_gender = false;
	private boolean flag_points = true;
	private boolean flag_angles_vectors = true;
	private boolean flag_emotions = false;
	private boolean flag_face_mask = false;
	private boolean flag_eyes_openness = false;

	private String faceCutType = null;


	public Demo(Activity activity, FacerecService service){
		this.activity = activity;

		this.service = service;

		Context detectorConfigCtx = service.createContext();
		detectorConfigCtx.get("unit_type").setString("FACE_DETECTOR");
		detectorConfigCtx.get("modification").setString("blf_front");
		faceDetector = service.createProcessingBlock(detectorConfigCtx);

		Context fitterConfigCtx = service.createContext();
		fitterConfigCtx.get("unit_type").setString("FACE_FITTER");
		fitterConfigCtx.get("modification").setString("fda");
		faceFitter = service.createProcessingBlock(fitterConfigCtx);
		
		Context configCtxQE = service.createContext();
		configCtxQE.get("unit_type").setString("QUALITY_CONTROL");
		configCtxQE.get("version").setLong(1);
		configCtxQE.get("modification").setString("estimation");
		qualityEstimator = service.createProcessingBlock(configCtxQE);

		Context configCtxAge = service.createContext();
		configCtxAge.get("unit_type").setString("AGE_ESTIMATOR");
		configCtxAge.get("version").setLong(2);
		configCtxAge.get("modification").setString("light");
		ageEstimator = service.createProcessingBlock(configCtxAge);

		Context configCtxGender = service.createContext();
		configCtxGender.get("unit_type").setString("GENDER_ESTIMATOR");
		configCtxGender.get("modification").setString("light");
		configCtxGender.get("version").setLong(2);
		genderEstimator = service.createProcessingBlock(configCtxGender);

		Context configCtxEmotions = service.createContext();
		configCtxEmotions.get("unit_type").setString("EMOTION_ESTIMATOR");
		configCtxEmotions.get("modification").setString("heavy");
		configCtxEmotions.get("version").setLong(1);
		emotionsEstimator = service.createProcessingBlock(configCtxEmotions);

		Context configCtxMask = service.createContext();
		configCtxMask.get("unit_type").setString("MASK_ESTIMATOR");
		configCtxMask.get("modification").setString("light");
		configCtxMask.get("version").setLong(2);
		faceMaskEstimator = service.createProcessingBlock(configCtxMask);

		Context configCtxEyesOpen = service.createContext();
		configCtxEyesOpen.get("unit_type").setString("EYE_OPENNESS_ESTIMATOR");
		configCtxEyesOpen.get("modification").setString("light");
		configCtxEyesOpen.get("version").setLong(1);
		eyesOpennessEstimator = service.createProcessingBlock(configCtxEyesOpen);
	}


	public void updateFaceDetector(){
		// force free resources
		// otherwise licence error may occur
		// when create sdk object in next time
		if (faceDetector != null) {
			faceDetector.dispose();
		}

		if (faceFitter != null) {
			faceFitter.dispose();
		}
		
		Context detectorConfigCtx = service.createContext();
		detectorConfigCtx.get("unit_type").setString("FACE_DETECTOR");
		detectorConfigCtx.get("modification").setString("blf_front");
		faceDetector = service.createProcessingBlock(detectorConfigCtx);
		
		Context fitterConfigCtx = service.createContext();
		fitterConfigCtx.get("unit_type").setString("FACE_FITTER");
		fitterConfigCtx.get("modification").setString("fda");
		faceFitter = service.createProcessingBlock(fitterConfigCtx);
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


		Context ioData = service.createContextFromFrame(data, width, height,
            Context.Format.FORMAT_NV21, 0);

		faceDetector.process(ioData);
		if(ioData.get("objects").size() != 1)
			return;

		
		faceFitter.process(ioData);

		//output info for one person
		Context object = ioData.get("objects").get(0);

		//face rectangle
		if (flag_rectangle){
			Context bbox = object.get("bbox");
			canvas.drawRect((float)bbox.get(0).getDouble() * width,
			                (float)bbox.get(1).getDouble() * height,
							(float)bbox.get(2).getDouble() * width,
							(float)bbox.get(3).getDouble() * height,
							paint);
		}

		//head angles
		if (flag_angles)
		{
			Context angles = object.get("pose");
			text += "Angles: \n\tyaw:\t" + angles.get("yaw").getDouble() +
								"\n\tpitch:\t" + angles.get("pitch").getDouble() +
								"\n\troll:\t" + angles.get("roll").getDouble() +
								"\n";
		}

		//quality
		if (flag_quality)
		{
			qualityEstimator.process(ioData);
			boolean passed = ioData.get("objects").get(0).get("quality").get("value").getBool();

			text += "Quality: " + passed + "\n";
		}

		//liveness
		if (flag_liveness)
		{
			Context singleObjData = service.createContext();
			singleObjData.get("image").setContext(ioData.get("image"));
			Context newObjCtx = service.createContext();
			newObjCtx.pushBack(ioData.get("objects").get(0));
			singleObjData.get("objects").setContext(newObjCtx);
			
			if(liveness2dEstimator == null){
				Context configCtx = service.createContext();
				
				configCtx.get("unit_type").setString("LIVENESS_ESTIMATOR");
				configCtx.get("modification").setString("2d_light");
				configCtx.get("version").setLong(4);

				liveness2dEstimator = service.createProcessingBlock(configCtx);

				if (liveness2dEstimator == null) {
					Log.e(TAG, "Failed to create liveness2dEstimator");
				}
			}
			liveness2dEstimator.process(singleObjData);
			Context liveness_results = singleObjData.get("objects").get(0).get("liveness");
			String liveness_value = liveness_results.get("value").getString();
			double liveness_confidence = liveness_results.get("confidence").getDouble();

			@SuppressLint("DefaultLocale") String score_str = (!Objects.equals(liveness_value, "NOT_ENOUGH_DATA")) ?
				String.format("%.03f", liveness_confidence) : "";
			text += "Liveness: " + score_str + " - "+ liveness_value + "\n";			
		}

		//age and gender
		if (flag_age_and_gender){
			ageEstimator.process(ioData);
			genderEstimator.process(ioData);
			long age = ioData.get("objects").get(0).get("age").getLong(); //+ 0.5
			String gender = ioData.get("objects").get(0).get("gender").getString();

			text += "Age: " + (int)(age) + " years - ";
			if (age < 18) {
				text += "kid\n";
			} else if (age < 37) {
				text += "young\n";
			} else if (age < 55) {
				text += "adult\n";
			} else {
				text += "senior\n";
			}

			switch(gender)
			{
				case "FEMALE": text += "Gender: female\n"; break;
				case "MALE":   text += "Gender: male\n";   break;
			}
		}

		// face attribute (masked_face)
		if (flag_face_mask)
		{
			faceMaskEstimator.process(ioData);
			Context objectMaskCtx = ioData.get("objects").get(0).get("has_medical_mask");
			double confidence = objectMaskCtx.get("confidence").getDouble();
			boolean value = objectMaskCtx.get("value").getBool();
            @SuppressLint("DefaultLocale") String score_str = String.format("%.03f", confidence);
			text += "masked: " + (value ? "true" : "false") + " - " + score_str + "\n";
		}

		// face attribute (eyes_openness)
		if (flag_eyes_openness)
		{
			eyesOpennessEstimator.process(ioData);
			Context sample = ioData.get("objects").get(0);
			Context left_eye_ctx = sample.get("is_left_eye_open");
			double eye_left_score = left_eye_ctx.get("confidence").getDouble();
			@SuppressLint("DefaultLocale") String eye_left_score_str = String.format("%.03f", eye_left_score);

			Context right_eye_ctx = sample.get("is_right_eye_open");
			double eye_right_score = right_eye_ctx.get("confidence").getDouble();
			@SuppressLint("DefaultLocale") String eye_right_score_str = String.format("%.03f", eye_left_score);

			String left_verdict_str = "not_computed";
			String right_verdict_str = "not_computed";

			if (eye_left_score != -1)
				left_verdict_str = (left_eye_ctx.get("value").getBool() ? "true" : "false");

			if (eye_right_score != -1)
				right_verdict_str = (right_eye_ctx.get("value").getBool() ? "true" : "false");

			text += "Eyes openness \n";
			text += "  left eye opened: \n     " + left_verdict_str + "  " + eye_left_score_str + "\n";
			text += "  right eye opened: \n     " + right_verdict_str + "  " + eye_right_score_str + "\n";
		}

		 if (faceCutType != null)
		 {
			 Paint bitmap_paint = new Paint(Paint.ANTI_ALIAS_FLAG);
			 Paint cut_border_paint = new Paint();
			 cut_border_paint.setColor(0xffff5555);
			 cut_border_paint.setStrokeWidth(3);
			 cut_border_paint.setStyle(Paint.Style.STROKE);

			 Context faceCutConfig = service.createContext();
			 faceCutConfig.get("unit_type").setString("UTILITY_MODULE");
			 faceCutConfig.get("modification").setString("face_cut");
			 faceCutConfig.get("cut_type").setString(faceCutType);

			 ProcessingBlock faceCutBlock = service.createProcessingBlock(faceCutConfig);
			 faceCutBlock.process(ioData);

			 Context sample = ioData.get("objects").get(0);
			 Context faceCropCtx = sample.get("face_crop");
			 Context cropShapes = faceCropCtx.get("shape");
			 final long cropHeight = cropShapes.get(0).getLong();
			 final long cropWidth = cropShapes.get(1).getLong();
			 final long cropCannel = cropShapes.get(2).getLong();
			 final long cropSizeInBytes  = cropHeight * cropWidth * cropCannel;
			 byte[] cropData = faceCropCtx.get("blob").getBytes((int)cropSizeInBytes);

			 int[] pixels = new int[Math.toIntExact(cropWidth * cropHeight)];
			 int idx = 0;
			 for (int i = 0; i < pixels.length; i++) {
				 int r = cropData[idx] & 0xFF;
				 int g = cropData[idx + 1] & 0xFF;
				 int b = cropData[idx + 2] & 0xFF;
				 idx += 3;

				 pixels[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
			 }

			 Bitmap bitmap_crop = Bitmap.createBitmap(pixels, (int) cropWidth, (int) cropHeight, Bitmap.Config.ARGB_8888);
			 Rect srcRect = new Rect(0, 0, bitmap_crop.getWidth(), bitmap_crop.getHeight());

			 double divider = 4;
			 double dstWidth = width / divider;
			 double k = dstWidth / bitmap_crop.getWidth();
			 double dstHeight = bitmap_crop.getHeight() * k;

			 Rect dstRect = new Rect(0, 0, (int) dstWidth, (int) dstHeight);
			 canvas.drawBitmap(bitmap_crop, srcRect, dstRect, bitmap_paint);
			 canvas.drawRect(dstRect, cut_border_paint);

			 faceCutBlock.dispose();
			 faceCutConfig.dispose();
		 }

		//points
		//all points - red
		//left eye - green
		//right eye - yellow
		if (flag_points){
			Context points = object.get("keypoints");
			paint.setStrokeWidth(3);

			Context pts = points.get("points");

			for (int i = 0; i < pts.size(); i++) {

				Context point = pts.get(i);
				canvas.drawCircle((float)point.get("proj").get(0).getDouble() * width,
								  (float)point.get("proj").get(1).getDouble() * height,
								  1,
								  paint);
			}

			// Context lEye = points.get("left_eye");
			// Context rEye = points.get("right_eye");
			// paint.setStrokeWidth(2);
			// paint.setColor(0xff00ff00);
			// canvas.drawCircle((float)lEye.get("proj").get(0).getDouble(),
			// 				  (float)lEye.get("proj").get(1).getDouble(),
			// 				  3,
			// 				  paint);
			// paint.setColor(0xffffff00);
			// canvas.drawCircle((float)rEye.get("proj").get(0).getDouble(),
			// 				  (float)rEye.get("proj").get(1).getDouble(),
			// 				  3,
			// 				  paint);
		}

		//angles vectors
		if (flag_angles_vectors){
			Context points = object.get("keypoints");
			Context lEye = points.get("left_eye");
			Context rEye = points.get("right_eye");
			Context head_angles = object.get("pose");
			Context face_rect = object.get("bbox");

			float length = ((float)((face_rect.get(2).getDouble() - face_rect.get(0).getDouble()) * width) +
							(float)((face_rect.get(3).getDouble() - face_rect.get(1).getDouble())) * height) * 0.3f;

			final float cos_yaw   = (float) Math.cos( head_angles.get("yaw").getDouble()   * Math.PI / 180 );
			final float sin_yaw   = (float) Math.sin( head_angles.get("yaw").getDouble()   * Math.PI / 180 );

			final float cos_pitch = (float) Math.cos( head_angles.get("pitch").getDouble() * Math.PI / 180 );
			final float sin_pitch = (float) Math.sin( head_angles.get("pitch").getDouble() * Math.PI / 180 );

			final float cos_roll  = (float) Math.cos( head_angles.get("roll").getDouble()  * Math.PI / 180 );
			final float sin_roll  = (float) Math.sin( head_angles.get("roll").getDouble()  * Math.PI / 180 );


			float centerX = (float)((lEye.get("proj").get(0).getDouble() + rEye.get("proj").get(0).getDouble())/2 * width);
			float centerY = (float)((lEye.get("proj").get(1).getDouble() + rEye.get("proj").get(1).getDouble())/2) * height;

			float XPoint_x = cos_yaw * cos_roll - sin_yaw * sin_pitch * sin_roll;
			float XPoint_y = sin_yaw * sin_pitch * cos_roll + cos_yaw * sin_roll;

			float YPoint_x = cos_pitch * sin_roll;
			float YPoint_y = -cos_pitch * cos_roll;

			float ZPoint_x = sin_yaw * cos_roll + cos_yaw * sin_pitch * sin_roll;
			float ZPoint_y = -cos_yaw * sin_pitch * cos_roll + sin_yaw * sin_roll;

			paint.setStrokeWidth(3);
			paint.setColor(0xffffff00);
			canvas.drawLine(centerX, centerY, centerX + XPoint_x * length, centerY + XPoint_y * length, paint);
			paint.setColor(0xff00ff00);
			canvas.drawLine(centerX, centerY, centerX + YPoint_x  * length, centerY + YPoint_y * length, paint);
			paint.setColor(0xffff0000);
			canvas.drawLine(centerX, centerY, centerX + ZPoint_x * length, centerY + ZPoint_y * length, paint);
		}

		//emotions
		if (flag_emotions)
		{
			emotionsEstimator.process(ioData);
			Context sample = ioData.get("objects").get(0);
			activity.findViewById(R.id.emotions).setVisibility(View.VISIBLE);

			Context emotions = sample.get("emotions");
			for (int i = 0; i < emotions.size(); i++)
			{
				Context ec = emotions.get(i);
				float conf = (float)ec.get("confidence").getDouble();
				switch(ec.get("emotion").getString())
				{
					case "NEUTRAL":
						setWeight(R.id.neutral1, conf);
						setWeight(R.id.neutral2, 1 - conf);
						break;
					case "HAPPY":
						setWeight(R.id.happy1, conf);
						setWeight(R.id.happy2, 1 - conf);
						break;
					case "ANGRY":
						setWeight(R.id.angry1, conf);
						setWeight(R.id.angry2, 1 - conf);
						break;
					case "SURPRISED":
						setWeight(R.id.surprise1, conf);
						setWeight(R.id.surprise2, 1 - conf);
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


	public void setOptions(boolean [] flags, String faceCutTypeId){

		if (flags != null){
			flag_rectangle = flags[0];
			flag_angles = flags[1];
			flag_quality = flags[2];
			flag_liveness = flags[3];
			flag_age_and_gender = flags[4];
			flag_points = flags[5];
			flag_angles_vectors = flags[6];
			flag_emotions = flags[7];
			flag_face_mask = flags[8];
			flag_eyes_openness = flags[9];
		}

		 faceCutType = faceCutTypeId;
	}


	public boolean [] getFlags(){

        return new boolean[]{
            flag_rectangle,
            flag_angles,
            flag_quality,
            flag_liveness,
            flag_age_and_gender,
            flag_points,
            flag_angles_vectors,
            flag_emotions,
            flag_face_mask,
            flag_eyes_openness
        };
	}


	 public String getFaceCutType(){
	 	return faceCutType;
	 }

	public void dispose()
	{
		service.dispose();
		faceDetector.dispose();
		faceFitter.dispose();
		ageEstimator.dispose();
		genderEstimator.dispose();
		emotionsEstimator.dispose();
		faceMaskEstimator.dispose();
		if (liveness2dEstimator != null) liveness2dEstimator.dispose();
	}

}
