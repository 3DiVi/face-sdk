/**
	\file video_recognition_demo/src/Database.h
*/

using System;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using VDT.FaceRecognition.SDK;

// database
public class Database
{
	public List<VideoWorker.DatabaseElement> vwElements;
	public List<RawSample> samples;
	public List<OpenCvSharp.Mat> thumbnails;
	public List<string> names;

	// make a thumbnail of a sample
	public static OpenCvSharp.Mat makeThumbnail(
		RawSample sample,
		string name = "")
	{
		int thumbnail_size = Worker.thumbnail_size;

		// buffer for the cutted image
		MemoryStream stream = new MemoryStream();

		// make a cut in bmp format
		// so we don't waste time for encode/decode image
		// just copying it few times, which is irrelevant
		sample.cutFaceImage(
			stream,
			RawSample.ImageFormat.IMAGE_FORMAT_BMP,
			RawSample.FaceCutType.FACE_CUT_BASE);

		OpenCvSharp.Mat temp = OpenCvSharp.Mat.ImDecode(stream.ToArray(), OpenCvSharp.ImreadModes.Color);

		// so we got an image

		// check it
		MAssert.Check(!temp.Empty());
		MAssert.Check(temp.Type() == OpenCvSharp.MatType.CV_8UC3);


		// and resize to the thumbnail_size

		OpenCvSharp.Rect resRect;

		if(temp.Rows >= temp.Cols)
		{
			resRect.Height = thumbnail_size;
			resRect.Width = temp.Cols * thumbnail_size / temp.Rows;
		}
		else
		{
			resRect.Width = thumbnail_size;
			resRect.Height = temp.Rows * thumbnail_size / temp.Cols;
		}

		resRect.X = (thumbnail_size - resRect.Width) / 2;
		resRect.Y = (thumbnail_size - resRect.Height) / 2;

		OpenCvSharp.Mat result = new OpenCvSharp.Mat(
			thumbnail_size,
			thumbnail_size,
			OpenCvSharp.MatType.CV_8UC3,
			OpenCvSharp.Scalar.All(0));

		OpenCvSharp.Cv2.Resize(
			temp,
			result[resRect],
			resRect.Size);

		if(!string.IsNullOrEmpty(name))
		{
			result[new OpenCvSharp.Rect(0, result.Rows - 27, result.Cols, 27)] = result.RowRange(result.Rows - 27, result.Rows) * 0.5f;

			OpenCvSharp.Cv2.PutText(
				result,
				name,
				new OpenCvSharp.Point(0, result.Rows - 7),
				OpenCvSharp.HersheyFonts.HersheyDuplex,
				0.7,
				OpenCvSharp.Scalar.All(255),
				1,
				OpenCvSharp.LineTypes.AntiAlias);
		}

		return result;

	}

	// create the database
	public Database(
		string databaseDirPath,
		Recognizer recognizer,
		Capturer capturer,
		float distanceThreshold)
	{
		vwElements = new List<VideoWorker.DatabaseElement>();
		samples = new List<RawSample>();
		thumbnails = new List<OpenCvSharp.Mat>();
		names = new List<string>();
		// check paths
		MAssert.Check(Directory.Exists(databaseDirPath), "database not found");

		// get directory content
		List<string> path_l1 = new List<string>(Directory.EnumerateDirectories(databaseDirPath));
	
		// check every element in that directory

		ulong element_id_counter = 0;

		for(int il1 = 0; il1 < path_l1.Count; ++il1)
		{
			// ignore files
			if(!Directory.Exists(path_l1[il1]))
				continue;
			// so path_l1[il1] is supposed to be the path to the person directory

			// get files inside i
			List<string> path_l2 = new List<string>(Directory.EnumerateFiles(path_l1[il1]));
			string name = string.Empty;

			// search for the name.txt file
			
			for(int il2 = 0; il2 < path_l2.Count; ++il2)
			{
				if(Path.GetFileName(path_l2[il2]) == "name.txt")
				{
					// put file content in the name

					using (StreamReader sr = new StreamReader(path_l2[il2]))
					{
						name = sr.ReadToEnd();
					}
				}
			}

			// try to open each file as an image
			for(int il2 = 0; il2 < path_l2.Count; ++il2)
			{
				if(Path.GetFileName(path_l2[il2]) == "name.txt")
					continue;

				Console.WriteLine("processing '{0}' name: '{1}'", path_l2[il2], name);

				// read image with opencv

				OpenCvSharp.Mat readed_image = OpenCvSharp.Cv2.ImRead(path_l2[il2]);

				if(readed_image.Empty() || readed_image.Type() != OpenCvSharp.MatType.CV_8UC3)
				{
					Console.WriteLine("\n\nWARNING: can't read image '{0}'\n\n", path_l2[il2]);
					continue;
				}

				byte[] data = new byte[readed_image.Total() * readed_image.Type().Channels];
				Marshal.Copy(readed_image.DataStart, data, 0, (int)data.Length);
				RawImage image = new RawImage(readed_image.Width, readed_image.Height, RawImage.Format.FORMAT_BGR, data);

				// capture the face
				List<RawSample> capturedSamples = capturer.capture(image);

				if(capturedSamples.Count != 1)
				{
					Console.WriteLine("\n\nWARNING: detected {0} faces on '{1}' image instead of one, image ignored \n\n", capturedSamples.Count, path_l2[il2]);
					continue;
				}

				RawSample sample = capturedSamples[0];

				// make template
				Template templ = recognizer.processing(sample);

				// prepare data for VideoWorker
				VideoWorker.DatabaseElement vwElement = new VideoWorker.DatabaseElement(element_id_counter++, (ulong)il1, templ, distanceThreshold);

				vwElements.Add(vwElement);

				samples.Add(sample);

				thumbnails.Add(makeThumbnail(sample, name));

				names.Add(name);
			}

		}

		MAssert.Check((int)element_id_counter == vwElements.Count);
		MAssert.Check((int)element_id_counter == samples.Count);
		MAssert.Check((int)element_id_counter == thumbnails.Count);
		MAssert.Check((int)element_id_counter == names.Count);
	}
};









/**
	\file video_recognition_demo/src/Database.cpp
*/
