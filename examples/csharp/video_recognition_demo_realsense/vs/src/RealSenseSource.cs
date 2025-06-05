/**
	\file csharp_demo/video_recognition_demo/src/RealSenseSource.cs
*/

#if WITH_REALSENSE

using System;

using Intel.RealSense;

using VDT.FaceRecognition.SDK;

class RealSenseSource : ImageAndDepthSource
{
	Pipeline pipeline;

	Align align_to_color;

	private float depth_scale = 0;

	private long prev_depth_frame_number = -1;
	private long prev_video_frame_number = -1;

	public RealSenseSource()
	{
		using (Context ctx = new Context())
		{
			DeviceList devices = ctx.QueryDevices();

			Console.WriteLine("There are {0} connected RealSense devices.", devices.Count);

			MAssert.Check(devices.Count > 0, "there is no RealSense devices");

			for(int i = 0; i < devices.Count; ++i)
			{
				Console.WriteLine(
					"Device {0}: name {1}, serial {2}, firmware version: {3}",
					i,
					devices[i].Info[CameraInfo.Name],
					devices[i].Info[CameraInfo.SerialNumber],
					devices[i].Info[CameraInfo.FirmwareVersion]);
			}
		}

		Config config = new Config();
		config.EnableStream(Stream.Depth, 640, 480, Format.Z16);
		config.EnableStream(Stream.Color, 640, 480, Format.Bgr8);

		pipeline = new Pipeline();

		PipelineProfile pipeline_profile = pipeline.Start(config);

		depth_scale = pipeline_profile.Device.Sensors[0].DepthScale * 1000;

		MAssert.Check(depth_scale > 0);

		align_to_color = new Align(Stream.Color);
	}


	public override ImageAndDepth Get()
	{
		ImageAndDepth res = new ImageAndDepth();

		using(FrameSet frameset = pipeline.WaitForFrames())
		{
			DepthFrame depth_frame = frameset.FirstOrDefault<DepthFrame>(Stream.Depth, Format.Z16).DisposeWith(frameset);

			if(depth_frame != null && ((long)depth_frame.Number) > prev_depth_frame_number)
			{
				prev_depth_frame_number = (long) depth_frame.Number;

				FrameSet registered = align_to_color.Process(frameset).As<FrameSet>().DisposeWith(frameset);

				depth_frame = registered.FirstOrDefault<DepthFrame>(Stream.Depth, Format.Z16).DisposeWith(frameset);

				MAssert.Check(depth_frame != null);
				MAssert.Check(depth_frame.BitsPerPixel == 16);
				MAssert.Check(depth_frame.Stride >= depth_frame.Width * 2);

				float[] fov = depth_frame.Profile.As<VideoStreamProfile>().GetIntrinsics().FOV;

				res.depth_opts.horizontal_fov = fov[0];
				res.depth_opts.vertical_fov = fov[1];
				res.depth_opts.depth_unit_in_millimeters = depth_scale;


				res.depth_opts.depth_map_2_image_offset_x = 0;
				res.depth_opts.depth_map_2_image_offset_y = 0;
				res.depth_opts.depth_map_2_image_scale_x = 1;
				res.depth_opts.depth_map_2_image_scale_y = 1;

				res.depth_opts.depth_map_rows = depth_frame.Height;
				res.depth_opts.depth_map_cols = depth_frame.Width;

				res.depth_timestamp_microsec = (ulong) depth_frame.Timestamp * 1000;

				res.depth = new OpenCvSharp.Mat<ushort>(
					depth_frame.Height,
					depth_frame.Width,
					depth_frame.Data,
					depth_frame.Stride).Clone();
			}


			VideoFrame video_frame = frameset.FirstOrDefault<VideoFrame>(Stream.Color, Format.Bgr8).DisposeWith(frameset);

			if(video_frame != null && ((long)video_frame.Number > prev_video_frame_number))
			{
				prev_video_frame_number = (long) video_frame.Number;

				MAssert.Check(video_frame.BitsPerPixel == 24);
				MAssert.Check(video_frame.Stride >= video_frame.Width * 3);

				res.image = new OpenCvSharp.Mat(
					video_frame.Height,
					video_frame.Width,
					OpenCvSharp.MatType.CV_8UC3,
					video_frame.Data,
					video_frame.Stride).Clone();

				res.image_timestamp_microsec = (ulong) video_frame.Timestamp * 1000;
			}
		}

		return res;
	}

};

#endif