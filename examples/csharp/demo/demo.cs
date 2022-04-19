/**
	\file csharp_demo/video_recognition_demo/Demo.cs
*/

using System;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using CommandLine;

using VDT.FaceRecognition.SDK;

class Worker
{
	static Worker Instance = null;
	// one service
	FacerecService _service;

	// one tracker (i.e. common_video_capturer for work with video)
	Capturer _tracker;

	// one quality estimator
	QualityEstimator _quality_estimator;

	// one age and gender estimator
	AgeGenderEstimator _age_geder_estimator;

	// one emotions estimator
	EmotionsEstimator _emotions_estimator;

	// one face quality estimator
	FaceQualityEstimator _face_quality_estimator;

	// one face liveness estimator
	Liveness2DEstimator _liveness_2d_estimator;

	// two face attributes estimators
	FaceAttributesEstimator _face_mask_estimator, _eyes_openness_estimator;

	// flags for enable / disable drawing and comuting of the features
	public const int flags_count = 14;

	bool _flag_positions;
	bool _flag_angles;
	bool _flag_angles_vectors;
	bool _flag_quality;
	bool _flag_face_quality;
	bool _flag_liveness;
	bool _flag_age_gender;
	bool _flag_emotions;
	bool _flag_cutting_base;
	bool _flag_cutting_full;
	bool _flag_cutting_token;
	bool _flag_points;
	bool _flag_masked_face;
	bool _flag_eyes_openness;


	public Worker(string facerec_conf_dir, string capturer_conf, string license_dir)
	{
		_service = FacerecService.createService(facerec_conf_dir, license_dir);
		Console.WriteLine("Library version: {0}\n", _service.getVersion());
		_tracker = _service.createCapturer(
			(new FacerecService.Config(capturer_conf))
				.overrideParameter("downscale_rawsamples_to_preferred_size", 0)
				.overrideParameter("iris_enabled", 1));
		_quality_estimator = _service.createQualityEstimator("quality_estimator_iso.xml");
		_age_geder_estimator = _service.createAgeGenderEstimator("age_gender_estimator.xml");
		//_age_geder_estimator = _service.createAgeGenderEstimator("age_gender_estimator_v2.xml");
		_emotions_estimator = _service.createEmotionsEstimator("emotions_estimator.xml");
		_face_quality_estimator = _service.createFaceQualityEstimator("face_quality_estimator.xml");
		_liveness_2d_estimator =  _service.createLiveness2DEstimator("liveness_2d_estimator_v2.xml");
		_face_mask_estimator = _service.createFaceAttributesEstimator("face_mask_estimator.xml");
		_eyes_openness_estimator = _service.createFaceAttributesEstimator("eyes_openness_estimator_v2.xml");

		_flag_positions = true;
		_flag_angles = true;
		_flag_angles_vectors = true;
		_flag_quality = false;
		_flag_face_quality = false;
		_flag_liveness = false;
		_flag_age_gender = false;
		_flag_emotions = false;
		_flag_cutting_base = false;
		_flag_cutting_full = false;
		_flag_cutting_token = false;
		_flag_points = true;
		_flag_masked_face = false;
		_flag_eyes_openness = false;

		Worker.Instance = this;
	}

	bool get_flag(int i)
	{
		switch(i)
		{
			case  0: return _flag_positions;
			case  1: return _flag_angles;
			case  2: return _flag_quality;
			case  3: return _flag_liveness;
			case  4: return _flag_age_gender;
			case  5: return _flag_cutting_base;
			case  6: return _flag_cutting_full;
			case  7: return _flag_cutting_token;
			case  8: return _flag_points;
			case  9: return _flag_face_quality;
			case 10: return _flag_angles_vectors;
			case 11: return _flag_emotions;
			case 12: return _flag_masked_face;
			case 13: return _flag_eyes_openness;
		}

		return _flag_points;
	}

