/**
	\file csharp_demo/video_recognition_demo/src/Worker.cs
*/


using System;
using System.Threading;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using VDT.FaceRecognition.SDK;


// data for each person that currently in process
public class FaceData
{
	public RawSample sample;
	public bool lost;
	public bool weak;
	public int frame_id;
	public TimeSpan lost_time;
	public int match_database_index;

	public float draw_multilier;
	public FaceData()
	{
		lost = true;
		match_database_index = -1;
		draw_multilier = 1;
	}
};	

class Worker: IDisposable
{
	public Mutex _draw_image_mutex = new Mutex();
	public OpenCvSharp.Mat _draw_image;

	public VideoWorker _video_worker;
	public ImageAndDepthSource _source;
	public int _stream_id;

	public Database _database;

	public int _tracking_callback_id;
	public int _tracking_lost_callback_id;
	public int _match_found_callback_id;


	volatile bool _shutdown;
	public Thread _camera_thread;
	public Thread _drawing_thread;

	public double _frame_min_delay_ms;

	public Queue<Tuple<int, ImageAndDepth> > _frames = new Queue<Tuple<int, ImageAndDepth> >();

	public ImageAndDepth _last_depth_frame = new ImageAndDepth();
	public Mutex _frames_mutex = new Mutex();

	public DrawingData _drawing_data = new DrawingData();
	public Mutex _drawing_data_mutex = new Mutex();

	// constants for drawing
	public const int thumbnail_size = 150;
	public const int draw_border = 5;
	public const int max_count_in_stripe = 6;


	public Worker(
		Database database,
		VideoWorker video_worker,
		ImageAndDepthSource source,
		int stream_id,
		Mutex draw_image_mutex,
		OpenCvSharp.Mat draw_image,
		float frame_fps_limit)
	{	
		_draw_image_mutex = draw_image_mutex;
		_draw_image = draw_image;
		_video_worker = video_worker;
		_source = source;
		_frame_min_delay_ms = 1000f / frame_fps_limit;
		_stream_id = stream_id;
		_database = database;

		// check arguments
		MAssert.Check(video_worker != null);
		MAssert.Check(source != null);
		MAssert.Check(stream_id >= 0 && stream_id < video_worker.getStreamsCount());

	// set callbacks arguments
		_tracking_callback_id =
			video_worker.addTrackingCallbackU(
				TrackingCallback,
				this);

		_tracking_lost_callback_id =
			video_worker.addTrackingLostCallbackU(
					TrackingLostCallback,
					this);

		_match_found_callback_id =
			video_worker.addMatchFoundCallbackU(
				MatchFoundCallback,
				this);

		// start threads
		_shutdown = false;

		_drawing_thread = new Thread(DrawingThreadFunc);
		_camera_thread = new Thread(CameraThreadFunc);
		_drawing_thread.Start();
		_camera_thread.Start();

	}

	public void Dispose()
	{
		_video_worker.removeTrackingCallback(_tracking_callback_id);
		_video_worker.removeTrackingLostCallback(_tracking_lost_callback_id);
		_video_worker.removeMatchFoundCallback(_match_found_callback_id);

		// stop threads
		_shutdown = true;

		_camera_thread.Join();
		_drawing_thread.Join();

		GC.SuppressFinalize(this);
	}

	~Worker()
	{
		Dispose();
	}


	// data for drawing
	public class DrawingData
	{
		public bool updated;

		public OpenCvSharp.Mat frame; 
		public OpenCvSharp.Mat depth;
		public int frame_id;

		// map<track_id , face>
		public Dictionary<int, FaceData> faces;
		public DrawingData()
		{
			updated = false;
			faces = new Dictionary<int, FaceData>();
		}
	};

