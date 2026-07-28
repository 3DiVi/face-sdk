package com.vdt.face_recognition.video_recognition_pb_demo;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;
import java.util.Vector;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.Log;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ContextTemplate;
import com.vdt.face_recognition.sdk.DynamicTemplateIndex;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.ProcessingBlock;

public class Database {

    private static final String TAG = "Database";
    private static final String CTEMPLATE_EXT = ".ctemplate";

    private final MainActivity activity;
    public DynamicTemplateIndex index;
    public Vector<Bitmap> thumbnails = new Vector<>();
    public Vector<String> names = new Vector<>();
    public Vector<String> uuids = new Vector<>();

    private boolean isImage(String filename) {
        final String[] okFileExtensions = new String[]{
            "jpg", "png", "gif", "bmp", "jpeg"
        };
        for (String extension : okFileExtensions) {
            if (filename.toLowerCase().endsWith(extension)) {
                return true;
            }
        }
        return false;
    }

    public Database(
        final MainActivity activity,
        final String db_dir,
        FacerecService service,
        ProcessingBlock detector,
        ProcessingBlock fitter,
        ProcessingBlock extractor)
    {
        this.activity = activity;

        Context indexConfig = service.createContext();
        indexConfig.get("modification").setString("50m");
        indexConfig.get("max_license_count").setLong(100);
        index = service.createDynamicTemplateIndex(indexConfig);
        indexConfig.dispose();

        File db_dir_file = new File(db_dir);
        if (!db_dir_file.exists()) {
            db_dir_file.mkdir();
        }

        ProcessingBlock utility = null;
        Context ucfg = service.createContext();
        ucfg.get("unit_type").setString("UTILITY_MODULE");
        ucfg.get("modification").setString("face_cut");
        ucfg.get("cut_type").setString("FACE_CUT_BASE");
        utility = service.createProcessingBlock(ucfg);
        ucfg.dispose();

        Vector<File> person_dirs = new Vector<>();
        for (File file : db_dir_file.listFiles()) {
            if (file.isDirectory()) {
                person_dirs.add(file);
            }
        }

        for (File dir : person_dirs) {
            String name = null;
            for (File file : dir.listFiles()) {
                if (file.getName().equals("name.txt")) {
                    Scanner scanner = null;
                    try {
                        scanner = new Scanner(file);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    }
                    name = scanner.useDelimiter("\\A").next();
                    scanner.close();
                }
            }

            Log.i(TAG, "Processing " + name);

            for (File file : dir.listFiles()) {
                final String filename = file.getName();

                if (!isImage(filename))
                    continue;

                byte[] byte_image;
                try {
                    byte_image = readImage(file);
                } catch (IOException e) {
                    showErrorMessage("Can not open " + file.getName() + "\nErrorMessage: " + e.getMessage());
                    continue;
                }

                String uuid = dir.getName() + "_" + filename;

                File ctemplate_file = new File(file.getAbsolutePath() + CTEMPLATE_EXT);
                if (ctemplate_file.exists()) {
                    try {
                        FileInputStream fis = new FileInputStream(ctemplate_file);
                        ContextTemplate templ = service.loadContextTemplate(fis);
                        fis.close();
                        index.addContextTemplate(templ, uuid);
                        templ.dispose();
                        thumbnails.add(makeThumbnail(byte_image, name));
                    } catch (Exception e) {
                        showErrorMessage("Can not open " + ctemplate_file.getName() + "\n" + e.getMessage());
                        continue;
                    }
                } else {
                    Context ioData = service.createContextFromEncodedImage(byte_image);
                    try {
                        detector.process(ioData);
                        fitter.process(ioData);
                        utility.process(ioData);
                        extractor.process(ioData);

                        if (ioData.get("objects").size() == 0) {
                            showErrorMessage("No face detected in " + file.getName());
                            ioData.dispose();
                            continue;
                        }

                        Context obj = ioData.get("objects").get(0);
                        ContextTemplate templ = obj.get("face_template").get("template").getContextTemplate();

                        index.addContextTemplate(templ, uuid);

                        FileOutputStream fos = new FileOutputStream(ctemplate_file);
                        templ.save(fos);
                        fos.close();

                        templ.dispose();

                        Context faceCropCtx = obj.get("face_crop");
                        byte[] rgbData = faceCropCtx.get("blob").getBytes();
                        int cropH = (int) faceCropCtx.get("shape").get(0).getLong();
                        int cropW = (int) faceCropCtx.get("shape").get(1).getLong();
                        int[] cropArgb = new int[cropW * cropH];
                        for (int j = 0; j < cropW * cropH; j++) {
                            cropArgb[j] = 0xFF000000
                                | ((rgbData[j * 3] & 0xFF) << 16)
                                | ((rgbData[j * 3 + 1] & 0xFF) << 8)
                                | (rgbData[j * 3 + 2] & 0xFF);
                        }
                        Bitmap faceCropBmp = Bitmap.createBitmap(cropArgb, cropW, cropH, Bitmap.Config.ARGB_8888);
                        thumbnails.add(addNameOverlay(faceCropBmp, name));
                    } catch (Exception e) {
                        showErrorMessage("File: " + file.getName() + "\n" + e.getMessage());
                        ioData.dispose();
                        continue;
                    }
                    ioData.dispose();
                }

                names.add(name);
                uuids.add(uuid);
            }
        }

        if (utility != null) utility.dispose();
    }

