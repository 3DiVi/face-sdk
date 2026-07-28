package com.vdt.face_recognition.video_recognition_pb_demo;

import java.lang.Math;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;
import java.util.SortedSet;
import java.util.TreeSet;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.DashPathEffect;
import android.graphics.Paint;
import android.os.Handler;
import android.util.Log;
import android.widget.ImageView;

import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ContextTemplate;
import com.vdt.face_recognition.sdk.DynamicTemplateIndex;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.utils.Converter_YUV_NV_2_ARGB;

public class VidRecDemo implements TheCameraPainter {

    private static final String TAG = "VidRecDemo";
    private static final long MATCH_INTERVAL_MS = 2000;

    private final MainActivity activity;
    private ImageView mainImageView = null;
    private final java.util.Vector<ImageView> vid_faces_image_views = new java.util.Vector<>();
    private final java.util.Vector<ImageView> db_faces_image_views = new java.util.Vector<>();

    private Database db = null;

    private final FacerecService service;
    private ProcessingBlock tracker = null;
    private final ProcessingBlock fitter;
    private final ProcessingBlock extractor;
    private ProcessingBlock matcher = null;
    private final ProcessingBlock utility;
    private DynamicTemplateIndex index = null;

    private final Thread init_thread;

    private final String modification;
    private final float threshold;

    private int frame_counter = 0;
    private final DrawingData drawingData = new DrawingData();
    private final HashMap<Long, Long> lastMatchTime = new HashMap<>();

    Thread drawThread = null;

    public VidRecDemo(final MainActivity activity) {
        this.activity = activity;

        android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_AUDIO);

        Log.v(TAG, "Start VidRecDemo");
        modification = "50m";
        threshold = 0.85f;

        service = MainActivity.getService();
        Log.v(TAG, "Loaded settings");

        SharedComponents shared = SharedComponents.get();
        fitter = shared.getFitter();
        extractor = shared.getExtractor();
        utility = shared.getUtility();

        Log.v(TAG, "Starting init thread");
        init_thread = new Thread(new Runnable() {
            public void run() {
                synchronized (init_thread) {
                    try {
                        Log.v(TAG, "init thread: using shared fitter, extractor, utility");

                        ProcessingBlock detector = createBlock(service, "FACE_DETECTOR", "ssyv_light");
                        Log.v(TAG, "init thread: created detector for db");

                        db = new Database(activity,
                            activity.getApplicationInfo().dataDir + "/fsdk/database",
                            service, detector, fitter, extractor);
                        Log.v(TAG, "init thread: created db");

                        index = db.index;

                        detector.dispose();
                        Log.v(TAG, "init thread: done");
                    } catch (Exception e) {
                        Log.e(TAG, "init thread error: " + e.getMessage());
                        e.printStackTrace();
                    }
                }
            }
        });
        init_thread.start();

        try {
            init_thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        matcher = createBlock(service, "MATCHER_MODULE", modification);
        Log.v(TAG, "created matcher");

        createTracker();
    }

    public void createTracker() {
        if (tracker == null) {
            tracker = createBlock(service, "FACE_TRACKING_MODULE", "fda");
            Log.v(TAG, "created tracker");
        }
    }

    public void disposeTracker() {
        if (tracker != null) {
            tracker.dispose();
            tracker = null;
            Log.v(TAG, "disposed tracker");
        }
    }