	// tracking callback function
	//  userdata is supposed to be pointer to this Worker
	private static
	void TrackingCallback(
		VideoWorker.TrackingCallbackData data,
		Object userdata)
	{
		// check arguments
		MAssert.Check(data != null);
		MAssert.Check(userdata != null);

		int stream_id = (int)data.stream_id;
		int frame_id = (int)data.frame_id;
		RawSample[] samples = data.samples;
		bool[] samples_weak = data.samples_weak;
		float[] samples_quality = data.samples_quality;

		MAssert.Check(samples.Length == samples_weak.Length);
		MAssert.Check(samples.Length == samples_quality.Length);

		// userdata is supposed to be pointer to this Worker
		// so cast the pointer
		Worker worker = (Worker)(userdata);

		// we care only about the worker._stream_id source
		// so just ignore any others
		if(stream_id != worker._stream_id)
			return;

		// get the frame with frame_id id
		ImageAndDepth frame = new ImageAndDepth();

		{
			worker._frames_mutex.WaitOne();

			// searching in worker._frames
			for(;;)
			{
				// there already must be some frames
				MAssert.Check(worker._frames.Count > 0);

				// this frame_id can't be from future
				MAssert.Check(worker._frames.Peek().Item1 <= frame_id);

				if(worker._frames.Peek().Item1 == frame_id)
				{
					// we found it
					frame = worker._frames.Dequeue().Item2;
					break;
				}
				else
				{
					// this (i.e. the worker._frames.front() ) frame was skipeed by tracking
					Console.WriteLine("skiped {0}:{1}", stream_id, worker._frames.Dequeue().Item1);
				}
			}
			worker._frames_mutex.ReleaseMutex();
		}

		// update the data
		{
			worker._drawing_data_mutex.WaitOne();

			// frame
			
			worker._drawing_data.frame = frame.image;
			worker._drawing_data.depth = frame.depth;
			worker._drawing_data.frame_id = frame_id;
			worker._drawing_data.updated = true;

			// and samples
			for(int i = 0; i < samples.Length; ++i)
			{
				if(!worker._drawing_data.faces.ContainsKey(samples[i].getID()))
					worker._drawing_data.faces[samples[i].getID()] = new FaceData();
				FaceData face = worker._drawing_data.faces[samples[i].getID()];
				face.frame_id = samples[i].getFrameID();  // it can differ from the frame_id
				face.lost = false;
				face.weak = samples_weak[i];
				face.sample = samples[i];
			}
			worker._drawing_data_mutex.ReleaseMutex();
		}
	}


	// tracking lost callback function
	//  userdata is supposed to be pointer to this Worker
	private static
	void TrackingLostCallback(
		VideoWorker.TrackingLostCallbackData data,
		Object userdata)
	{
		MAssert.Check(data != null);
		MAssert.Check(userdata != null);

		int stream_id = (int)data.stream_id;
		int track_id = (int)data.track_id;
		int first_frame_id = (int)data.first_frame_id;
		int last_frame_id = (int)data.last_frame_id;
		float best_quality = (float)data.best_quality;
		int best_quality_frame_id = (int)data.best_quality_frame_id;
		RawSample best_quality_sample = data.best_quality_sample;
		Template best_quality_templ = data.best_quality_templ;

		// userdata is supposed to be pointer to this Worker
		// so cast the pointer
		Worker worker = (Worker)(userdata);

		// we care only about the worker._stream_id source
		// so just ignore any others
		if(stream_id != worker._stream_id)
			return;

		// here we just remember the time when the face was lost
		// but the real purpose of this callback if to store faces
		// for futher matching or processing

		{
			worker._drawing_data_mutex.WaitOne();

			FaceData face = worker._drawing_data.faces[track_id];

			MAssert.Check(!face.lost);

			face.lost = true;
			face.lost_time = new TimeSpan(DateTime.Now.Ticks);
			if (best_quality_sample != null)
			{
				face.sample = best_quality_sample;
			}

			worker._drawing_data_mutex.ReleaseMutex();
		}
	}


