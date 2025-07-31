using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Data.SqlTypes;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using CommandLine;
using OpenCvSharp;
using System.Diagnostics;
using System.Text.RegularExpressions;

using VDT.FaceRecognition.SDK;

public class Options
{
	[Option("sdk_path", Default = "../../../", HelpText = "Path to FaceSDK directory.")]
	public string SdkPath { get; set; }

	[Option("input_image")]
	public string InputImage { get; set; }

	[Option("unit_type", Default = "objects", HelpText = "body|face|face_keypoint|pose|objects|emotions|age|gender|mask|glasses|liveness|quality")]
	public string UnitType { get; set; }

	[Option("version", Default = 0)]
	public int Version { get; set; }

	[Option("modification", Default = "")]
	public string Modification { get; set; }

	[Option("use_cuda", Default = false)]
	public bool UseCuda { get; set; }
};

namespace csharp_processing_block_demo
{
	class Program
	{
		private static readonly Dictionary<string, string> unitTypes = new()
		{
			{ "body", "HUMAN_BODY_DETECTOR" },
			{ "face", "FACE_DETECTOR" },
			{ "objects", "OBJECT_DETECTOR" },
			{ "face_keypoint", "FACE_FITTER" },
			{ "emotions", "EMOTION_ESTIMATOR" },
			{ "age", "AGE_ESTIMATOR" },
			{ "gender","GENDER_ESTIMATOR" },
			{ "mask", "MASK_ESTIMATOR" },
			{ "glasses", "GLASSES_ESTIMATOR" },
			{ "eye_openness", "EYE_OPENNESS_ESTIMATOR" },
			{ "liveness", "LIVENESS_ESTIMATOR" },
			{ "deepfake", "DEEPFAKE_ESTIMATOR" },
			{ "quality", "QUALITY_ASSESSMENT_ESTIMATOR" },
			{ "pose", "HUMAN_POSE_ESTIMATOR" },
		};

		private const int SM_CXSCREEN = 0;
    	private const int SM_CYSCREEN = 1;

		[DllImport("user32.dll")]
    	public static extern int GetSystemMetrics(int nIndex);

