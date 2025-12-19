import com.android.build.gradle.internal.tasks.factory.dependsOn
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
}

android {
    namespace = "com.example.recognition_demo"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.example.recognition_demo"
        minSdk = 26
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    packaging {
        jniLibs {
            useLegacyPackaging = true
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }

    sourceSets {
        this.getByName("main"){
            jniLibs {
                srcDir("$projectDir/../../../../lib")
            }
        }
    }

    tasks.register("computeAssetsHash") {
        doLast {
            mkdir("$projectDir/src/main/assets/")
            File("$projectDir/src/main/assets/", "assets-hash.txt").writeText(
                "Buildtime ${LocalDateTime.now().format(DateTimeFormatter.ofPattern("YYYY:MM:dd:HH:mm:ss"))}"
            )
        }
    }

    tasks.register<Copy>("copyFiles") {
        description = "Copy files"
        from("$projectDir/../../../..") {
            include(
                "conf/**",
                "share/processing_block/age_estimator/light/2.enc",
                "share/processing_block/gender_estimator/light/2.enc",
                "share/processing_block/face_fitter/fda/1.enc",
                "share/processing_block/face_detector/ssyv_light/1.enc",
                "share/processing_block/liveness_estimator/2d_ensemble_light/2/**",
                "share/processing_block/face_template_extractor/100m/**",
                "license/**"
            )
        }
        into("$projectDir/src/main/assets/")
    }

    project.tasks.preBuild.dependsOn("computeAssetsHash")
    project.tasks.preBuild.dependsOn("copyFiles")
}

dependencies {
    implementation(files("$projectDir/../../../../lib/facerec.jar"))
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.activity)
    implementation(libs.androidx.constraintlayout)

    implementation(libs.kotlin.coroutines.core)
    implementation(libs.coroutines.android)

    implementation(libs.androidx.navigation.fragment.ktx)
    implementation(libs.androidx.navigation.ui.ktx)

    implementation(libs.camerax.core)
    implementation(libs.camerax.camera2)
    implementation(libs.camerax.lifecycle)
    implementation(libs.camerax.view)
    implementation(libs.camerax.extensions)

    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