	void set_flag(int i, bool value)
	{
		switch(i)
		{
			case  0: _flag_positions      = value; return;
			case  1: _flag_angles         = value; return;
			case  2: _flag_quality        = value; return;
			case  3: _flag_liveness       = value; return;
			case  4: _flag_age_gender     = value; return;
			case  5: _flag_cutting_base   = value; return;
			case  6: _flag_cutting_full   = value; return;
			case  7: _flag_cutting_token  = value; return;
			case  8: _flag_points         = value; return;
			case  9: _flag_face_quality   = value; return;
			case 10: _flag_angles_vectors = value; return;
			case 11: _flag_emotions       = value; return;
			case 12: _flag_masked_face    = value; return;
			case 13: _flag_eyes_openness  = value; return;
		}
	}

	string flag_name(int i)
	{
		switch(i)
		{
			case 0: return "rectangles";
			case 1: return "angles";
			case 2: return "quality";
			case 3: return "liveness";
			case 4: return "age and gender";
			case 5: return "base cut";
			case 6: return "full cut";
			case 7: return "token cut";
			case 8: return "points";
			case 9: return "face quality";
			case 10: return "angles vectors";
			case 11: return "emotions";
			case 12: return "face mask";
			case 13: return "eyes_openness";
		}

		return "";
	}

	OpenCvSharp.Rect flag_rect(int i)
	{
		return new OpenCvSharp.Rect(16, 16 + 24 * i, 16, 16);
	}

	// mouse click callback for flags control
	static void onMouse(OpenCvSharp.MouseEvent mouseEvent, int x, int y, OpenCvSharp.MouseEvent flags)
	{
		if(mouseEvent != OpenCvSharp.MouseEvent.LButtonDown && mouseEvent != OpenCvSharp.MouseEvent.LButtonDown && mouseEvent != OpenCvSharp.MouseEvent.MButtonDown)
		{
			return;
		}

		// if mouse click is in some flag's rectangle
		// change flag state
		for(int i = 0; i < flags_count; ++i)
		{
			OpenCvSharp.Rect r = Worker.Instance.flag_rect(i);

			if(x >= r.X && y >= r.Y && x <= r.X + r.Width && y <= r.Y + r.Height)
				Worker.Instance.set_flag(i, !Worker.Instance.get_flag(i));
		}
	}

	void puttext(OpenCvSharp.Mat image, string text, OpenCvSharp.Point2f position)
	{
		// twice - for better reading
		// since we are drawing on the frame from webcam

		// white background
		OpenCvSharp.Cv2.PutText(
			image,
			text,
			position,
			OpenCvSharp.HersheyFonts.HersheyDuplex,
			0.7,
			OpenCvSharp.Scalar.All(255),
			5,
			OpenCvSharp.LineTypes.AntiAlias);

		// black text
		OpenCvSharp.Cv2.PutText(
			image,
			text,
			position,
			OpenCvSharp.HersheyFonts.HersheyDuplex,
			0.7,
			OpenCvSharp.Scalar.All(0),
			1,
			OpenCvSharp.LineTypes.AntiAlias);
	}