		static int Main(string[] args)
		{
			Console.WriteLine
			(
				$@"Usage: dotnet csharp_csharp_processing_block_demo.dll {System.Reflection.Assembly.GetExecutingAssembly().Location}
				[--input_image <path to image>]
				[--unit_type body|face|face_keypoint|pose|objects|emotions|age|gender|mask|glasses|eye_openness|liveness|quality]
				[--sdk_path ../../../]
				[--use_cuda]"
			);

			bool error = false;
			Options options = new Options();
			CommandLine.Parser.Default.ParseArguments<Options>(args)
				.WithParsed<Options>(opts => options = opts)
				.WithNotParsed<Options>(errs => error = true);

			if (error)
			{
				return 1;
			}

			Console.WriteLine("Arguments:");
			foreach (var opt in options.GetType().GetProperties())
			{
				Console.WriteLine($"--{ToSnakeCase(opt.Name)} = {opt.GetValue(options, null)}");
			}
			Console.WriteLine();

			try
			{
				FacerecService service = FacerecService.createService(options.SdkPath + "/conf/facerec/", options.SdkPath + "/license/");
				string libDir = options.SdkPath + (OperatingSystem.IsWindows() ? "\\bin" : "/lib");
    
				string unitType = options.UnitType;
				Dictionary<object, object> configContext = new()
				{
					{
						"ONNXRuntime",
						new Dictionary<object, object>
						{
							{ "library_path", libDir }
						}
					}
				};

				Console.WriteLine($"Library version: {service.getVersion()}");

				configContext["unit_type"] = unitTypes[unitType];
				configContext["use_cuda"] = options.UseCuda;

				if (options.Version != 0)
				{
					configContext["version"] = options.Version;
				}

				if (unitType == "quality")
				{
					configContext["config_name"] = "quality_assessment.xml";
				}
				else if (unitType == "liveness")
				{
					configContext["config_name"] = "liveness_2d_estimator_v3.xml";
					configContext["sdk_path"] = options.SdkPath;
					configContext["facerec_conf_dir"] = options.SdkPath + "/conf/facerec";
				}

				if (options.Modification != "")
				{
					configContext["modification"] = options.Modification;
				}

				ProcessingBlock block = service.CreateProcessingBlock(configContext);
				Context ioData = service.CreateContextFromEncodedImage(GetImageData(options.InputImage));
				Mat image = Cv2.ImRead(options.InputImage);

				if (unitType == "quality" || unitType == "liveness" || unitType == "deepfake")
				{
					ProcessingBlock faceDetector = service.CreateProcessingBlock(CreateFaceDetector(service));
					ProcessingBlock faceFitter = service.CreateProcessingBlock(CreateFaceFitter(service));

					faceDetector.Invoke(ioData);
					faceFitter.Invoke(ioData);
				}
				else if (new List<string> { "emotions", "gender", "age", "mask", "glasses", "eye_openness", "face_keypoint" }.Contains(unitType))
				{
					ProcessingBlock faceDetector = service.CreateProcessingBlock(CreateFaceDetector(service));

					faceDetector.Invoke(ioData);

					if (unitType != "face_keypoint")
					{
						ProcessingBlock faceFitter = service.CreateProcessingBlock(CreateFaceFitter(service));

						faceFitter.Invoke(ioData);
					}
				}
				else if (unitType == "pose")
				{
					ProcessingBlock bodyDetector = service.CreateProcessingBlock
						(
							new Dictionary<object, object>
							{
								{ "unit_type", unitTypes["body"] },
								{ "ONNXRuntime", configContext["ONNXRuntime"] },
								{ "confidence_threshold", 0.4 },
								{ "iou_threshold", 0.45 },
								{ "use_cuda", options.UseCuda }
							}
						);

					bodyDetector.Invoke(ioData);
				}

				block.Invoke(ioData);

				if (ioData["objects"].Length() == 0)
				{
					Console.WriteLine("No objects have been detected!");

					return 0;
				}

				switch (unitType)
				{
					case "body":
					case "pose":
						DrawObjects(ioData, image, "body");

						break;

					case "face":
						DrawObjects(ioData, image, "face");

						break;

					case "face_keypoint":
						DrawFaceKeypoint(ioData, image);

						break;

					case "objects":
						DrawObjects(ioData, image);

						break;

					case "emotions":
						DrawEmotions(ioData, image);

						break;

					case "age":
					case "gender":
					case "mask":
					case "quality":
						DrawAgeGenderMaskQuality(ioData, image, unitType);

						break;

					case "glasses":
						DrawGlasses(ioData, image);

						break;

					case "eye_openness":
						DrawEyeOpenness(ioData, image);

						break;

					case "liveness":
						DrawLiveness(ioData, image);

						break;

					case "deepfake":
						DrawDeepfake(ioData, image);

						break;
				}

				int width = 0;
				int height = 0;

				GetScreenResolution(ref width, ref height);

				float widthToHeight = ((float)image.Cols) / ((float)image.Rows);

				FitResize(width, height, ref image, widthToHeight);

				Cv2.ImShow("image", image);
				Cv2.WaitKey();
			}
			catch (Exception e)
			{
				Console.WriteLine($"! exception catched: '{e}' ... exiting");

				return 1;
			}

			return 0;
		}

		private static void DrawObjects(Context ioData, Mat image, string classFilter = "")
		{
			List<Tuple<string, string>> bone_map = new()
			{
				new("right_ankle", "right_knee"),
				new("right_knee","right_hip"),
				new("left_hip","right_hip"),
				new("left_shoulder","left_hip"),
				new("right_shoulder","right_hip"),
				new("left_shoulder","right_shoulder"),
				new("left_shoulder","left_elbow"),
				new("right_shoulder","right_elbow"),
				new("left_elbow","left_wrist"),
				new("right_elbow","right_wrist"),
				new("left_eye","right_eye"),
				new("nose","left_eye"),
				new("left_knee", "left_hip"),
				new("right_ear", "right_shoulder"),
				new("left_ear", "left_shoulder"),
				new("right_eye", "right_ear"),
				new("left_eye", "left_ear"),
				new("nose", "right_eye"),
				new("left_ankle", "left_knee")
			};

			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];
				string className = obj["class"].GetString();
				if (classFilter.Length != 0 && className != classFilter)
				{
					continue;
				}