	// match found callback function
	//  userdata is supposed to be pointer to this Worker
	private static
	void MatchFoundCallback(
		VideoWorker.MatchFoundCallbackData data,
		Object userdata)
	{
		MAssert.Check(data != null);
		MAssert.Check(userdata != null);

		int stream_id = (int)data.stream_id;
		int frame_id = (int)data.frame_id;
		RawSample sample = data.sample;
		float quality = (float)data.quality;
		Template templ = data.templ;
		VideoWorker.SearchResult[] search_results = data.search_results;

		// userdata is supposed to be pointer to this Worker
		// so cast the pointer
		Worker worker = (Worker)(userdata);

		// we care only about the worker._stream_id source
		// so just ignore any others
		if(stream_id != worker._stream_id)
			return;

		MAssert.Check(sample != null);
		MAssert.Check(templ != null);
		MAssert.Check(search_results.Length > 0);

		// just print distances in the console
		Console.WriteLine("stream {0} match track {1} : ", stream_id, sample.getID());

		for(int i = 0; i < search_results.Length; ++i)
		{
			ulong element_id = search_results[i].element_id;

			if(element_id == (ulong)VideoWorker.MATCH_NOT_FOUND_ID)
			{
				MAssert.Check(i == 0);
				Console.WriteLine("  {0}: MATCH NOT FOUND", i);
			}
			else
			{
				MAssert.Check(element_id < (UInt64)worker._database.names.Count);
				Console.WriteLine("  {0}:  with '{1}' distance: {2}",
					i,
					worker._database.names[(int)element_id],
					search_results[i].match_result.distance);
			}
		}
		Console.WriteLine("");

		ulong match_element_id = search_results[0].element_id;

		if(match_element_id != (ulong)VideoWorker.MATCH_NOT_FOUND_ID)
		{
			MAssert.Check((int)match_element_id < worker._database.thumbnails.Count);

			// set the match info in the worker._drawing_data.faces
			worker._drawing_data_mutex.WaitOne();

			FaceData face = worker._drawing_data.faces[sample.getID()];

			MAssert.Check(!face.lost);

			face.match_database_index = (int)match_element_id;

			worker._drawing_data_mutex.ReleaseMutex();
		}
	}


	// here we grab frames from the sources
	// and put them in a VideoWorker
	void CameraThreadFunc()
	{
		try
		{
			TimeSpan prev_capture_time = new TimeSpan(DateTime.Now.Ticks);

			for(;;)
			{
				// check for stop
				if(_shutdown)
					break;

				// sleep some time to get stable frequency
				if (_frame_min_delay_ms > 0)
				{
					TimeSpan now = new TimeSpan(DateTime.Now.Ticks);

					// 40ms - for 24 fps
					int sleep_time =
						(int)(_frame_min_delay_ms - (now - prev_capture_time).TotalMilliseconds);

					if(sleep_time > 0)
						Thread.Sleep(sleep_time);

					prev_capture_time = new TimeSpan(DateTime.Now.Ticks);
				}


				ImageAndDepth frame = _source.Get();

				if (!frame.depth.Empty())
				{
					_video_worker.addDepthFrame(frame.make_dmr(), _stream_id, (long)frame.depth_timestamp_microsec);

					_last_depth_frame = frame;
				}
				// put a frame in the VideoWorker
				if (!frame.image.Empty())
				{
					// get the frame
					byte[] data = new byte[frame.image.Total() * frame.image.Type().Channels];
					Marshal.Copy(frame.image.DataStart, data, 0, (int)data.Length);
					RawImage raw_image = new RawImage(frame.image.Width, frame.image.Height, RawImage.Format.FORMAT_BGR, data);

					_frames_mutex.WaitOne();

					int frame_id = _video_worker.addVideoFrame(raw_image, _stream_id, (long)frame.image_timestamp_microsec);

					if (frame.depth.Empty())
						frame.depth = _last_depth_frame.depth;

					// and store it here for further drawing
					_frames.Enqueue(new Tuple<int,ImageAndDepth>(frame_id, frame));
					_frames_mutex.ReleaseMutex();
				}
			}
		}
		catch(Exception e)
		{
			Console.WriteLine("\n WCameraThreadFunc exception: '{0}'\n", e.ToString());
		}
	}

