import com.android.build.gradle.internal.tasks.factory.dependsOn
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
}

android {
    namespace = "com.example.kotlintutorial"
    compileSdk = 36

    defaultConfig {
        applicationId = "com.example.kotlintutorial"
        minSdk = 24
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    packagingOptions {
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
                srcDir("$projectDir/../../../../../lib")
            }
        }
    }

    task("computeAssetsHash") {
        doLast {
            mkdir("$projectDir/src/main/assets/")
            File("$projectDir/src/main/assets/", "assets-hash.txt").writeText(
                "Buildtime ${LocalDateTime.now().format(DateTimeFormatter.ofPattern("YYYY:MM:dd:HH:mm:ss"))}"
            )
        }
    }

    task<Copy>("copyFiles") {
        description = "Copy files"
        from("$projectDir/../../../../..") {
            include(
                "conf/**",
                "share/processing_block/age_estimator/light/2.enc",
                "share/processing_block/gender_estimator/light/2.enc",
                "share/processing_block/face_fitter/fda/1.enc",
                "share/processing_block/face_detector/blf_front/1.enc",
                "license/**"
            )
        }
        into("$projectDir/src/main/assets/")
    }

    project.tasks.preBuild.dependsOn("computeAssetsHash")
    project.tasks.preBuild.dependsOn("copyFiles")
}

dependencies {
    implementation(files("$projectDir/../../../../../lib/facerec.jar"))
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.activity)
    implementation(libs.androidx.constraintlayout)
    implementation(libs.kotlin.coroutines.core)
    implementation(libs.coroutines.android)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}