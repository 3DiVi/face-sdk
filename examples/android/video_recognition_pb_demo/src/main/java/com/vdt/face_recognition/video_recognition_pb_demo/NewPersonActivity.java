package com.vdt.face_recognition.video_recognition_pb_demo;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;

import android.app.Activity;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.Toast;

import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ContextTemplate;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.utils.Converter_YUV_NV_2_ARGB;

public class NewPersonActivity extends Activity implements TheCameraPainter {

    private static final String TAG = "NewPerson";
    private static final String CTEMPLATE_EXT = ".ctemplate";

    private TheCamera camera = null;
    private FacerecService service = null;
    private ProcessingBlock detector = null;
    private ProcessingBlock fitter = null;
    private ProcessingBlock extractor = null;
    private ProcessingBlock utility = null;
    private ProcessingBlock qualityControl = null;
    private boolean faceAccepted = false;

    private ImageView mainImageView = null;
    private ImageView currentFaceImageView = null;
    private ImageView bestFaceImageView = null;

    private EditText nameEditText = null;

    private Bitmap bestImage = null;
    private float[] bestFacePoints = null;
    private byte[] bestFaceTemplateBytes = null;
    private String bestFaceTemplateMethod = null;

    private final String points_postfix = ".detected_points.txt";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.new_person);

        mainImageView = findViewById(R.id.mainImageView);
        currentFaceImageView = findViewById(R.id.currentFaceImageView);
        bestFaceImageView = findViewById(R.id.bestFaceImageView);

        service = MainActivity.getService();

        SharedComponents shared = SharedComponents.get();
        fitter = shared.getFitter();
        extractor = shared.getExtractor();
        utility = shared.getUtility();

        detector = createBlock(service, "FACE_DETECTOR", "ssyv_light");
        qualityControl = createBlock(service, "QUALITY_CONTROL", "core");

        nameEditText = findViewById(R.id.name_editText);

        final Handler handler = new Handler();
        Button addButton = findViewById(R.id.add_button);
        addButton.setOnClickListener(v -> {

            String name = nameEditText.getText().toString();
            if (name.isEmpty()) {
                Toast.makeText(getApplicationContext(), "Add name", Toast.LENGTH_SHORT).show();
                return;
            }

            if (bestImage == null) {
                Toast.makeText(getApplicationContext(), "Face not detected!", Toast.LENGTH_SHORT).show();
                return;
            }

            try {
                final String DATABASE_DIR = getApplicationInfo().dataDir + "/fsdk/database/";

                File new_person_dir = new File(DATABASE_DIR, name);

                final File db_file = new File(DATABASE_DIR);
                final File[] persons_array = db_file.listFiles();
                if (persons_array != null && Arrays.asList(persons_array).contains(new_person_dir)) {
                    Toast.makeText(getApplicationContext(), "This person exists\nAdding new photo", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(getApplicationContext(), "Create new person", Toast.LENGTH_SHORT).show();
                    new_person_dir.mkdir();

                    File name_file = new File(new_person_dir, "name.txt");
                    PrintWriter writer = new PrintWriter(name_file.getAbsolutePath(), "UTF-8");
                    writer.println(name);
                    writer.close();
                }

                int images_count = -1;
                File new_photo_file;
                File new_points_file;
                do {
                    images_count++;
                    final String short_new_photo_filename = Integer.toString(images_count) + ".jpg";
                    new_photo_file = new File(new_person_dir, short_new_photo_filename);
                    new_points_file = new File(new_person_dir, short_new_photo_filename + points_postfix);
                } while (new_photo_file.exists() || new_points_file.exists());

                final String new_photo_filename = new_photo_file.getAbsolutePath();

                final String points_filename = new_photo_filename + points_postfix;
                PrintWriter points_writer = new PrintWriter(points_filename, "UTF-8");
                if (bestFacePoints != null) {
                    for (int i = 0; i < bestFacePoints.length; i += 2) {
                        points_writer.print(bestFacePoints[i]);
                        points_writer.print(" ");
                        points_writer.println(bestFacePoints[i + 1]);
                    }
                }
                points_writer.close();

                FileOutputStream out = new FileOutputStream(new_photo_filename);
                bestImage.compress(Bitmap.CompressFormat.JPEG, 100, out);
                out.close();

                if (bestFaceTemplateBytes != null) {
                    final String template_filename = new_photo_file.getAbsolutePath() + CTEMPLATE_EXT;
                    ByteArrayInputStream bais = new ByteArrayInputStream(bestFaceTemplateBytes);
                    ContextTemplate templ = service.loadContextTemplate(bais);
                    bais.close();
                    FileOutputStream template_file = new FileOutputStream(template_filename);
                    templ.save(template_file);
                    template_file.close();
                    templ.dispose();
                }

                Database db = MainActivity.getDatabase();
                if (db != null && bestFaceTemplateBytes != null) {
                    ByteArrayInputStream bais2 = new ByteArrayInputStream(bestFaceTemplateBytes);
                    ContextTemplate cTempl = service.loadContextTemplate(bais2);
                    bais2.close();
                    String uuid = new_person_dir.getName() + "_" + new_photo_file.getName();
                    db.index.addContextTemplate(cTempl, uuid);
                    cTempl.dispose();

                    ByteArrayOutputStream jpg = new ByteArrayOutputStream();
                    bestImage.compress(Bitmap.CompressFormat.JPEG, 100, jpg);
                    Bitmap thumb = Database.makeThumbnail(jpg.toByteArray(), name);
                    db.thumbnails.add(thumb);
                    db.names.add(name);
                    db.uuids.add(uuid);
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            handler.postDelayed(this::finish, Toast.LENGTH_SHORT);
        });

        camera = new TheCamera(this);
        camera.open(this);
    }

    @Override
    public void processingImage(byte[] data, int width, int height) {
        if (detector == null || fitter == null || extractor == null || qualityControl == null || utility == null)
            return;

        int[] argb = Converter_YUV_NV_2_ARGB.convert_yuv_nv_2_argb(false, data, width, height);
        Bitmap immut_bitmap = Bitmap.createBitmap(argb, width, height, Bitmap.Config.ARGB_8888);
        mainImageView.setImageBitmap(immut_bitmap);

        if (faceAccepted) return;

        Context ioData = service.createContextFromFrame(
            data, width, height, Context.Format.FORMAT_NV21, 0);

        try {
            detector.process(ioData);

            if (ioData.get("objects").size() != 1) {
                return;
            }

            Context obj = ioData.get("objects").get(0);
            if (!obj.get("class").getString().equals("face")) return;

            fitter.process(ioData);
            qualityControl.process(ioData);

            if (!obj.get("quality").get("value").getBool()) return;

            extractor.process(ioData);
            utility.process(ioData);

            Context faceCropCtx = obj.get("face_crop");
            byte[] rgbData = faceCropCtx.get("blob").getBytes();
            int cropH = (int) faceCropCtx.get("shape").get(0).getLong();
            int cropW = (int) faceCropCtx.get("shape").get(1).getLong();
            int[] faceArgb = new int[cropW * cropH];
            for (int j = 0; j < cropW * cropH; j++) {
                faceArgb[j] = 0xFF000000
                    | ((rgbData[j * 3] & 0xFF) << 16)
                    | ((rgbData[j * 3 + 1] & 0xFF) << 8)
                    | (rgbData[j * 3 + 2] & 0xFF);
            }
            Bitmap cropBitmap = Bitmap.createBitmap(faceArgb, cropW, cropH, Bitmap.Config.ARGB_8888);
            currentFaceImageView.setImageBitmap(cropBitmap);
            Bitmap bestFaceCut = cropBitmap.copy(cropBitmap.getConfig(), true);
            bestFaceImageView.setImageBitmap(bestFaceCut);
            bestImage = cropBitmap;

            Context keypoints = obj.get("keypoints");
            Context points = keypoints.get("points");
            bestFacePoints = new float[(int) points.size() * 2];
            for (int j = 0; j < (int) points.size(); j++) {
                Context pt = points.get(j);
                bestFacePoints[j * 2] = (float) (pt.get("proj").get(0).getDouble() * width);
                bestFacePoints[j * 2 + 1] = (float) (pt.get("proj").get(1).getDouble() * height);
            }

            ContextTemplate templ = obj.get("face_template").get("template").getContextTemplate();
            bestFaceTemplateMethod = templ.getMethodName();
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            templ.save(baos);
            bestFaceTemplateBytes = baos.toByteArray();

            faceAccepted = true;
            findViewById(R.id.add_layout).setVisibility(View.VISIBLE);
        } finally {
            ioData.dispose();
        }
    }

    @Override
    protected void onPause() {
        camera.close();
        if (detector != null) detector.dispose();
        detector = null;
        if (qualityControl != null) qualityControl.dispose();
        qualityControl = null;
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        finish();
        super.onDestroy();
    }

    private ProcessingBlock createBlock(FacerecService s, String type, String mod) {
        Context c = s.createContext();
        c.get("unit_type").setString(type);
        if (!mod.isEmpty()) c.get("modification").setString(mod);
        ProcessingBlock block = s.createProcessingBlock(c);
        c.dispose();
        return block;
    }
}