	// draw results
	private static
	OpenCvSharp.Mat Draw(
		DrawingData data,
		Database database)
	{
		// drawing params of stripe with current faces
		int stripe_width = thumbnail_size * 2 + draw_border * 2;

		int stripe_height =
			(thumbnail_size + draw_border) * max_count_in_stripe - draw_border;
		// image for draw the frame and the stripe
		OpenCvSharp.Mat result = new OpenCvSharp.Mat(
			Math.Max(data.frame.Rows + data.depth.Rows, stripe_height),
			Math.Max(data.frame.Cols, data.depth.Cols) + stripe_width,
			OpenCvSharp.MatType.CV_8UC3,
			OpenCvSharp.Scalar.All(0));

		// copy the frame
		int frame_y_offset = (result.Rows - data.frame.Rows - data.depth.Rows) / 2;
		int depth_y_offset = frame_y_offset + data.frame.Rows;
		
		data.frame.CopyTo(
			result[
				new OpenCvSharp.Rect(
					0,
					frame_y_offset,
					data.frame.Cols,
					data.frame.Rows)]);

		for(int i = 0; i < data.depth.Rows; ++i)
		{
			unsafe
			{
				UInt16* src_ptr = (UInt16*) data.depth.Ptr(i);
				byte* dst_ptr = (byte*) result.Ptr(depth_y_offset + i);

				for(int j = 0; j < data.depth.Cols; ++j, ++src_ptr, dst_ptr += 3)
				{
					UInt16 depth = *src_ptr;
					byte* bgr = dst_ptr;

					bgr[0] = bgr[1] = bgr[2] = 0;

					if(depth == 0)
						continue;

					if(depth < 256)
						bgr[2] = (byte) ( 256 - depth );

					if(depth < 256 * 2)
						bgr[1] = (byte) ( depth / 2 );
					else
						bgr[1] = (byte) ( 255 );

					if(depth < 256 * 4)
						bgr[0] = (byte) ( depth / 4 );
					else
						bgr[0] = (byte) ( 255 );
				}
			}
		}

		// clone faces data for random access
		List<Tuple<int, FaceData>> faces = new List<Tuple<int, FaceData>>();
		foreach(KeyValuePair<int, FaceData> pair in data.faces)
		{
			faces.Add(new Tuple<int, FaceData>(pair.Key, pair.Value));
		}



		// make order with recognized first
		// pair<match_database_index, index in faces>
		List<Tuple<int, int>> order = new List<Tuple<int, int>>();

		for(int i = 0; i < faces.Count; ++i)
		{
			order.Add(new Tuple<int,int>(faces[i].Item2.match_database_index, i));
		}

		order.Sort();

		// draw alive faces
		for(int order_i = 0; order_i < order.Count; ++order_i)
		{
			FaceData face = faces[order[order_i].Item2].Item2;

			// draw circles of faces appeared on this frame
			if(face.frame_id == data.frame_id && !face.lost)
			{
				// get points
				List<Point> points = face.sample.getLandmarks();

				// compute center
				OpenCvSharp.Point2f center = new OpenCvSharp.Point2f(0, 0);

				for(int j = 0; j < points.Count; ++j)
				{
					center.X += points[j].x;
					center.Y += points[j].y;
				}
				center *= 1.0 / points.Count;


				// compute radius
				double radius = 0;

				for(int j = 0; j < points.Count; ++j)
					radius += OpenCvSharp.Point2f.Distance(new OpenCvSharp.Point2f(points[j].x, points[j].y), center);

				radius *= 1.5 / points.Count;

				radius *= 2;

				// choose color
				OpenCvSharp.Scalar color =
					face.match_database_index < 0 ?
						new OpenCvSharp.Scalar(0, 0, 255) :  // red color for unrecognized
						new OpenCvSharp.Scalar(0, 255, 0);   // green color for recognizerd

				for (int k = 0; k < (data.depth.Empty() ? 1 : 2); ++k)
				{
					int y_offset = (k == 0 ? frame_y_offset : depth_y_offset);

					// dashed circle for weak face samples
					if(face.weak)
					{
						// draw dashed cirle for weak samples
						int n = 8;
						for(int i = 0; i < n; ++i)
							OpenCvSharp.Cv2.Ellipse(
								result,
								(OpenCvSharp.Point)(center + new OpenCvSharp.Point2f(0f, y_offset)),
								new OpenCvSharp.Size(radius, radius),
								(face.frame_id * 2) % 360,
								(i * 2 + 0) * 180 / n,
								(i * 2 + 1) * 180 / n,
								color,
								3,
								OpenCvSharp.LineTypes.AntiAlias);
					}
					else
					{
						OpenCvSharp.Cv2.Circle(
							result,
							(OpenCvSharp.Point)(center + new OpenCvSharp.Point2f(0f, y_offset)),
							(int)radius,
							color,
							3,
							OpenCvSharp.LineTypes.AntiAlias);
					}
				}
			}

			// no - draw the stripe
			if(order_i < max_count_in_stripe)
			{
				// place for thumbnail from the frame
				OpenCvSharp.Rect sample_rect = new OpenCvSharp.Rect(
					data.frame.Cols + draw_border,
					(thumbnail_size + draw_border) * order_i,
					thumbnail_size,
					thumbnail_size);

				// place for thumbnail from the database
				OpenCvSharp.Rect match_rect = new OpenCvSharp.Rect(
					data.frame.Cols + draw_border * 2 + thumbnail_size,
					(thumbnail_size + draw_border) * order_i,
					thumbnail_size,
					thumbnail_size);

				// make thumbnail from the frame
				Database.makeThumbnail(face.sample).CopyTo(result[sample_rect]);

				// fade if image is lost
				if(face.draw_multilier < 1)
					result[sample_rect] *= face.draw_multilier;

				if(face.match_database_index < 0)
				{
					// gray color for unrecognized persons
					result[match_rect].SetTo(128 * face.draw_multilier);
				}
				else
				{
					// thumbnail from the database for recognized persons
					database.thumbnails[face.match_database_index].CopyTo(result[match_rect]);

					// fade if image is lost
					if(face.draw_multilier < 1)
						result[match_rect] *= face.draw_multilier;
				}
			}
		}

		return result;
	}

