package com.example.recognition_demo

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.edit
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import java.io.File
import java.io.FileOutputStream
import java.util.ArrayDeque
import java.util.Queue

class UnpackAssetsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_splash)
        lifecycleScope.launch(Dispatchers.IO) {
            unpackAssetsIfNeeded()
            val intent = Intent(this@UnpackAssetsActivity, MainActivity:: class.java)
            startActivity(intent)
            finish()
        }
    }

    private fun unpackAssetsIfNeeded() {
        try {
            val newHash = assets.open("assets-hash.txt").bufferedReader().use { it.readLine() }
            val sharedPreferences = getSharedPreferences("fe9733f0bfb7", MODE_PRIVATE)
            val prevHash = sharedPreferences.getString("assets-hash", null)

            if (prevHash == newHash) return

            val buffer = ByteArray(10240)
            val persistentDir = applicationInfo.dataDir
            val queue: Queue<String?> = ArrayDeque()
            queue.add("conf")
            queue.add("share")
            queue.add("license")

            while (queue.isNotEmpty()) {
                val path = queue.remove() ?: continue
                val list = assets.list(path)
                if (list.isNullOrEmpty()) {
                    val fullPath = "$persistentDir/fsdk/$path"
                    File(fullPath).parentFile?.mkdirs()

                    assets.open(path).use { input ->
                        FileOutputStream(fullPath).use { output ->
                            while (true) {
                                val read = input.read(buffer)
                                if (read <= 0) break
                                output.write(buffer, 0, read)
                            }
                        }
                    }
                } else {
                    for (p in list) queue.add("$path/$p")
                }
            }

            sharedPreferences.edit { putString("assets-hash", newHash) }

        } catch (e: Exception) {
            Log.e("FaceApplication", "UnpackAssets error: ${e.message}", e)
        }
    }
}
