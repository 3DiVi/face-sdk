package com.example.recognition_demo

import androidx.lifecycle.ViewModel
import com.vdt.face_recognition.sdk.ContextTemplate
import com.vdt.face_recognition.sdk.DynamicTemplateIndex
import com.vdt.face_recognition.sdk.FacerecService
import com.vdt.face_recognition.sdk.ProcessingBlock

class SharedViewModel : ViewModel() {
    var service: FacerecService? = null
    var faceDetector: ProcessingBlock? = null
    var faceFitter: ProcessingBlock? = null
    var ageEstimator: ProcessingBlock? = null
    var genderEstimator: ProcessingBlock? = null
    var livenessEstimator: ProcessingBlock? = null
    var faceExtractor : ProcessingBlock? = null
    var faceMatcher : ProcessingBlock? = null
    var dynamicTemplateIndex: DynamicTemplateIndex? = null

    fun identification(detectedFaceTemplate : ContextTemplate) : String? {
        if(dynamicTemplateIndex?.size() == 0)
            return null

        val matcherData = service?.createContext()
        matcherData!!["queries"].contextTemplate = detectedFaceTemplate
        matcherData["template_index"].dynamicTemplateIndex = dynamicTemplateIndex

        faceMatcher?.process(matcherData)

        val matcherResults = matcherData["results"]
        val thresholdScore = 0.85f
        for(face in 0 until matcherResults.size()){
            val score = matcherResults.get(face.toInt())["score"].double
            if(score >= thresholdScore){
                return matcherResults.get(face.toInt())["uuid"].string
            }
        }
        return null
    }
}