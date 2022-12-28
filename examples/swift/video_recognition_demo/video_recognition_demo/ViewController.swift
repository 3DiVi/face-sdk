import UIKit
import AVFoundation
import CoreImage
import Vision



let service = FacerecService()
let vw_params = Params()


func testTryCatch() -> VideoWorker{
    do{
        vw_params.video_worker_config = Config("video_worker_fdatracker_blf_fda.xml")
        vw_params.recognizer_config = Config("method12v30_recognizer.xml")
        vw_params.streams_count = 1
        vw_params.processing_threads_count = 1
        vw_params.matching_threads_count = 1
        vw_params.emotions_estimation_threads_count = 1
        vw_params.age_gender_estimation_threads_count = 1
        
        return try CheckException{service.createVideoWorker(vw_params)}
    }catch{
        fatalError("Code: \((error as NSError).domain)")
    }
}

let video_worker = testTryCatch()
let capturer = service.createCapturer(Config("common_capturer_blf_fda_front.xml"))
let recognizer = service.createRecognizer(Config("method12v30_recognizer.xml"), true, true, false)
var arr: Array<RawImage> = []


class ViewController: UIViewController, AVCaptureVideoDataOutputSampleBufferDelegate {
    
    @IBOutlet weak var labelText: UILabel!
    @IBOutlet weak var textFieldName: UITextField!
    @IBOutlet weak var cutImage: UIImageView!
    @IBOutlet weak var imageView: UIImageView!
    @IBOutlet var SaveButton: [UIButton]!
    
    private let captureSession = AVCaptureSession()
    private lazy var previewLayer = AVCaptureVideoPreviewLayer(session: self.captureSession)
    private let videoDataOutput = AVCaptureVideoDataOutput()
    
    let database = DataBase(recognizer: recognizer ,capturer: capturer, distance_threshold: 7000)
    var raw_image = RawImage()
    var isAgeGender = true
    var isEmotions = true
    
    var editText = true
    var tic = 0
    
    var img = UIImage()
    var strem_id = 0
    var isCutImage = true
    
    var frame_id_int = 0
    var frame_id_match_found = 0
    var width: CGFloat = 0.0
    var height: CGFloat = 0.0

    let queue = DispatchQueue.global(qos: .utility)
        
    override func viewDidLoad() {
        super.viewDidLoad()
        self.captureSession.sessionPreset = AVCaptureSession.Preset.vga640x480
        
        self.addCameraInput()
        self.showCameraFeed()
        self.getCameraFrames()
        
        self.labelText.backgroundColor = UIColor.white
        
        do {
            try CheckException{video_worker.setDatabase(self.database.vm_elements)}
        }catch{
            print("Code: \((error as NSError).domain)")
        }
        
        video_worker.addHandlerCallback(UserClass(userdata: bridgeRetained(obj: self)))
        
        self.captureSession.startRunning()
        self.width = self.view.frame.width
        self.height = self.view.frame.height
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        self.previewLayer.frame = self.view.frame
    }
    
    // Convert CIImage to UIImage
    func convert(sampleBuffer: CMSampleBuffer) -> UIImage {
        let imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer)!
        let ciimage = CIImage(cvPixelBuffer: imageBuffer)
        let context = CIContext(options: nil)
        let cgImage = context.createCGImage(ciimage, from: ciimage.extent)!
        let image = UIImage(cgImage: cgImage)
        return image
    }

    @IBAction func HideClav(_ sender: Any) {
        self.view.endEditing(true)
    }
    
    func captureOutput(
        _ output: AVCaptureOutput,
        didOutput sampleBuffer: CMSampleBuffer,
        from connection: AVCaptureConnection) {
        
        do{
            if !self.editText{
                self.tic += 1
                if self.tic == 50{
                    self.tic = 0
                    self.editText = true
                }
            }
            self.img = self.convert(sampleBuffer: sampleBuffer)

            self.raw_image = try CheckException{return RawImage(sampleBuffer, Format.FORMAT_RGB.rawValue)}
                        
            self.frame_id_int = Int(try CheckException{return video_worker.addVideoFrame(self.raw_image, Int32(self.strem_id))})
            
            try CheckException{video_worker.checkException()}
        }catch{
            print("Code: \((error as NSError).domain)")
        }
    }
    
    private func addCameraInput() {
        guard let device = AVCaptureDevice.DiscoverySession(
                deviceTypes: [.builtInWideAngleCamera, .builtInDualCamera],
                mediaType: .video,
                position: .front).devices.last else {
                fatalError("No back camera device found, please make sure to run SimpleLaneDetection in an iOS device and not a simulator")
        }
        let cameraInput = try! AVCaptureDeviceInput(device: device)
        self.captureSession.addInput(cameraInput)
    }
    
    private func showCameraFeed() {
        self.previewLayer.videoGravity = .resizeAspectFill
        self.previewLayer.frame = self.view.frame
        self.imageView.layer.addSublayer(self.previewLayer)
    }
    
    
    @IBAction func actSaveButton(_ sender: Any) {
        if self.textFieldName.text! != ""{
            let file_name = self.textFieldName.text! + ".jpg"
            if self.database.data_image.firstIndex(of: file_name) == nil{
                let samples = capturer.capture(self.raw_image) as! [RawSample]
                if samples.count == 1{
                    self.database.saveElement(sample: samples[0], image: self.img, file_name: file_name)
                    do {
                        try CheckException{video_worker.setDatabase(self.database.vm_elements)}
                    }catch{
                        print("Code: \((error as NSError).domain)")
                    }
                    self.labelText.text = "The person was added to the database"
                }
            }else{
                self.labelText.text = "This name is already in the database"
            }
        }else{
            self.labelText.text = "Need to enter a name"
        }
        self.editText = false
        self.tic = 0
    }
    
    private func getCameraFrames() {
        self.videoDataOutput.videoSettings = [(kCVPixelBufferPixelFormatTypeKey as NSString) : NSNumber(value: kCVPixelFormatType_32BGRA)] as [String : Any]
        
        self.videoDataOutput.alwaysDiscardsLateVideoFrames = true
        self.videoDataOutput.setSampleBufferDelegate(self, queue: DispatchQueue(label: "camera_frame_processing_queue"))
        self.captureSession.addOutput(self.videoDataOutput)
        guard let connection = self.videoDataOutput.connection(with: AVMediaType.video),
            connection.isVideoOrientationSupported else { return }
        connection.videoOrientation = .portrait
        connection.isVideoMirrored = true;
    }
}
