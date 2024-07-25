package com.example.kotlintutorial

import android.app.Activity
import android.content.Intent
import android.os.Bundle
import android.util.Log
import java.io.BufferedReader
import java.io.File
import java.io.FileOutputStream
import java.io.InputStreamReader
import java.util.ArrayDeque
import java.util.Queue

// activity to upack all assets
class UnpackAssetsActivity : Activity() {
    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        try {
            // read first line from assets-hash.txt
            val newHash =
                BufferedReader(InputStreamReader(assets.open("assets-hash.txt"))).readLine()

            // and compare it with what we have already
            val sharedPreferences = getSharedPreferences("fe9733f0bfb7", 0)

            val prevHash = sharedPreferences.getString("assets-hash", null)

            // unpack everything again, if something changes
            if (prevHash == null || prevHash != newHash) {
                val buffer = ByteArray(10240)

                val persistentDir = applicationInfo.dataDir

                val queue: Queue<String?> = ArrayDeque<String?>()
                queue.add("conf")
                queue.add("share")
                queue.add("license")

                while (!queue.isEmpty()) {
                    val path = queue.element()
                    queue.remove()

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

                val editor = sharedPreferences.edit()
                editor.putString("assets-hash", newHash)
                while (!editor.commit());
            }

            val intent = Intent(applicationContext, MainActivity::class.java)
            startActivity(intent)
            finish()
        } catch (e: Exception) {
            Log.e("UnpackAssetsActivity", e.message!!)
            e.printStackTrace()
            finishAffinity()
        }
    }
}