    @Override
    public void processingImage(byte[] data, int width, int height) {
        if (tracker == null || fitter == null || extractor == null || utility == null)
            return;

        int[] argb = Converter_YUV_NV_2_ARGB.convert_yuv_nv_2_argb(false, data, width, height);
        Bitmap bitmap = Bitmap.createBitmap(argb, width, height, Bitmap.Config.ARGB_8888);
        Bitmap mut_bitmap = bitmap.copy(Bitmap.Config.ARGB_8888, true);

        Context ioData = service.createContextFromFrame(
            data, width, height, Context.Format.FORMAT_NV21, 0);

        try {
            tracker.process(ioData);
            fitter.process(ioData);

            boolean needExtract = false;
            for (int i = 0; i < ioData.get("objects").size() && !needExtract; i++) {
                Context obj = ioData.get("objects").get(i);
                if (!obj.get("class").getString().equals("face")) continue;
                long trackId = obj.get("track_id").getLong();
                long now = System.currentTimeMillis();
                Long lastMatch = lastMatchTime.get(trackId);
                if (lastMatch == null || now - lastMatch > MATCH_INTERVAL_MS) {
                    needExtract = true;
                }
            }

            if (needExtract) {
                extractor.process(ioData);
                utility.process(ioData);
            }

            synchronized (drawingData) {
                drawingData.frame = mut_bitmap;
                drawingData.frame_id = frame_counter++;
                drawingData.updated = true;

                Set<Long> currentTrackIds = new HashSet<>();

                for (int i = 0; i < ioData.get("objects").size(); i++) {
                    Context obj = ioData.get("objects").get(i);
                    if (!obj.get("class").getString().equals("face")) continue;

                    long trackId = obj.get("track_id").getLong();
                    currentTrackIds.add(trackId);

                    if (!drawingData.faces.containsKey(trackId)) {
                        FaceData faceData = new FaceData(trackId);
                        drawingData.faces.put(trackId, faceData);
                    }

                    FaceData face = drawingData.faces.get(trackId);
                    assert face != null;
                    face.frame_id = drawingData.frame_id;
                    face.lost = false;

                    double x1 = obj.get("bbox").get(0).getDouble() * width;
                    double y1 = obj.get("bbox").get(1).getDouble() * height;
                    double x2 = obj.get("bbox").get(2).getDouble() * width;
                    double y2 = obj.get("bbox").get(3).getDouble() * height;
                    face.bbox = new float[]{(float) x1, (float) y1, (float) x2, (float) y2};

                    Context keypoints = obj.get("keypoints");
                    Context points = keypoints.get("points");
                    face.landmarks = new float[(int) points.size() * 2];
                    for (int j = 0; j < (int) points.size(); j++) {
                        Context pt = points.get(j);
                        face.landmarks[j * 2] = (float) (pt.get("proj").get(0).getDouble() * width);
                        face.landmarks[j * 2 + 1] = (float) (pt.get("proj").get(1).getDouble() * height);
                    }

                    if (needExtract) {
                        long now = System.currentTimeMillis();
                        if (index != null && index.size() > 0) {
                            ContextTemplate templ = obj.get("face_template").get("template").getContextTemplate();

                            Context matcherData = service.createContext();
                            try {
                                matcherData.get("template_index").setDynamicTemplateIndex(index);
                                matcherData.get("queries").setContextTemplate(templ);
                                matcherData.get("knn").setLong(1L);
                                matcher.process(matcherData);

                                if (matcherData.get("results").size() > 0) {
                                    Context result = matcherData.get("results").get(0);
                                    double score = result.get("score").getDouble();

                                    if (score >= threshold) {
                                        String uuid = result.get("uuid").getString();
                                        face.match_db_index = db.uuids.indexOf(uuid);
                                        face.match_uuid = uuid;
                                    } else {
                                        face.match_db_index = -1;
                                        face.match_uuid = null;
                                    }
                                } else {
                                    face.match_db_index = -1;
                                    face.match_uuid = null;
                                }
                            } finally {
                                matcherData.dispose();
                            }
                        }
                        lastMatchTime.put(trackId, now);

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
                        face.faceCrop = Bitmap.createBitmap(faceArgb, cropW, cropH, Bitmap.Config.ARGB_8888);
                    }

                    face.weak = obj.get("confidence").getDouble() < 0.5;
                }

                for (java.util.Map.Entry<Long, FaceData> entry : drawingData.faces.entrySet()) {
                    FaceData face = entry.getValue();
                    if (!currentTrackIds.contains(face.track_id) && !face.lost) {
                        face.lost = true;
                        face.lost_time = System.currentTimeMillis();
                    }
                }

            }
        } finally {
            ioData.dispose();
        }
    }