				Context rectCtx = obj["bbox"];
				OpenCvSharp.Point topLeft = new(Math.Max(rectCtx[0].GetDouble() * width, 0), Math.Max(rectCtx[1].GetDouble() * heigth, 0));
				OpenCvSharp.Point bottomRight = new(Math.Min(rectCtx[2].GetDouble() * width, width), Math.Min(rectCtx[3].GetDouble() * heigth, heigth));
				Cv2.Rectangle(image, topLeft, bottomRight, new Scalar(0, 255, 0));
				if (classFilter.Length == 0)
				{
					PutTextWithRightExpansion(image, className, new(topLeft.X, topLeft.Y + 20), HersheyFonts.HersheyDuplex, 1, new(0, 255, 0));
				}

				if (classFilter == "body" && obj.Contains("keypoints"))
				{
					Context posesCtx = obj["keypoints"];

					foreach (string key in posesCtx.Keys())
					{
						Context proj = posesCtx[key]["proj"];

						Cv2.Circle(image, new(proj[0].GetDouble() * width, proj[1].GetDouble() * heigth), 3, new(0, 255, 0));
					}

					foreach (Tuple<string, string> bone in bone_map)
					{
						string key1 = bone.Item1;
						string key2 = bone.Item2;
						int x1 = (int)(posesCtx[key1]["proj"][0].GetDouble() * width);
						int y1 = (int)(posesCtx[key1]["proj"][1].GetDouble() * heigth);
						int x2 = (int)(posesCtx[key2]["proj"][0].GetDouble() * width);
						int y2 = (int)(posesCtx[key2]["proj"][1].GetDouble() * heigth);

						Cv2.Line(image, new(x1, y1), new(x2, y2), new(0, 255, 0), 1, LineTypes.Link4);
					}
				}
			}
		}

		private static void DrawFaceKeypoint(Context ioData, Mat image)
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];
				Context points = obj["keypoints"]["points"];

				for (int j = 0; j < (int)points.Length(); j++)
				{
					Context point = points[j];

					Cv2.Circle(image, new(point["proj"][0].GetDouble() * width, point["proj"][1].GetDouble() * heigth), 2, new Scalar(0, 255, 0), 5);
				}
			}
		}
		private static void DrawEmotions(Context ioData, Mat image)
		{
			int width = image.Cols;
			int height = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];
				string className = obj["class"].GetString();

				if (className != "face")
				{
					continue;
				}

				Context emotions = obj["emotions"];
				OpenCvSharp.Point textPoint = new
				(
					Math.Min(obj["bbox"][2].GetDouble() * width, width),
					Math.Max(obj["bbox"][1].GetDouble() * height, 0) + 15
				);

				SortedSet<Tuple<double, string>> results = new();

				for (int j = 0; j < (int)emotions.Length(); j++)
				{
					results.Add(new(emotions[j]["confidence"].GetDouble(), emotions[j]["emotion"].GetString()));
				}

				double topShift = results.Last().Item1;

				double lengthCoef = 50;
				float fontHeight = 12;

				foreach (Tuple<double, string> iterator in results.AsEnumerable().Reverse())
				{
					string emotion = iterator.Item2;
					double confidence = iterator.Item1;
					int textLineHeight = (int)(fontHeight * Math.Max(0.5, confidence / topShift));

					PutTextWithRightExpansion(image, emotion, textPoint + new OpenCvSharp.Point(topShift * lengthCoef, 0), HersheyFonts.HersheyDuplex, textLineHeight / (fontHeight * 2), new(255, 255, 0));

					Cv2.Rectangle
					(
						image,
						new Rect(textPoint.X, textPoint.Y - textLineHeight / 2, (int)(confidence * lengthCoef), textLineHeight),
						emotion == "NEUTRAL" ? new(255, 0, 0) :
						emotion == "HAPPY" ? new(0, 255, 0) :
						emotion == "ANGRY" ? new(0, 0, 255) :
						emotion == "SURPRISED" ? new(0, 255, 255) :
						emotion == "DISGUSTED" ? new(255, 255, 0) :
						emotion == "SAD" ? new(255, 0, 255) :
						emotion == "SCARED" ? new(127, 127, 127) :
						new(0, 0, 0),
						-1
					);

					textPoint.Y += textLineHeight;
					textPoint.Y += textLineHeight / 3;
				}
			}
		}

		private static void DrawAgeGenderMaskQuality(Context ioData, Mat image, string classFilter = "")
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Mat imageCopy = image.Clone();
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			int objectsCounter = 1;

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];

				if (obj["class"].GetString() != "face")
				{
					continue;
				}

				OpenCvSharp.Point textPoint = new
				(
					Math.Min(obj["bbox"][2].GetDouble() * width, width),
					Math.Max(obj["bbox"][1].GetDouble() * heigth, 0) + 15
				);

				if (classFilter == "gender")
				{
					PutTextWithRightExpansion(image, obj["gender"].GetString(), textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);
				}
				else if (classFilter == "age")
				{
					PutTextWithRightExpansion(image, $"Age: {obj[classFilter].GetLong()}", textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);
				}
				else if (classFilter == "mask")
				{
					PutTextWithRightExpansion(image, $"Mask: {obj["has_medical_mask"]["value"].GetBool()}", textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);
				}
				else if (classFilter == "quality")
				{
					if (objectsCounter <= 6)
					{
						OpenCvSharp.Point topLeft = new(Math.Max(obj["bbox"][0].GetDouble() * width, 0), Math.Max(obj["bbox"][1].GetDouble() * heigth, 0));
						OpenCvSharp.Point bottomRight = new(Math.Min(obj["bbox"][2].GetDouble() * width, width), Math.Min(obj["bbox"][3].GetDouble() * heigth, heigth));
						Rect faceRect = Rect.FromLTRB(topLeft.X, topLeft.Y, bottomRight.X, bottomRight.Y);
						int heightForTextAndCrop = (int)(heigth * 0.15 * objectsCounter - (heigth * 0.05));
						Mat face = imageCopy[faceRect];
						double scaleFactor = heigth / (7.0 * face.Rows);

						Cv2.Resize(face, face, new(), scaleFactor, scaleFactor, InterpolationFlags.Linear);

						Rect roi = new(width + 5, heightForTextAndCrop, face.Cols, face.Rows);

						PutTextWithRightExpansion(image, "Result of 6 first faces:", new OpenCvSharp.Point(width, 20), HersheyFonts.HersheySimplex,
													0.5, new(0, 255, 0), 1);

						PutTextWithRightExpansion(image, "Quality :", new OpenCvSharp.Point(width + 5 + roi.Width, heightForTextAndCrop + 30), HersheyFonts.HersheyDuplex,
													0.5, new(0, 255, 0), 1);

						PutTextWithRightExpansion(image, Math.Round(obj["quality"]["total_score"].GetDouble() * 100).ToString(),
												new OpenCvSharp.Point(width + 75 + roi.Width, heightForTextAndCrop + 30), HersheyFonts.HersheyDuplex,
													0.5, new(0, 255, 0), 1);
						Mat drawRoi = image[roi];

						face.CopyTo(drawRoi);

						objectsCounter++;
					}
				}
			}
		}

		private static void DrawGlasses(Context ioData, Mat image)
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];

				if (obj["class"].GetString() != "face")
				{
					continue;
				}

				Context glasses = obj["glasses"];
				OpenCvSharp.Point textPoint = new
				(
					Math.Min(obj["bbox"][2].GetDouble() * width, width),
					Math.Max(obj["bbox"][1].GetDouble() * heigth, 0) + 15
				);

				PutTextWithRightExpansion
				(
					image, $"Glasses: {glasses["value"].GetBool()}",
					textPoint, 
					HersheyFonts.HersheyDuplex, 
					0.5, 
					new(0, 0, 255), 
					1
				);

				textPoint.Y += 15;

				PutTextWithRightExpansion
				(
					image, $"Confidence: {glasses["confidence"].GetDouble()}",
					textPoint, 
					HersheyFonts.HersheyDuplex, 
					0.5, 
					new(0, 0, 255), 
					1
				);
			}
		}

		private static void DrawDeepfake(Context ioData, Mat image)
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];

				if (obj["class"].GetString() != "face")
				{
					continue;
				}

				Context deepfake = obj["deepfake"];
				OpenCvSharp.Point textPoint = new
				(
					Math.Min(obj["bbox"][2].GetDouble() * width, width),
					Math.Max(obj["bbox"][1].GetDouble() * heigth, 0) + 15
				);

				PutTextWithRightExpansion
				(
					image, $"Deepfake: {deepfake["value"].GetBool()}",
					textPoint,
					HersheyFonts.HersheyDuplex,
					0.5,
					new(0, 0, 255),
					1
				);

				textPoint.Y += 15;

				PutTextWithRightExpansion
				(
					image, $"Confidence: {deepfake["confidence"].GetDouble()}",
					textPoint,
					HersheyFonts.HersheyDuplex,
					0.5,
					new(0, 0, 255),
					1
				);
			}
		}

		private static void DrawEyeOpenness(Context ioData, Mat image)
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];

				if (obj["class"].GetString() != "face")
				{
					continue;
				}

				OpenCvSharp.Point textPoint = new(obj["bbox"][2].GetDouble() * width, obj["bbox"][1].GetDouble() * heigth);

				PutTextWithRightExpansion(image, $"Is left eye open: {obj["is_left_eye_open"]["value"].GetBool()}", textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);

				textPoint.Y += 15;

				PutTextWithRightExpansion(image, $"Is right eye open: {obj["is_right_eye_open"]["value"].GetBool()}", textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);
			}
		}

		private static void DrawLiveness(Context ioData, Mat image)
		{
			int width = image.Cols;
			int heigth = image.Rows;
			Context objects = ioData["objects"];

			DrawObjects(ioData, image, "face");

			for (int i = 0; i < (int)objects.Length(); i++)
			{
				Context obj = objects[i];

				if (obj["class"].GetString() != "face")
				{
					continue;
				}

				OpenCvSharp.Point textPoint = new(obj["bbox"][2].GetDouble() * width, obj["bbox"][1].GetDouble() * heigth);

				if (obj.Contains("liveness"))
				{
					Context liveness = obj["liveness"];

					PutTextWithRightExpansion(image, liveness["value"].GetString(), textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);

					textPoint.Y += 12;

					PutTextWithRightExpansion(image, liveness["confidence"].GetDouble().ToString(), textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);

					if (liveness.Contains("info"))
					{
						textPoint.Y += 12;

						PutTextWithRightExpansion(image, $"info: {liveness["info"].GetString()}", textPoint, HersheyFonts.HersheyDuplex, 0.5, new(0, 0, 255), 1);
					}
				}
			}
		}

		private static void PutTextWithRightExpansion(Mat image, string text, OpenCvSharp.Point org, HersheyFonts fontFace, double fontScale, Scalar color,
									int thickness = 1, LineTypes lineType = OpenCvSharp.LineTypes.Link8, bool bottomLeftOrigin = false, BorderTypes borderType = BorderTypes.Constant)
		{

			Size textSize = Cv2.GetTextSize(text, fontFace, fontScale, thickness, out int baseLine);
			int rightBorder = textSize.Width + org.X - image.Cols;

			if (rightBorder > 0)
			{
				Cv2.CopyMakeBorder(image, image, 0, 0, 0, rightBorder + 2, borderType);
			}

			Cv2.PutText(image, text, org, fontFace, fontScale, color, thickness, lineType, bottomLeftOrigin);
		}

		private static void MatToBsm(ref Dictionary<object, object> bsmCtx, Mat img)
		{
			Dictionary<int, string> CvTypeToStr = new()
			{
				{MatType.CV_8U,"uint8_t"}, {MatType.CV_8S, "int8_t"},
				{MatType.CV_16U, "uint16_t"}, {MatType.CV_16S, "int16_t"} ,
				{MatType.CV_32S, "int32_t"}, {MatType.CV_32F, "float"}, {MatType.CV_64F, "double"}
			};

			var inputImage = img.IsContinuous() ? img : img.Clone();
			long size = inputImage.Total() * inputImage.ElemSize();
			List<object> sizes = new();
			byte[] buffer = new byte[size];

			using (Mat temp = new(inputImage.Rows, inputImage.Cols, inputImage.Type(), buffer))
			{
				inputImage.CopyTo(temp);
			}

			for (int i = 0; i < inputImage.Dims; ++i)
			{
				sizes.Add(inputImage.Size(i));
			}

			sizes.Add(inputImage.Channels());

			bsmCtx["blob"] = buffer;
			bsmCtx["format"] = "NDARRAY";
			bsmCtx["shape"] = sizes;
			bsmCtx["dtype"] = CvTypeToStr[inputImage.Depth()];
		}

		private static byte[] GetImageData(string imagePath)
		{
			return File.ReadAllBytes(imagePath);
		}

		private static string ToSnakeCase(string text)
		{
			if (text == null)
			{
				throw new ArgumentNullException(nameof(text));
			}

			if (text.Length < 2)
			{
				return text;
			}

			StringBuilder stringBuilder = new();
			
			stringBuilder.Append(char.ToLowerInvariant(text[0]));

			for (int i = 1; i < text.Length; ++i)
			{
				char c = text[i];

				if (char.IsUpper(c))
				{
					stringBuilder.Append('_');
					stringBuilder.Append(char.ToLowerInvariant(c));
				}
				else
				{
					stringBuilder.Append(c);
				}
			}

			return stringBuilder.ToString();
		}

		private static Dictionary<object, object> CreateFaceDetector(FacerecService service)
		{
			return new Dictionary<object, object>
			{
				{ "unit_type", "FACE_DETECTOR" },
				{ "modification", "ssyv_light" }
			};
		}

		private static Dictionary<object, object> CreateFaceFitter(FacerecService service)
		{
			return new Dictionary<object, object>
			{
				{ "unit_type", "FACE_FITTER" },
				{ "modification", "fda" }
			};
		}

		private static void GetScreenResolution(ref int width, ref int height)
		{
			const int padding = 100;

			if (OperatingSystem.IsWindows())
			{
				width = GetSystemMetrics(SM_CXSCREEN);
				height = GetSystemMetrics(SM_CYSCREEN);
			}
			else
			{
				try
				{
					Process process = new Process();
					process.StartInfo.FileName = "xdpyinfo";
					process.StartInfo.UseShellExecute = false;
					process.StartInfo.RedirectStandardOutput = true;
					process.StartInfo.CreateNoWindow = true;

					process.Start();

					string output = process.StandardOutput.ReadToEnd();
					process.WaitForExit();

					var match = Regex.Match(output, @"dimensions:\s+(\d+)x(\d+)");

					if (match.Success)
					{
						width = int.Parse(match.Groups[1].Value);
						height = int.Parse(match.Groups[2].Value);
					}
					else
					{
						Console.WriteLine("Could not determine screen resolution.");
					}
				}
				catch (Exception ex)
				{
					Console.WriteLine($"Error: {ex.Message}");
				}
			}

			width -= padding;
			height -= padding;
		}

		private static void FitResize(int width, int height, ref Mat image, float widthToHeight)
		{
			const int step = 100;

			if (image.Cols > width)
			{
				int newHeight = height;

				while (newHeight >= height)
				{
					width -= step;

					newHeight = (int)(width / widthToHeight);
				}

				image = image.Resize(new Size(width, newHeight));
			}
			else
			{
				int newWidth = width;

				while (newWidth >= width)
				{
					height -= step;

					newWidth = (int)(height * widthToHeight);
				}

				image = image.Resize(new Size(newWidth, height));
			}
		}
	}
}
