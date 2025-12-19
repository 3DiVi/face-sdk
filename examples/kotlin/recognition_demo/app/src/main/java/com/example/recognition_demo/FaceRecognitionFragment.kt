package com.example.recognition_demo

import android.Manifest
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Matrix
import android.graphics.Paint
import android.graphics.RectF
import android.net.Uri
import android.os.Bundle
import android.provider.Settings
import android.view.LayoutInflater
import android.view.Surface
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.TextView
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AlertDialog
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageAnalysis
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.core.view.doOnLayout
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.channels.BufferOverflow
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.coroutines.withContext
import java.util.concurrent.Executors


data class FrameData(
    val data: ByteArray,
    val width: Int,
    val height: Int
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as FrameData

        if (width != other.width) return false
        if (height != other.height) return false
        if (!data.contentEquals(other.data)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = width
        result = 31 * result + height
        result = 31 * result + data.contentHashCode()
        return result
    }
}

class FaceRecognitionFragment : Fragment() {
    private val sharedViewModel : SharedViewModel by activityViewModels()
    private lateinit var activityRef: MainActivity

    private val requestPermissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) { permission ->
            when {
                permission -> {
                    initializeCamera()
                }

                shouldShowRequestPermissionRationale(Manifest.permission.CAMERA) -> {
                    showCameraPermissionRationale()
                }

                else -> {
                    showCameraPermissionSettingsPrompt()
                }
            }
        }

    private val settingsLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()){
        requestPermissionLauncher.launch(Manifest.permission.CAMERA)
    }

    private lateinit var rootView : View
    private lateinit var previewViewRec : PreviewView
    private lateinit var bboxImage: ImageView
    private lateinit var noDetectedWarning : TextView
    private lateinit var detectedFaceName : TextView

    private lateinit var cameraProvider : ProcessCameraProvider
    private lateinit var cameraSelector : CameraSelector
    private lateinit var imageAnalysis: ImageAnalysis

    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.Default)
    private val detectionChannel = Channel<List<RectF>>(capacity = 1, onBufferOverflow = BufferOverflow.DROP_OLDEST)

    private val mutex = Mutex()
    private val frameFlow = MutableSharedFlow<FrameData>(
        replay = 1,
        extraBufferCapacity = 1,
        onBufferOverflow = BufferOverflow.DROP_OLDEST
    )

    private val uiFlow = MutableSharedFlow<Bitmap>(
        replay = 1,
        extraBufferCapacity = 1,
        onBufferOverflow = BufferOverflow.DROP_OLDEST
    )

    val paint = Paint()

    private var detectedFaceNameStr: String = ""
    private var detectedAge: Long = 0
    private var detectedGender: String = ""
    private var angle = 0


    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        rootView = inflater.inflate(R.layout.fragment_face_recognition, container, false)
        return rootView
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        activityRef = requireActivity() as MainActivity

        activityRef.serviceReady.observe(viewLifecycleOwner) { ready ->
            if (ready) {
                initializeUIAndObservers()
            }
        }
    }

    private fun initializeUIAndObservers()
    {
        previewViewRec = rootView.findViewById(R.id.previewViewRec)
        noDetectedWarning = rootView.findViewById(R.id.noDetectedWarning)
        bboxImage = rootView.findViewById(R.id.bbox)
        detectedFaceName = rootView.findViewById(R.id.detectedFaceName)
        noDetectedWarning.visibility = View.GONE

        previewViewRec.doOnLayout {
            requestPermissionLauncher.launch(Manifest.permission.CAMERA)
        }

        scope.launch(Dispatchers.Default) {
            frameFlow.collectLatest { frame ->
                processFrameData(frame.data, frame.width, frame.height)
            }
        }

        viewLifecycleOwner.lifecycleScope.launch {
            uiFlow
                .collectLatest { bitmap ->
                    withContext(Dispatchers.Main) {
                        renderDetectionResults(bitmap)
                    }
                }
        }
    }

    private fun renderDetectionResults(bitmap: Bitmap) {
        val  width = bitmap.width
        val  height = bitmap.height


        val mutable = bitmap.copy(Bitmap.Config.ARGB_8888, true)
        val canvas = Canvas(mutable)
        val boxes = detectionChannel.tryReceive().getOrNull()
        if (boxes != null) {
            if(boxes.isEmpty()){
                detectedFaceName.text = null
                noDetectedWarning.visibility = View.VISIBLE
                bboxImage.setImageBitmap(mutable)
                return
            }

            paint.textSize = height / 40f
            paint.strokeWidth = width / 100f

            paint.color = if (detectedFaceNameStr != "") Color.GREEN else Color.RED
            paint.style = Paint.Style.STROKE
            noDetectedWarning.visibility = View.GONE
            val indentionY = paint.textSize

            for (box in boxes)
            {
                val matrix = Matrix().apply {
                    preScale(-width.toFloat(), height.toFloat())
                    postTranslate(width.toFloat(), 0f)
                }
                matrix.mapRect(box)
                val x1 = box.left
                val x2 = box.right
                val y1 = box.top

                paint.style = Paint.Style.STROKE
                canvas.drawRect(RectF(box), paint)
                paint.style = Paint.Style.FILL


                var paintTextY = y1
                if (detectedFaceNameStr != "")
                {
                    val paintTextX = (x1 + x2 -  paint.measureText(detectedFaceNameStr)) / 2f
                    canvas.drawText(
                        detectedFaceNameStr,
                        paintTextX, y1 - indentionY, paint
                    )
                }

                val paintTextX = x1 + (x2 - x1) * 0.05f
                if (detectedAge != 0L)
                {
                    val resultText = "age: $detectedAge"
                    paintTextY += indentionY
                    canvas.drawText(
                        resultText,
                        paintTextX, paintTextY, paint
                    )
                }

                if (detectedGender != "")
                {
                    val resultText = "gender: $detectedGender"
                    paintTextY += indentionY
                    canvas.drawText(
                        resultText,
                        paintTextX, paintTextY, paint
                    )
                }

            }
            bboxImage.setImageBitmap(mutable)
        }
    }

    fun convertImageProxyToYUV(image: ImageProxy): ByteArray {
        val yBuffer = image.planes[0].buffer
        val uBuffer = image.planes[1].buffer
        val vBuffer = image.planes[2].buffer

        val ySize = yBuffer.remaining()
        val uSize = uBuffer.remaining()
        val vSize = vBuffer.remaining()

        val data = ByteArray(ySize + uSize + vSize)

        yBuffer.get(data, 0, ySize)
        uBuffer.get(data, ySize, uSize)
        vBuffer.get(data, ySize + uSize, vSize)

        return data
    }

    private fun initializeCamera(){
        val processCameraProvider = ProcessCameraProvider.getInstance(requireContext())
        processCameraProvider.addListener({
            cameraProvider = processCameraProvider.get()

            cameraSelector = CameraSelector.DEFAULT_FRONT_CAMERA


            val preview = Preview.Builder()
                .setTargetRotation(Surface.ROTATION_0)
                .build()
                .also {
                    it.surfaceProvider = previewViewRec.surfaceProvider
                }

            imageAnalysis = ImageAnalysis.Builder()
                .setBackpressureStrategy(ImageAnalysis.STRATEGY_KEEP_ONLY_LATEST)
                .build()
                .also {
                    it.setAnalyzer(Executors.newSingleThreadExecutor()){ imageProxy ->
                        val data = convertImageProxyToYUV(imageProxy)
                        val width = imageProxy.width
                        val height = imageProxy.height
                        frameFlow.tryEmit(FrameData(data, width, height))
                        imageProxy.close()
                    }
                }

            try{
                val camera = cameraProvider.bindToLifecycle(
                    viewLifecycleOwner,
                    cameraSelector,
                    preview,
                    imageAnalysis,
                )


                if (camera.cameraInfo.sensorRotationDegrees == 90){
                    angle = 1
                } else if (camera.cameraInfo.sensorRotationDegrees == 270) {
                    angle = 2
                } else if (camera.cameraInfo.sensorRotationDegrees == 180) {
                    angle = 3
                }

            } catch (e: Exception){
                e.printStackTrace()
            }


            startPreviewFrameCollector()
        }, ContextCompat.getMainExecutor(requireContext()))

    }

    private fun startPreviewFrameCollector() {
        val handler = android.os.Handler(android.os.Looper.getMainLooper())
        var lastBitmapHash: Int? = null
        val runnable = object : Runnable {
            override fun run() {
                val bmp = previewViewRec.bitmap
                if (bmp != null) {
                    val newHash = bmp.hashCode()
                    if (newHash != lastBitmapHash) {
                        lastBitmapHash = newHash
                        uiFlow.tryEmit(bmp)
                    }
                }
                handler.postDelayed(this, 5)
            }
        }
        handler.post(runnable)
    }

    private fun processFrameData(data: ByteArray, width: Int, height: Int){
        val ioData = sharedViewModel.service!!.createContextFromFrame(data, width, height,
            com.vdt.face_recognition.sdk.Context.Format.FORMAT_YUV420, angle)
        sharedViewModel.faceDetector?.process(ioData)

        val faces = ioData["objects"]
        val boxes = mutableListOf<RectF>()

        for (i in 0 until faces.size()) {
            val obj = faces[i.toInt()]
            val bbox = obj["bbox"]
            val x1 = bbox[0].double.toFloat()
            val y1 = bbox[1].double.toFloat()
            val x2 = bbox[2].double.toFloat()
            val y2 = bbox[3].double.toFloat()

            boxes.add(RectF(x1, y1, x2, y2))
        }

        detectionChannel.trySend(boxes)

        if (faces.size() == 1L)
        {
            scope.launch {
                mutex.withLock {
                    sharedViewModel.faceFitter?.process(ioData)
                    val obj = ioData["objects"][0]
                    if (sharedViewModel.dynamicTemplateIndex?.size() != 0 && detectedFaceNameStr == "")
                    {
                        sharedViewModel.livenessEstimator?.process(ioData)

                        val liveness = obj["liveness"]["value"].string
                        if (liveness == "REAL")
                        {
                            sharedViewModel.faceExtractor?.process(ioData)
                            val detectedFaceTemplate = ioData["objects"][0]["face_template"]["template"].contextTemplate
                            val result = sharedViewModel.identification(detectedFaceTemplate)
                            if(result != null){
                                detectedFaceNameStr = result
                            }
                        }
                    }

                    sharedViewModel.ageEstimator?.process(ioData)
                    val age = obj["age"].long
                    detectedAge = if (detectedAge != 0L) (detectedAge + age) / 2 else age

                    if (detectedGender == "")
                    {
                        sharedViewModel.genderEstimator?.process(ioData)
                        detectedGender = obj["gender"].string
                    }
                }
            }
        }
        else
        {
            detectedFaceNameStr = ""
            detectedAge = 0
            detectedGender = ""
        }

    }

    private fun showCameraPermissionRationale() {
        AlertDialog.Builder(requireContext())
            .setTitle("Permission required to use camera")
            .setMessage("To detect your face, you need to allow the application to use the camera")
            .setCancelable(false)
            .setPositiveButton("OK"){dialog, _->
                requestPermissionLauncher.launch(Manifest.permission.CAMERA)
                dialog.dismiss()
            }
            .create()
            .show()
    }

    private fun showCameraPermissionSettingsPrompt() {
        AlertDialog.Builder(requireContext())
            .setTitle("Permission required to use camera")
            .setMessage("You need to give permission in the application settings")
            .setCancelable(false)
            .setPositiveButton("Go to settings"){dialog, _ ->
                openAppSettings()
                dialog.dismiss()
            }
            .create()
            .show()
    }

    private fun openAppSettings() {
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
        val uri : Uri = Uri.fromParts("package", requireContext().packageName, null)
        intent.data = uri
        settingsLauncher.launch(intent)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        cameraProvider.unbindAll()
    }
}