    public void startDrawThread(final Handler handler) {
        if (drawThread != null) {
            closeDrawThread();
        }

        mainImageView = activity.findViewById(R.id.mainImageView);
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView0));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView1));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView2));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView3));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView4));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView5));
        vid_faces_image_views.add(activity.findViewById(R.id.video_faces_ImageView6));

        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView0));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView1));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView2));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView3));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView4));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView5));
        db_faces_image_views.add(activity.findViewById(R.id.db_faces_ImageView6));

        drawThread = new Thread(() -> {
            while (true) {
                try {
                    Thread.sleep(20);
                } catch (InterruptedException ex) {
                    Thread.currentThread().interrupt();
                    break;
                }
                if (drawingData.updated) {
                    handler.sendEmptyMessage(0);
                }
            }
        });
        drawThread.start();
    }

    public void updateImageViews() {
        synchronized (drawingData) {
            int count = 0;
            SortedSet<Long> keys = new TreeSet<>(drawingData.faces.keySet());

            for (Long key : keys) {
                FaceData face = drawingData.faces.get(key);

                assert face != null;
                if (face.frame_id == drawingData.frame_id && !face.lost) {
                    float cx = 0, cy = 0;
                    int numPts = face.landmarks.length / 2;
                    for (int i = 0; i < numPts; i++) {
                        cx += face.landmarks[i * 2];
                        cy += face.landmarks[i * 2 + 1];
                    }
                    cx /= numPts;
                    cy /= numPts;

                    float radius = 0;
                    for (int i = 0; i < numPts; i++) {
                        float dx = face.landmarks[i * 2] - cx;
                        float dy = face.landmarks[i * 2 + 1] - cy;
                        radius += (float) Math.sqrt(dx * dx + dy * dy);
                    }
                    radius /= numPts;
                    radius *= 2;

                    Paint paint = new Paint();
                    paint.setStyle(Paint.Style.STROKE);
                    paint.setStrokeWidth(2);

                    if (face.weak) {
                        DashPathEffect dashPath = new DashPathEffect(new float[]{20, 10}, 0);
                        paint.setPathEffect(dashPath);
                    }

                    if (face.match_db_index >= 0) {
                        paint.setColor(0xff00ff00);
                    } else {
                        paint.setColor(0xffff0000);
                    }

                    Canvas main_canvas = new Canvas(drawingData.frame);
                    main_canvas.drawCircle(cx, cy, radius, paint);
                }

                Bitmap thumbnail_from_frame = face.faceCrop != null
                    ? face.faceCrop
                    : Bitmap.createBitmap(50, 50, Bitmap.Config.ARGB_8888);

                Bitmap thumbnail_from_db;
                if (face.match_db_index >= 0 && face.match_db_index < db.thumbnails.size()) {
                    thumbnail_from_db = db.thumbnails.get((int) face.match_db_index);
                } else {
                    thumbnail_from_db = Bitmap.createBitmap(
                        thumbnail_from_frame.getWidth(),
                        thumbnail_from_frame.getHeight(),
                        Bitmap.Config.ARGB_8888);
                    Canvas canvas = new Canvas(thumbnail_from_db);
                    Paint paint = new Paint();
                    paint.setColor(0xff555555);
                    canvas.drawRect(0, 0,
                        thumbnail_from_db.getWidth(),
                        thumbnail_from_db.getHeight(), paint);
                }

                if (face.lost) {
                    long cur_time = System.currentTimeMillis();
                    long alive_lost_time = 5000;
                    if (cur_time - face.lost_time > alive_lost_time) {
                        drawingData.faces.remove(key);
                        clearImageViews(count);
                        continue;
                    } else {
                        int alpha = (int) ((1 - (1.0 * cur_time - face.lost_time) / alive_lost_time) * 255);
                        vid_faces_image_views.get(count).setImageAlpha(alpha);
                        db_faces_image_views.get(count).setImageAlpha(alpha);
                    }
                }

                vid_faces_image_views.get(count).setImageBitmap(thumbnail_from_frame);
                db_faces_image_views.get(count).setImageBitmap(thumbnail_from_db);

                if (count >= 6)
                    break;
                count++;
            }

            mainImageView.setImageBitmap(drawingData.frame);
            drawingData.updated = false;
        }
    }

    public void closeDrawThread() {
        if (drawThread != null) {
            drawThread.interrupt();
        }
    }

    private void clearImageViews(int currentImageViewID) {
        for (int i = currentImageViewID; i < vid_faces_image_views.size(); i++) {
            vid_faces_image_views.get(i).setImageAlpha(0xff);
            vid_faces_image_views.get(i).setImageResource(android.R.color.transparent);
            db_faces_image_views.get(i).setImageAlpha(0xff);
            db_faces_image_views.get(i).setImageResource(android.R.color.transparent);
        }
    }

    public Database getDatabase() { return db; }

    public void dispose() {
        disposeTracker();
        if (matcher != null) matcher.dispose();
        if (index != null) index.dispose();
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
