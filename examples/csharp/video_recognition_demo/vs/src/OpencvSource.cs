/**
	\file csharp_demo/video_recognition_demo/src/OpencvSource.cs
*/

using System;
using System.IO;
using VDT.FaceRecognition.SDK;

class OpencvSource : ImageAndDepthSource
{
	private OpenCvSharp.VideoCapture capturer;

	public override ImageAndDepth Get()
	{
			ImageAndDepth res = new ImageAndDepth();
			res.image = this.capturer.RetrieveMat();
			return res;
	}

	public OpencvSource(string cam_or_url)
	{
		MAssert.Check(cam_or_url != string.Empty);

		// check if cam_or_url is number
		bool stream = false;

		for(int i = 0; i < cam_or_url.Length; ++i)
			stream = stream ||
				(cam_or_url[i] < '0') ||
				(cam_or_url[i] > '9');

		if(stream)
		{
			// open stream
			Console.WriteLine("opening stream '{0}'", cam_or_url);
			capturer = new OpenCvSharp.VideoCapture(cam_or_url);
		}
		else
		{
			// convert to integer
			int cam_id = Convert.ToInt32(cam_or_url, 10);
			MAssert.Check(cam_id >= 0, "wrong webcam id");

			// open vebcam
			Console.WriteLine("opening webcam {0}", cam_id);
			capturer = new OpenCvSharp.VideoCapture(cam_id);
			MAssert.Check(capturer.IsOpened(), "webcam not opened");

			// set resolution
			capturer.Set(OpenCvSharp.VideoCaptureProperties.FrameWidth, 1280);
			capturer.Set(OpenCvSharp.VideoCaptureProperties.FrameHeight, 720);

			MAssert.Check(capturer.IsOpened(), "webcam not opened");
		}

			// sometimes first few frames can be empty even if camera is good
			// so skip few frames
			OpenCvSharp.Mat frame;
			for(int i = 0; i < 10; ++i)
				frame = capturer.RetrieveMat();
		// check first two frames
		OpenCvSharp.Mat image1 = new OpenCvSharp.Mat(), image2 = new OpenCvSharp.Mat();
		capturer.Read(image1);
		capturer.Read(image2);
		Console.WriteLine("image1 size: {0}", image1.Size());
		Console.WriteLine("image1 size: {0}", image2.Size());

		MAssert.Check(
			!image1.Empty() &&
			!image2.Empty() &&
			image1.Size() == image2.Size() &&
			image1.Type() == OpenCvSharp.MatType.CV_8UC3 &&
			image2.Type() == OpenCvSharp.MatType.CV_8UC3,
			"error opening webcam or stream");
	}
};
