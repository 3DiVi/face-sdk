/**
	\file csharp_demo/video_recognition_demo/src/ImageAndDepthSource.cs
*/

using System;

using VDT.FaceRecognition.SDK;


public class ImageAndDepth
{
	public OpenCvSharp.Mat image;
	public OpenCvSharp.MatOfUShort depth;

	public UInt64 image_timestamp_microsec;
	public UInt64 depth_timestamp_microsec;

	public DepthMapRaw depth_opts;  // all but depth_data and depth_data_stride_in_bytes must be set

	public DepthMapRaw make_dmr()
	{
		MAssert.Check(!depth.Empty());

		DepthMapRaw r = depth_opts;
		r.depth_data_ptr = depth.Data;
		r.depth_data_stride_in_bytes = (int)depth.Step();

		return r;
	}
	public ImageAndDepth()
	{
		image = new OpenCvSharp.Mat();
		depth = new OpenCvSharp.MatOfUShort();
		depth_opts = new DepthMapRaw();
	}
};

abstract class ImageAndDepthSource
{
	public abstract ImageAndDepth Get();
};