	// here we draw results
	void DrawingThreadFunc()
	{
		for(;;)
		{
			// check for stop
			if(_shutdown)
				break;

			// we dont need to redraw too often
			Thread.Sleep(20);

			// get the data
			DrawingData data = new DrawingData();

			{
				_drawing_data_mutex.WaitOne();
				// remove lost faces after 5 seconds
				{
					TimeSpan now = new TimeSpan(DateTime.Now.Ticks);

					const float timeout = 5000;  // ms - i.e. 5 seconds
					
					List<int> removingKeys = new List<int>();
					foreach (KeyValuePair<int, FaceData> entry in _drawing_data.faces)
					{
						FaceData face = entry.Value;
						if(!face.lost)
							continue;

						_drawing_data.updated = true;

						float delay = (float)(now - face.lost_time).TotalMilliseconds;

						if(delay < timeout)
							face.draw_multilier = 1 - delay / timeout;
						else
							removingKeys.Add(entry.Key);
					}
					
					for(int i_key = 0; i_key < removingKeys.Count; i_key++)
						_drawing_data.faces.Remove(removingKeys[i_key]);
				}

				data = _drawing_data;
				
				_drawing_data.updated = false;
				_drawing_data_mutex.ReleaseMutex();
			}

			// draw
			if(data.frame == null)
				continue;
			OpenCvSharp.Mat drawed = Draw(
				data,
				_database);

			// and store it (it will be used in imshow in the main)
			{
				_draw_image_mutex.WaitOne();
				_draw_image = drawed;
				_draw_image_mutex.ReleaseMutex();
			}
		}
	}

};
