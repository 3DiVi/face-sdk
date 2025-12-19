package com.example.recognition_demo

import android.os.Bundle
import android.util.Log
import android.view.ViewGroup
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.updateLayoutParams
import androidx.core.view.updatePadding
import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.lifecycleScope
import androidx.navigation.NavController
import androidx.navigation.fragment.NavHostFragment
import com.google.android.material.bottomnavigation.BottomNavigationView
import com.vdt.face_recognition.sdk.DynamicTemplateIndex
import com.vdt.face_recognition.sdk.FacerecService
import com.vdt.face_recognition.sdk.ProcessingBlock
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.nio.file.Files
import java.nio.file.Paths

class MainActivity : AppCompatActivity() {
    private val sharedViewModel: SharedViewModel by viewModels()
    private val _serviceReady = MutableLiveData(false)
    val serviceReady: LiveData<Boolean> get() = _serviceReady

    private lateinit var service: FacerecService
    private lateinit var faceDetector: ProcessingBlock
    private lateinit var faceFitter: ProcessingBlock
    private lateinit var ageEstimator: ProcessingBlock
    private lateinit var genderEstimator: ProcessingBlock
    private lateinit var livenessEstimator : ProcessingBlock
    private lateinit var faceExtractor : ProcessingBlock
    private lateinit var faceMatcher : ProcessingBlock
    private lateinit var dynamicTemplateIndex: DynamicTemplateIndex

    private lateinit var navManager : NavHostFragment
    private lateinit var navController : NavController
    private lateinit var menu: BottomNavigationView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_splash)


        lifecycleScope.launch(Dispatchers.IO) {

            service = FacerecService.createService(
                applicationInfo.nativeLibraryDir + "/libfacerec.so",
                applicationInfo.dataDir + "/fsdk/conf/facerec",
                applicationInfo.dataDir + "/fsdk/license"
            )

            val configIndex = service.createContext()
            configIndex["modification"].string = "100m"

            dynamicTemplateIndex = service.createDynamicTemplateIndex(configIndex)

            loadTemplates()
            initializeProcessingBlocks(service)
            transferDataToViewModel()
            withContext(Dispatchers.Main)
            {
                setContentView(R.layout.activity_main)
                ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
                    val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
                    v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
                    insets
                }
                setUpNavigation()
                _serviceReady.value = true
            }
        }
    }


    private fun setUpNavigation()
    {
        navManager = supportFragmentManager.findFragmentById(R.id.fragment_manager) as NavHostFragment
        navController = navManager.navController
        menu = findViewById(R.id.menu)

        ViewCompat.setOnApplyWindowInsetsListener(menu) { view, insets ->
            val navBarInsets = insets.getInsets(WindowInsetsCompat.Type.navigationBars())
            view.updateLayoutParams<ViewGroup.MarginLayoutParams> {
                bottomMargin = 0
            }
            view.updatePadding(bottom = navBarInsets.bottom / 3)
            insets
        }

        menu.setOnItemSelectedListener { item ->
            when (item.itemId) {
                R.id.faceRecognitionFragment -> {
                    if(navController.currentDestination?.id != R.id.faceRecognitionFragment){
                        navController.navigate(R.id.faceRecognitionFragment)
                    }
                    true
                }

                R.id.faceRegisterFragment -> {
                    if(navController.currentDestination?.id != R.id.faceRegisterFragment){
                        navController.navigate(R.id.faceRegisterFragment)
                    }
                    true
                }
                else -> false
            }
        }
    }

    private fun loadTemplates(){
        val templatesDirPath = Paths.get(applicationInfo.dataDir + "/templates")
        if(Files.isDirectory(templatesDirPath)){
            try{
                Files.newDirectoryStream(templatesDirPath).use { directoryStream ->
                    for(templateFile in directoryStream){
                        Files.newInputStream(templateFile).use { stream ->
                            val template = service.loadContextTemplate(stream)
                            dynamicTemplateIndex.addContextTemplate(template,templateFile.fileName.toString())
                        }
                    }
                }
            }
            catch (e: Exception) {
                Log.e("FaceApplication", e.message!!)
                e.printStackTrace()
            }
        }
    }

    private fun initializeProcessingBlocks(service : FacerecService) {
        val configDetector = service.createContext()
        configDetector["unit_type"].string = "FACE_DETECTOR"
        configDetector["modification"].string = "ssyv_light"
        configDetector["version"].long = 1

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

        val configLivenessEstimator = service.createContext()
        configLivenessEstimator["unit_type"].string = "LIVENESS_ESTIMATOR"
        configLivenessEstimator["modification"].string = "2d_ensemble_light"
        configLivenessEstimator["version"].long = 2

        livenessEstimator = service.createProcessingBlock(configLivenessEstimator)

        val configFaceExtractor = service.createContext()
        configFaceExtractor["unit_type"].string = "FACE_TEMPLATE_EXTRACTOR"
        configFaceExtractor["modification"].string = "100m"

        faceExtractor = service.createProcessingBlock(configFaceExtractor)

        val configMatcher = service.createContext()
        configMatcher["unit_type"].string = "MATCHER_MODULE"

        faceMatcher = service.createProcessingBlock(configMatcher)
    }

    private fun transferDataToViewModel(){
        sharedViewModel.service = service
        sharedViewModel.faceDetector = faceDetector
        sharedViewModel.faceFitter = faceFitter
        sharedViewModel.ageEstimator = ageEstimator
        sharedViewModel.genderEstimator = genderEstimator
        sharedViewModel.livenessEstimator = livenessEstimator
        sharedViewModel.faceExtractor = faceExtractor
        sharedViewModel.faceMatcher = faceMatcher
        sharedViewModel.dynamicTemplateIndex = dynamicTemplateIndex
    }
}