	public void work(OpenCvSharp.Mat frame)
	{
		// sending the frame in the capturer (_tracker)
		// pbio::CVRawImage cvri_frame;
		byte[] data = new byte[frame.Total() * frame.Type().Channels];
		Marshal.Copy(frame.DataStart, data, 0, (int)data.Length);
		RawImage ri_frame = new RawImage(frame.Width, frame.Height, RawImage.Format.FORMAT_BGR, data);
		List<RawSample> samples = _tracker.capture(ri_frame);
		// clone the frame for drawing on it
		OpenCvSharp.Mat draw_image = frame.Clone();
		// handle each face on the frame separately
		for(int i = 0; i < samples.Count; ++i)
		{
			RawSample sample = samples[i];

			// get a face rectangle
			RawSample.Rectangle rectangle = sample.getRectangle();

			// set a point to place information for this face
			OpenCvSharp.Point2f text_point = new OpenCvSharp.Point2f(
				rectangle.x + rectangle.width + 3,
				rectangle.y + 10);

			const float text_line_height = 22;

			// draw facial points
			// red color for all points
			// green for left eye
			// yellow for right eye
			// (yes, there is a mess with left and right eyes in face_sdk api,
			// but if we fix it now we will lose compatibility with previous versions)
			if(_flag_points)
			{
				List<Point> points = sample.getLandmarks();
				List<Point> iris_points = sample.getIrisLandmarks();

				for(int j = -2; j < points.Count; ++j)
				{
					Point p =
						j == -2 ?
							sample.getLeftEye() :
						j == -1 ?
							sample.getRightEye() :
						points[j];

					OpenCvSharp.Scalar color =
						j == -2 ?
							new OpenCvSharp.Scalar(50, 255, 50) :
						j == -1 ?
							new OpenCvSharp.Scalar(50, 255, 255) :
						new OpenCvSharp.Scalar(50, 50, 255);


					OpenCvSharp.Cv2.Circle(
						draw_image,
						new OpenCvSharp.Point2f(p.x, p.y),
						j < 0 ? 4 : 2,
						color,
						-1,
						OpenCvSharp.LineTypes.AntiAlias);
				}

				// draw iris points
				for(int j = 0; j < iris_points.Count; ++j)
				{
					int ms = 1;
					OpenCvSharp.Scalar color = new OpenCvSharp.Scalar(0, 255, 255);
					int oi = j - 20 * Convert.ToInt32(j >= 20);
					Point pt1 = iris_points[j];
					Point pt2 = iris_points[(oi < 19 ? j : j - 15) + 1];
					OpenCvSharp.Point2f cv_pt1 = new OpenCvSharp.Point2f(pt1.x, pt1.y);
					OpenCvSharp.Point2f cv_pt2 = new OpenCvSharp.Point2f(pt2.x, pt2.y);

					if(oi < 5)
					{
						color = new OpenCvSharp.Scalar(0, 165, 255);
						if(oi == 0)
						{
							double radius = Math.Sqrt(Math.Pow(pt1.x - pt2.x, 2) + Math.Pow(pt1.y - pt2.y, 2));
							OpenCvSharp.Cv2.Circle(
								draw_image,
								cv_pt1,
								(int) radius,
								color,
								ms,
								OpenCvSharp.LineTypes.AntiAlias);
						}
					}else
					{
						OpenCvSharp.Cv2.Line(
							draw_image,
							cv_pt1,
							cv_pt2,
							color,
							ms,
							OpenCvSharp.LineTypes.AntiAlias);
					}

					OpenCvSharp.Cv2.Circle(
						draw_image,
						cv_pt1,
						ms,
						color,
						-1,
						OpenCvSharp.LineTypes.AntiAlias);
				}
			}

			// draw rectangle
			if(_flag_positions)
			{

				OpenCvSharp.Cv2.Rectangle(
					draw_image,
					new OpenCvSharp.Rect(
						rectangle.x,
						rectangle.y,
						rectangle.width,
						rectangle.height),
					new OpenCvSharp.Scalar(50, 50, 255),
					2,
					OpenCvSharp.LineTypes.AntiAlias);
			}

			// draw age and gender
			if( _flag_age_gender )
			{
				AgeGenderEstimator.AgeGender age_gender =	_age_geder_estimator.estimateAgeGender(sample);

				string age_text = "age: ";

				switch(age_gender.age)
				{
					case AgeGenderEstimator.Age.AGE_KID    : age_text += "kid    "; break;
					case AgeGenderEstimator.Age.AGE_YOUNG  : age_text += "young  "; break;
					case AgeGenderEstimator.Age.AGE_ADULT  : age_text += "adult  "; break;
					case AgeGenderEstimator.Age.AGE_SENIOR : age_text += "senior "; break;
				}

				age_text += string.Format("years: {0:G3}", age_gender.age_years);

				puttext(
					draw_image,
					age_text,
					text_point);
				text_point.Y += text_line_height;

				puttext(
					draw_image,
					age_gender.gender == AgeGenderEstimator.Gender.GENDER_FEMALE ? "gender: female" :
					age_gender.gender == AgeGenderEstimator.Gender.GENDER_MALE ? "gender: male" : "?",
					text_point);
				text_point.Y += text_line_height;

				text_point.Y += text_line_height / 3;
			}

			// draw emotions
			if( _flag_emotions )
			{
				List<EmotionsEstimator.EmotionConfidence> emotions =
					_emotions_estimator.estimateEmotions(sample);

				for(int j = 0; j < emotions.Count; ++j)
				{
					EmotionsEstimator.Emotion emotion = emotions[j].emotion;
					float confidence = emotions[j].confidence;

					OpenCvSharp.Cv2.Rectangle(
						draw_image,
						new OpenCvSharp.Rect(
							(int)text_point.X,
							(int)text_point.Y - (int)text_line_height / 2,
							(int)(100 * confidence),
							(int)text_line_height),
						emotion == EmotionsEstimator.Emotion.EMOTION_NEUTRAL  ? new OpenCvSharp.Scalar(255, 0, 0) :
						emotion == EmotionsEstimator.Emotion.EMOTION_HAPPY    ? new OpenCvSharp.Scalar(0, 255, 0) :
						emotion == EmotionsEstimator.Emotion.EMOTION_ANGRY    ? new OpenCvSharp.Scalar(0, 0, 255) :
						emotion == EmotionsEstimator.Emotion.EMOTION_SURPRISE ? new OpenCvSharp.Scalar(0, 255, 255) :
							new OpenCvSharp.Scalar(0, 0, 0),
						-1);

					puttext(
						draw_image,
						emotion == EmotionsEstimator.Emotion.EMOTION_NEUTRAL  ? "neutral" :
						emotion == EmotionsEstimator.Emotion.EMOTION_HAPPY    ? "happy" :
						emotion == EmotionsEstimator.Emotion.EMOTION_ANGRY    ? "angry" :
						emotion == EmotionsEstimator.Emotion.EMOTION_SURPRISE ? "surprise" : "?",
						text_point + new OpenCvSharp.Point2f(100, 0));

					text_point.Y += text_line_height;

					text_point.Y += text_line_height / 3;
				}
			}


			// draw angles text
			if( _flag_angles )
			{
				
				string yaw, pitch, roll;
				yaw = string.Format("yaw: {0}", (0.1f * (int)10 * sample.getAngles().yaw + 0.5f));
				pitch = string.Format("pitch: {0}", (0.1f * (int)10 * sample.getAngles().pitch + 0.5f));
				roll = string.Format("roll: {0}", (0.1f * (int)10 * sample.getAngles().roll + 0.5f));

				puttext(draw_image, yaw, text_point);
				text_point.Y += text_line_height;

				puttext(draw_image, pitch, text_point);
				text_point.Y += text_line_height;

				puttext(draw_image, roll, text_point);
				text_point.Y += text_line_height;

				text_point.Y += text_line_height / 3;
			}

			// draw angles vectors
			if( _flag_angles_vectors )
			{
				RawSample.Angles angles = sample.getAngles();

				float cos_a = (float)Math.Cos( angles.yaw * OpenCvSharp.Cv2.PI / 180 );
				float sin_a = (float)Math.Sin( angles.yaw * OpenCvSharp.Cv2.PI / 180 );

				float cos_b = (float)Math.Cos( angles.pitch * OpenCvSharp.Cv2.PI / 180 );
				float sin_b = (float)Math.Sin( angles.pitch * OpenCvSharp.Cv2.PI / 180 );

				float cos_c = (float)Math.Cos( angles.roll * OpenCvSharp.Cv2.PI / 180 );
				float sin_c = (float)Math.Sin( angles.roll * OpenCvSharp.Cv2.PI / 180 );

				OpenCvSharp.Point3f[] xyz = {
					new OpenCvSharp.Point3f(cos_a * cos_c, -sin_c, -sin_a),
					new OpenCvSharp.Point3f(sin_c, cos_b * cos_c, -sin_b),
					new OpenCvSharp.Point3f(sin_a, sin_b, cos_a * cos_b) };

				OpenCvSharp.Point2f center = new OpenCvSharp.Point2f(
					(sample.getLeftEye().x + sample.getRightEye().x) * 0.5f,
					(sample.getLeftEye().y + sample.getRightEye().y) * 0.5f);

				float length = (rectangle.width + rectangle.height) * 0.3f;

				for(int c = 0; c < 3; ++c)
				{
					OpenCvSharp.Cv2.Line(
						draw_image,
						center,
						center + new OpenCvSharp.Point2f(xyz[c].X, -xyz[c].Y) * length,
						c == 0 ? new OpenCvSharp.Scalar(50, 255, 255) :
						c == 1 ? new OpenCvSharp.Scalar(50, 255, 50) :
						c == 2 ? new OpenCvSharp.Scalar(50, 50, 255) : new OpenCvSharp.Scalar(),
						2,
						OpenCvSharp.LineTypes.AntiAlias);
				}
			}

			// draw quality text
			if( _flag_quality )
			{
				QualityEstimator.Quality q =
					_quality_estimator.estimateQuality(sample);

				string lighting, noise, sharpness, flare;

				lighting = "lighting: " + q.lighting.ToString();
				puttext(draw_image, lighting, text_point);
				text_point.Y += text_line_height;

				noise = "noise: " + q.noise.ToString();
				puttext(draw_image, noise, text_point);
				text_point.Y += text_line_height;

				sharpness = "sharpness: " + q.sharpness.ToString();
				puttext(draw_image, sharpness, text_point);
				text_point.Y += text_line_height;

				flare = "flare: " + q.flare.ToString();
				puttext(draw_image, flare, text_point);
				text_point.Y += text_line_height;

				text_point.Y += text_line_height / 3;
			}

			// draw liveness text
			if( _flag_liveness )
			{
				Liveness2DEstimator.LivenessAndScore liveness_2d_result = _liveness_2d_estimator.estimate(sample);
				string score_str = Math.Round(liveness_2d_result.score, 3).ToString();

				puttext(
					draw_image,
					"liveness: " + (
						liveness_2d_result.liveness == Liveness2DEstimator.Liveness.REAL ? score_str + " - real"  :
						liveness_2d_result.liveness == Liveness2DEstimator.Liveness.FAKE ? score_str + " - fake" :
						liveness_2d_result.liveness == Liveness2DEstimator.Liveness.NOT_ENOUGH_DATA ? "not enough data" : "??"),
					text_point);

				text_point.Y += text_line_height;
				text_point.Y += text_line_height / 3;
			}

			// draw face quality
			if( _flag_face_quality )
			{
				float quality = _face_quality_estimator.estimateQuality(sample);

				string ss = "face quality: " + quality.ToString();
				puttext(draw_image, ss, text_point);
				text_point.Y += text_line_height;
				text_point.Y += text_line_height / 3;
			}

			// draw face attribute (masked_face)
			if(_flag_masked_face) {
				FaceAttributesEstimator.Attribute attr = _face_mask_estimator.estimate(sample);
				string score_str = Math.Round(attr.score, 3).ToString();
				puttext(
					draw_image,
					"masked: " + (attr.verdict ? "true - " : "false - ") + score_str,
					text_point);
				text_point.Y += text_line_height;
				text_point.Y += text_line_height / 3;
			}

			// draw face attribute (eyes_openness)
			if(_flag_eyes_openness) {
				FaceAttributesEstimator.Attribute attr = _eyes_openness_estimator.estimate(sample);
				string left_score_str = Math.Round(attr.left_eye_state.score, 3).ToString();
				string right_score_str = Math.Round(attr.right_eye_state.score, 3).ToString();

				string left_verdict_str = "not_computed";
				string right_verdict_str = "not_computed";

				if (attr.left_eye_state.score != -1)
					left_verdict_str = (attr.left_eye_state.eye_state == FaceAttributesEstimator.EyeStateScore.EyeState.OPENED ? "true" : "false");

				if (attr.right_eye_state.score != -1)
					right_verdict_str = (attr.right_eye_state.eye_state == FaceAttributesEstimator.EyeStateScore.EyeState.OPENED ? "true" : "false");

				puttext(
					draw_image,
					"left eye: " + left_verdict_str + " " + left_score_str,
					text_point);
				text_point.Y += text_line_height;
				text_point.Y += text_line_height / 3;
				puttext(
					draw_image,
					"right eye: " + right_verdict_str + " " + right_score_str,
					text_point);
				text_point.Y += text_line_height;
				text_point.Y += text_line_height / 3;
			}

			// draw face cuts
			for(int cut_i = 0; cut_i < 3; ++cut_i)
			{
				if( (cut_i == 0 && ! _flag_cutting_base) ||
					(cut_i == 1 && ! _flag_cutting_full) ||
					(cut_i == 2 && ! _flag_cutting_token) )
				{
					continue;
				}

				puttext(
					draw_image,
					cut_i == 0 ? "base cut:" :
					cut_i == 1 ? "full cut:" :
					cut_i == 2 ? "token cut:" : "?? cut",
					text_point);
				text_point.Y += text_line_height / 2;

				MemoryStream obuf = new MemoryStream();
				sample.cutFaceImage(
					obuf,
					RawSample.ImageFormat.IMAGE_FORMAT_BMP,
					cut_i == 0 ? RawSample.FaceCutType.FACE_CUT_BASE :
					cut_i == 1 ? RawSample.FaceCutType.FACE_CUT_FULL_FRONTAL :
					cut_i == 2 ? RawSample.FaceCutType.FACE_CUT_TOKEN_FRONTAL :
					(RawSample.FaceCutType)999);

				byte[] sbuf = obuf.ToArray();

				// const OpenCvSharp.Mat_<uchar> cvbuf(1, sbuf.length(), (uchar*) sbuf.c_str());

				OpenCvSharp.Mat img = OpenCvSharp.Cv2.ImDecode(sbuf, OpenCvSharp.ImreadModes.Unchanged);

				OpenCvSharp.Cv2.Resize(img, img, OpenCvSharp.Size.Zero, 0.3, 0.3);


				int img_rect_x = (int)Math.Max(0, -text_point.X);
				int img_rect_y = (int)Math.Max(0, -text_point.Y);

				int img_rect_width = (int)Math.Min(
					img.Cols - img_rect_x,
					draw_image.Cols - Math.Max(0, text_point.X));

				int img_rect_height = (int)Math.Min(
					img.Rows - img_rect_y,
					draw_image.Rows - Math.Max(0, text_point.Y));

				if(img_rect_width <= 0 || img_rect_height <= 0)
					continue;

				OpenCvSharp.Rect img_rect = new OpenCvSharp.Rect(img_rect_x, img_rect_y, img_rect_width, img_rect_height);

				img[img_rect].CopyTo(
					draw_image[new OpenCvSharp.Rect(
						(int)Math.Max(0, text_point.X),
						(int)Math.Max(0, text_point.Y),
						img_rect.Width,
						img_rect.Height)]);

				text_point.Y += text_line_height / 2;
				text_point.Y += img.Rows;


				text_point.Y += text_line_height / 3;
			}

		}
		// draw checkboxes
		for(int i = 0; i < flags_count; ++i)
		{
			OpenCvSharp.Rect rect = flag_rect(i);
			OpenCvSharp.Rect rect2 = new OpenCvSharp.Rect(rect.X + 5, rect.Y + 5, rect.Width - 10, rect.Height - 10);

			OpenCvSharp.Cv2.Rectangle(draw_image, rect, OpenCvSharp.Scalar.All(255), -1);
			OpenCvSharp.Cv2.Rectangle(draw_image, rect, OpenCvSharp.Scalar.All(0), 2, OpenCvSharp.LineTypes.AntiAlias);

			if( get_flag(i) )
			{
				OpenCvSharp.Cv2.Rectangle(draw_image, rect2, OpenCvSharp.Scalar.All(0), -1, OpenCvSharp.LineTypes.AntiAlias);
			}

			puttext(
				draw_image,
				flag_name(i),
				new OpenCvSharp.Point2f(rect.X + rect.Width + 3, rect.Y + rect.Height - 3));
		}
		
		
		// show image with drawed information
		OpenCvSharp.Cv2.ImShow("demo", draw_image);

		// register callback on mouse events
		OpenCvSharp.Cv2.SetMouseCallback("demo", (OpenCvSharp.CvMouseCallback)onMouse);
	}

