package com.example.kotlintutorial

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.edit
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.BufferedReader
import java.io.File
import java.io.FileOutputStream
import java.io.InputStreamReader
import java.util.ArrayDeque
import java.util.Queue

class UnpackAssetsActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_splash)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.SplashScreen)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }
        CoroutineScope(Dispatchers.IO).launch {
            try {
                // read first line from assets-hash.txt
                val newHash =
                    BufferedReader(InputStreamReader(assets.open("assets-hash.txt"))).readLine()

                // and compare it with what we have already
                val sharedPreferences = getSharedPreferences("fe9733f0bfb7", 0)

                val prevHash = sharedPreferences.getString("assets-hash", null)

                // unpack everything again, if something changes
                if (prevHash != newHash) {
                    val buffer = ByteArray(10240)

                    val persistentDir = applicationInfo.dataDir

                    val queue: Queue<String?> = ArrayDeque<String?>()
                    queue.add("conf")
                    queue.add("share")
                    queue.add("license")

                    while (!queue.isEmpty()) {
                        val path = queue.remove()

                        val list = assets.list(path!!)

                        if (list!!.isEmpty()) {
                            val fileStream = assets.open(path)

                            val fullPath = "$persistentDir/fsdk/$path"

                            File(fullPath).parentFile?.mkdirs()

                            val outFile = FileOutputStream(fullPath)

                            while (true) {
                                val read = fileStream.read(buffer)

                                if (read <= 0) break

                                outFile.write(buffer, 0, read)
                            }

                            fileStream.close()
                            outFile.close()
                        } else {
                            for (p in list) queue.add("$path/$p")
                        }
                    }

                    sharedPreferences.edit {
                        putString("assets-hash",newHash)
                    }
                }

            } catch (e: Exception) {
                Log.e("FaceApplication", e.message!!)
                e.printStackTrace()
            }

            withContext(Dispatchers.Main){
                val intent = Intent(applicationContext, MainActivity:: class.java)
                startActivity(intent)
                finish()
            }
        }
    }
}