package com.example.recognition_demo

import android.graphics.Bitmap
import android.os.Bundle
import android.text.InputType
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.view.inputmethod.EditorInfo
import android.widget.EditText
import android.widget.ImageButton
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.camera.core.CameraSelector
import androidx.camera.core.ImageCapture
import androidx.camera.core.ImageCaptureException
import androidx.camera.core.ImageProxy
import androidx.camera.core.Preview
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.content.ContextCompat
import androidx.fragment.app.activityViewModels
import androidx.lifecycle.coroutineScope
import kotlinx.coroutines.cancel
import kotlinx.coroutines.launch
import kotlinx.coroutines.suspendCancellableCoroutine
import java.io.File
import java.io.FileOutputStream
import java.nio.ByteBuffer

class FaceRegisterFragment : Fragment() {
    private val sharedViewModel : SharedViewModel by activityViewModels()

    private lateinit var ioData : com.vdt.face_recognition.sdk.Context

    private lateinit var rootView : View
    private lateinit var previewView : PreviewView
    private lateinit var takePhotoBtn : ImageButton

    private lateinit var cameraProvider : ProcessCameraProvider
    private lateinit var cameraSelector : CameraSelector
    private lateinit var preview : Preview
    private lateinit var imageCapture : ImageCapture

    lateinit var bitmap: Bitmap

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        rootView = inflater.inflate(R.layout.fragment_face_register, container, false)
        return rootView
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        previewView = rootView.findViewById(R.id.previewViewPhoto)
        takePhotoBtn = rootView.findViewById(R.id.facePhotoBtn)

        configurateCamera()

        takePhotoBtn.setOnClickListener{
            takePhotoBtn.visibility = View.GONE
            takePhoto()
        }
    }

    private fun configurateCamera(){
        val processCameraProvider = ProcessCameraProvider.getInstance(requireContext())
        processCameraProvider.addListener({
            cameraProvider = processCameraProvider.get()

            cameraSelector = CameraSelector.DEFAULT_FRONT_CAMERA

            preview = Preview.Builder()
                .build()
                .also {
                    it.surfaceProvider = previewView.surfaceProvider
                }

            imageCapture = ImageCapture.Builder()
                .setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
                .build()

            try{
                cameraProvider.bindToLifecycle(
                    viewLifecycleOwner,
                    cameraSelector,
                    preview,
                    imageCapture
                )

            } catch (e: Exception){
                e.printStackTrace()
            }

            takePhotoBtn.visibility = View.VISIBLE

        }, ContextCompat.getMainExecutor(requireContext()))
    }

    private fun takePhoto(){
        imageCapture.takePicture(ContextCompat.getMainExecutor(requireContext()), object : ImageCapture.OnImageCapturedCallback() {
            override fun onCaptureSuccess(image: ImageProxy) {
                super.onCaptureSuccess(image)
                previewView.bitmap?.let {
                    bitmap = it
                }

                val width = bitmap.width
                val height = bitmap.height

                val buffer = ByteBuffer.allocate(bitmap.byteCount)
                bitmap.copyPixelsToBuffer(buffer)
                val imageData = buffer.array()

                ioData = sharedViewModel.service!!.createContextFromFrame(imageData,width, height,
                    com.vdt.face_recognition.sdk.Context.Format.FORMAT_BGRA8888,0)
                sharedViewModel.faceDetector?.process(ioData)

                if(ioData["objects"].size().toInt() == 0){
                    image.close()
                    promptToRetakePhoto()
                    takePhotoBtn.visibility = View.VISIBLE
                    return
                }

                sharedViewModel.faceFitter?.process(ioData)
                sharedViewModel.faceExtractor?.process(ioData)
                val faceTemplate = ioData["objects"][0]["face_template"]["template"].contextTemplate

                val result = sharedViewModel.identification(faceTemplate)
                if(result == null){
                    viewLifecycleOwner.lifecycle.coroutineScope.launch {
                        val templateName = enterCapturedFaceName()
                        if(templateName == null){
                            image.close()
                            takePhotoBtn.visibility = View.VISIBLE
                            coroutineContext.cancel()
                        }
                        else{
                            sharedViewModel.dynamicTemplateIndex?.addContextTemplate(faceTemplate, templateName)

                            val fullPath = requireContext().applicationInfo.dataDir + "/templates" + "/$templateName"
                            File(fullPath).parentFile?.mkdirs()
                            val outFile = FileOutputStream(fullPath)
                            faceTemplate.save(outFile)

                            takePhotoBtn.visibility = View.VISIBLE
                            Toast.makeText(requireContext(),"Template has been added to base!", Toast.LENGTH_SHORT).show()
                        }
                    }
                }
                else{
                    takePhotoBtn.visibility = View.VISIBLE
                    Toast.makeText(requireContext(),"Template already in the base", Toast.LENGTH_SHORT).show()
                }
            }

            override fun onError(exception: ImageCaptureException) {
                super.onError(exception)
                promptToRetakePhoto()
                takePhotoBtn.visibility = View.VISIBLE
            }
        })
    }

    private suspend fun enterCapturedFaceName() : String? = suspendCancellableCoroutine { continuation ->
        val input = EditText(requireContext()).apply {
            hint = "Enter the name who was photographed"
            maxLines = 1
            inputType = InputType.TYPE_CLASS_TEXT
            imeOptions = EditorInfo.IME_ACTION_DONE
        }

        val dialog = AlertDialog.Builder(requireContext())
            .setView(input)
            .setPositiveButton("OK", null)
            .create()

        dialog.setOnShowListener {
            fun inputValidate(){
                val inputName = input.text.toString().trim()
                if (inputName.isNotEmpty()) {
                    try{
                        sharedViewModel.dynamicTemplateIndex?.at(inputName)
                        input.error = "Template with the same name is already in the base, please enter another name"
                    }
                    catch (_ : Exception){
                        if (continuation.isActive) {
                            dialog.dismiss()
                            continuation.resume(inputName) { _, _, _ -> }
                        }
                    }
                }
                else
                    input.error = "Please enter a name"
            }

            val button = dialog.getButton(AlertDialog.BUTTON_POSITIVE)
            button.setOnClickListener {
                inputValidate()
            }

            input.setOnEditorActionListener { _, actionID, _ ->
                if(actionID == EditorInfo.IME_ACTION_DONE) {
                    inputValidate()
                    true
                }
                else
                    false
            }
        }

        dialog.setOnCancelListener {
            if(continuation.isActive)
                continuation.resume(null){_,_,_ ->}
        }

        dialog.show()
    }

    private fun promptToRetakePhoto(){
        AlertDialog.Builder(requireContext())
            .setTitle("Face has no detected")
            .setMessage("Please take the photo again")
            .setPositiveButton("Retry"){dialog, _ ->
                dialog.dismiss()
            }
            .create()
            .show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        cameraProvider.unbindAll()
    }
}