	public void dispose()
	{
		_service.Dispose();
		_tracker.Dispose();
		_quality_estimator.Dispose();
		_age_geder_estimator.Dispose();
		_emotions_estimator.Dispose();
		_face_quality_estimator.Dispose();
		_liveness_2d_estimator.Dispose();
		_face_mask_estimator.Dispose();
	}
};

class Options
{
	[Option("config_dir", Default = "../../../conf/facerec", HelpText = "Path to config directory.")]
	public string config_dir { get; set; }

	[Option("capturer_config", Default = "fda_tracker_capturer_blf.xml", HelpText = "Capturer config file.")]
	public string capturer_config { get; set; }

	[Option("license_dir", Default = null, HelpText = "Path to license directory [optional].")]
	public string license_dir { get; set; }
};

namespace csharp_demo
{
	class Program
	{
		static int Main(string[] args)
		{
			// print usage
			Console.WriteLine("Usage: dotnet csharp_demo.dll [OPTIONS]");
			Console.WriteLine("Examples:");
			Console.WriteLine("    To specify license directory (for online license for example): dotnet csharp_demo.dll --license_dir license_dir");
			Console.WriteLine("");

			// parse arguments
			bool error = false;
			Options options = new Options();
			CommandLine.Parser.Default.ParseArguments<Options>(args)
				.WithParsed<Options>(opts => options = opts)
				.WithNotParsed<Options>(errs => error = true);

			// exit if argument parsign error
			if (error) return 1;

			// print values of arguments
			Console.WriteLine("Arguments:");
			foreach(var opt in options.GetType().GetProperties())
			{
				Console.WriteLine("--{0} = {1}", opt.Name, opt.GetValue(options, null));
			}
			Console.WriteLine();

			try
			{
				OpenCvSharp.Window window = new OpenCvSharp.Window("demo");
				// create worker
				// (we are expecting run from bin folder)
				Worker worker = new Worker(
					options.config_dir,
					options.capturer_config,
					options.license_dir);

				// here we trying camera open with opencv
				const int desiredFrameWidht = 1280;
				const int desiredFrameHeight = 720;

				int camera_id = 0;

				// VideoCapture camera;
				OpenCvSharp.VideoCapture camera = new OpenCvSharp.VideoCapture();

				
				for(; camera_id < 10; ++camera_id)
				{
					camera.Open(camera_id);
					camera.Set(OpenCvSharp.CaptureProperty.FrameWidth, desiredFrameWidht);
					camera.Set(OpenCvSharp.CaptureProperty.FrameHeight, desiredFrameHeight); 
					OpenCvSharp.Mat frame = new OpenCvSharp.Mat();
					for (int i = 0; i < 10; i++)
						frame = camera.RetrieveMat();

					if(frame.Empty())
					{
						Console.WriteLine("webcam " + camera_id + " not opened");
						camera.Release();
					}
					else
					{
						break;
					}
				}

				for(;;)
				{
					OpenCvSharp.Mat frame;

					//camera.Grab();
					frame = camera.RetrieveMat();

					if(frame.Empty())
						break;
						
					// give a frame to the worker
					worker.work(frame);

					if(27 == (char) OpenCvSharp.Cv2.WaitKey(10))
					{
						camera.Release(); 
						break;
					}
				}

				// force free resources
				// otherwise licence error may occur
				// when create sdk object in next time 
				worker.dispose();
			
			}
			catch(Exception e)
			{	
				Console.WriteLine("! exception catched: '" + e + "' ... exiting");
				return 1;
			}
			Console.WriteLine("close program");
			return 0;
		}
	}
}