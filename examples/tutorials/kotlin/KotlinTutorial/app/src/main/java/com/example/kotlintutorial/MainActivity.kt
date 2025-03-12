package com.example.kotlintutorial

import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.RectF
import android.graphics.SurfaceTexture
import android.hardware.camera2.CameraCaptureSession
import android.hardware.camera2.CameraCharacteristics
import android.hardware.camera2.CameraDevice
import android.hardware.camera2.CameraManager
import android.os.Bundle
import android.os.Handler
import android.os.HandlerThread
import android.util.Log
import android.util.Size
import android.view.Surface
import android.view.TextureView
import android.widget.ImageView
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import com.vdt.face_recognition.sdk.FacerecService
import com.vdt.face_recognition.sdk.ProcessingBlock


class MainActivity : AppCompatActivity() {
    private lateinit var service: FacerecService
    private lateinit var faceDetector: ProcessingBlock
    private lateinit var faceFitter: ProcessingBlock
    private lateinit var ageEstimator: ProcessingBlock
    private lateinit var genderEstimator: ProcessingBlock

    val paint = Paint()
    lateinit var bitmap: Bitmap
    lateinit var imageView: ImageView

    private lateinit var cameraManager: CameraManager
    lateinit var cameraDevice: CameraDevice
    lateinit var cameraCaptureSession: CameraCaptureSession

    private lateinit var handlerThread: HandlerThread
    lateinit var handler: Handler

    lateinit var textureView: TextureView
    val previewSize = Size(1280, 720)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        getPermission()

        service = FacerecService.createService(
            applicationInfo.nativeLibraryDir + "/libfacerec.so",
            applicationInfo.dataDir + "/fsdk/conf/facerec",
            applicationInfo.dataDir + "/fsdk/license"
        )

        val configDetector = service.createContext()
        configDetector["unit_type"].string = "FACE_DETECTOR"
        configDetector["modification"].string = "blf_front"

        faceDetector = service.createProcessingBlock(configDetector)

        val configFitter = service.createContext()
        configFitter["unit_type"].string = "FACE_FITTER"
        configFitter["modification"].string = "fda"

        faceFitter = service.createProcessingBlock(configFitter)

        val configAgeEstimator = service.createContext()
        configAgeEstimator["unit_type"].string = "AGE_ESTIMATOR"
        configAgeEstimator["modification"].string = "light"
        configAgeEstimator["version"].long = 2

        ageEstimator = service.createProcessingBlock(configAgeEstimator)

        val configGenderEstimator = service.createContext()
        configGenderEstimator["unit_type"].string = "GENDER_ESTIMATOR"
        configGenderEstimator["modification"].string = "light"
        configGenderEstimator["version"].long = 2

        genderEstimator = service.createProcessingBlock(configGenderEstimator)

        imageView = findViewById(R.id.imageView)
        textureView = findViewById(R.id.textureView)
        cameraManager = getSystemService(Context.CAMERA_SERVICE) as CameraManager

        handlerThread = HandlerThread("videoThread")
        handlerThread.start()
        handler = Handler(handlerThread.looper)

        textureView.surfaceTextureListener = object : TextureView.SurfaceTextureListener{
            override fun onSurfaceTextureAvailable(p0: SurfaceTexture, p1: Int, p2: Int) {
                openCamera()
            }

            override fun onSurfaceTextureUpdated(p0: SurfaceTexture) {
                bitmap = textureView.bitmap!!

                val  width = bitmap.width
                val  height = bitmap.height

                val pixels = IntArray(width * height)
                val imageData = ByteArray(width * height * 3)

                bitmap.getPixels(pixels, 0, width, 0, 0, width, height)

                for (i in pixels.indices) {
                    imageData[i * 3 + 0] = (pixels[i] shr 16).toByte()
                    imageData[i * 3 + 1] = (pixels[i] shr 8).toByte()
                    imageData[i * 3 + 2] = (pixels[i] shr 0).toByte()
                }

                val ioData = service.createContextFromFrame(imageData, width, height,
                    com.vdt.face_recognition.sdk.Context.Format.FORMAT_RGB, 0)
                faceDetector.process(ioData)
                faceFitter.process(ioData)
                ageEstimator.process(ioData)
                genderEstimator.process(ioData)

                val objects = ioData["objects"]

                val mutable = bitmap.copy(Bitmap.Config.ARGB_8888, true)

                val canvas = Canvas(mutable)

                paint.textSize = height/50f
                paint.strokeWidth = width/100f
                val indentionX = (0.01 * width).toFloat()
                val indentionY = paint.textSize
                for (i in 0..< objects.size()) {
                    val obj = objects[0]
                    val bbox = obj["bbox"]
                    val x1 = (bbox[0].double).toFloat() * width
                    val y1 = (bbox[1].double).toFloat() * height
                    val x2 = (bbox[2].double).toFloat() * width
                    val y2 = (bbox[3].double).toFloat() * height

                    paint.color = Color.GREEN
                    paint.style = Paint.Style.STROKE
                    canvas.drawRect(RectF(x1, y1, x2, y2), paint)
                    paint.style = Paint.Style.FILL
                    canvas.drawText("age: " + obj["age"].long,
                        x2 + indentionX, y1 + indentionY, paint)
                    canvas.drawText("gender: " + obj["gender"].string,
                        x2 + indentionX, y1 + 2 * indentionY, paint)
                }

                imageView.setImageBitmap(mutable)
            }

            override fun onSurfaceTextureDestroyed(p0: SurfaceTexture): Boolean {
                return false
            }

            override fun onSurfaceTextureSizeChanged(p0: SurfaceTexture, p1: Int, p2: Int) {}
        }
    }



    private fun getPermission(){
        if (ContextCompat.checkSelfPermission(this, android.Manifest.permission.CAMERA) !=
            PackageManager.PERMISSION_GRANTED){
            requestPermissions(arrayOf(android.Manifest.permission.CAMERA), 0)
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        getPermission()
    }

    private fun getFrontalCameraId(): String {
        return cameraManager.cameraIdList.first {
            cameraManager
                .getCameraCharacteristics(it)
                .get(CameraCharacteristics.LENS_FACING) == CameraCharacteristics.LENS_FACING_FRONT
        }
    }

    @SuppressLint("MissingPermission")
    fun openCamera(){
        cameraManager.openCamera(getFrontalCameraId(), object : CameraDevice.StateCallback(){
            override fun onOpened(p0: CameraDevice) {
                cameraDevice = p0
                val surfaceTexture = textureView.surfaceTexture
                surfaceTexture?.setDefaultBufferSize(previewSize.width, previewSize.height)

                val capReq = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW)
                val surface = Surface(surfaceTexture)
                capReq.addTarget(surface)

                cameraDevice.createCaptureSession(listOf(surface), object: CameraCaptureSession.StateCallback(){
                    override fun onConfigured(p0: CameraCaptureSession) {
                        cameraCaptureSession = p0
                        cameraCaptureSession.setRepeatingRequest(capReq.build(), null, null)
                    }
                    override fun onConfigureFailed(p0: CameraCaptureSession) {

                    }
                }, handler)
            }

            override fun onDisconnected(p0: CameraDevice) {}

            override fun onError(p0: CameraDevice, p1: Int) {}
        }, handler)
    }
}