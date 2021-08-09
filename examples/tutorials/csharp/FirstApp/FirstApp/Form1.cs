using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Emgu.CV;
using Emgu.CV.Structure;
using System.Runtime.InteropServices;
using VDT.FaceRecognition.SDK;

namespace FirstApp
{
    public partial class Form1 : Form
    {
        private int camera_id = 0;
        private VideoCapture camera;
        private String faceSDKRootDir = "../../../../../../../../";
        private Capturer capturer;

        private void GetFrame(object sender, EventArgs e)
        {
            Image<Bgr, byte> image = camera.QueryFrame().ToImage<Bgr, byte>();
            drawDetections(image);
            pictureBox1.Image = image.ToBitmap();
        }


        private void drawDetections(Image<Bgr, byte> image)
        {
            Mat frame_m = image.Mat.Clone();
            byte[] data = new byte[frame_m.Total.ToInt32() * frame_m.NumberOfChannels];
            Marshal.Copy(frame_m.DataPointer, data, 0, (int)data.Length);
            RawImage ri_frame = new RawImage(frame_m.Width, frame_m.Height, RawImage.Format.FORMAT_BGR, data);
            List<RawSample> detected = capturer.capture(ri_frame);
            foreach (RawSample sample in detected)
            {
                RawSample.Rectangle rect = sample.getRectangle();
                image.Draw(new Rectangle((int)rect.x,
                                         (int)rect.y,
                                         (int)rect.width,
                                         (int)rect.height),
                           new Bgr(0, 255, 0),
                           2);
            }
        }


        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load_1(object sender, EventArgs e)
        {
            camera = new VideoCapture(camera_id);
            if (camera.IsOpened)
            {
                FacerecService service = FacerecService.createService(faceSDKRootDir + "\\conf\\facerec", "");
                FacerecService.Config capturerConfig = new FacerecService.Config("fda_tracker_capturer_uld.xml");
                capturer = service.createCapturer(capturerConfig);
                Application.Idle += GetFrame;
            }
            else
            {
                MessageBox.Show("Camera " + camera_id + " not opened!", "Camera error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
        }
    }
}