    private Bitmap addNameOverlay(Bitmap faceCrop, String name) {
        Bitmap result = faceCrop.copy(faceCrop.getConfig(), true);
        Canvas canvas = new Canvas(result);
        int width = result.getWidth();
        int height = result.getHeight();
        double k = 0.87;
        Rect backgroundRect = new Rect(0, (int) (k * height), width, height);
        Paint backgroundPaint = new Paint();
        backgroundPaint.setColor(0xa0000000);
        backgroundPaint.setStyle(Paint.Style.FILL);
        canvas.drawRect(backgroundRect, backgroundPaint);
        Paint text_paint = new Paint();
        int font_size = height / 8;
        text_paint.setColor(0xffffffff);
        text_paint.setTextSize(font_size);
        int text_pad = (int) (height * (1 - k) / 10);
        canvas.drawText(name, text_pad, height - text_pad, text_paint);
        return result;
    }

    public static Bitmap makeThumbnail(byte[] imageBytes, String name) {
        Bitmap bitmap = BitmapFactory.decodeByteArray(imageBytes, 0, imageBytes.length);
        if (bitmap == null) return null;

        float scale = Math.min(200f / bitmap.getWidth(), 200f / bitmap.getHeight());
        if (scale < 1f) {
            bitmap = Bitmap.createScaledBitmap(bitmap,
                (int) (bitmap.getWidth() * scale),
                (int) (bitmap.getHeight() * scale), true);
        }

        Bitmap result = bitmap.copy(Bitmap.Config.ARGB_8888, true);
        Canvas canvas = new Canvas(result);
        int width = result.getWidth();
        int height = result.getHeight();

        double k = 0.87;
        Rect backgroundRect = new Rect(0, (int) (k * height), width, height);
        Paint backgroundPaint = new Paint();
        backgroundPaint.setColor(0xa0000000);
        backgroundPaint.setStyle(Paint.Style.FILL);
        canvas.drawRect(backgroundRect, backgroundPaint);

        Paint text_paint = new Paint();
        int font_size = height / 8;
        text_paint.setColor(0xffffffff);
        text_paint.setTextSize(font_size);
        int text_pad = (int) (height * (1 - k) / 10);
        canvas.drawText(name, text_pad, height - text_pad, text_paint);

        return result;
    }

    private byte[] readImage(final File file) throws IOException {
        int size = (int) file.length();
        byte[] bytes = new byte[size];
        BufferedInputStream buf = new BufferedInputStream(new FileInputStream(file));
        buf.read(bytes, 0, bytes.length);
        buf.close();
        return bytes;
    }

    private void showErrorMessage(final String message) {
        activity.runOnUiThread(new Runnable() {
            public void run() {
                Toast.makeText(activity, message, Toast.LENGTH_LONG).show();
            }
        });
    }
}
