package com.example.face_sdk_3divi_isolates_demo;
import android.content.pm.ApplicationInfo;
import androidx.annotation.NonNull;
import io.flutter.embedding.android.FlutterActivity;
import io.flutter.embedding.engine.FlutterEngine;
import io.flutter.embedding.engine.loader.FlutterLoader;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.FlutterInjector;

public class MainActivity extends FlutterActivity {
    static{
        System.loadLibrary("facerec");
    }

    private static final String CHANNEL = "samples.flutter.dev/facesdk";

    private String _getNativeLibDir() {
        return getApplicationInfo().nativeLibraryDir;
    }

    @Override
    public void configureFlutterEngine(@NonNull FlutterEngine flutterEngine) {
        super.configureFlutterEngine(flutterEngine);
        new MethodChannel(flutterEngine.getDartExecutor().getBinaryMessenger(), CHANNEL)
                .setMethodCallHandler(
                        (call, result) -> {
                            if (call.method.equals("getNativeLibDir")) {
                                String nativeLibDir = _getNativeLibDir();
                                result.success(nativeLibDir);
                            } else {
                                result.notImplemented();
                            }
                        }
                );
    }
}
