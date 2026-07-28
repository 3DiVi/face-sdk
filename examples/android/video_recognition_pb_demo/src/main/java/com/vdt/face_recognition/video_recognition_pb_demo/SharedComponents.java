package com.vdt.face_recognition.video_recognition_pb_demo;

import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.ProcessingBlock;

public class SharedComponents {

    private static SharedComponents instance = null;

    private final ProcessingBlock fitter;
    private final ProcessingBlock extractor;
    private final ProcessingBlock utility;

    private SharedComponents(FacerecService service) {
        fitter = createBlock(service, "FACE_FITTER", "fda");
        extractor = createBlock(service, "FACE_TEMPLATE_EXTRACTOR", "50m");

        Context ucfg = service.createContext();
        ucfg.get("unit_type").setString("UTILITY_MODULE");
        ucfg.get("modification").setString("face_cut");
        ucfg.get("cut_type").setString("FACE_CUT_BASE");
        utility = service.createProcessingBlock(ucfg);
        ucfg.dispose();
    }

    public static synchronized void create(FacerecService service) {
        if (instance == null) {
            instance = new SharedComponents(service);
        }
    }

    public static synchronized SharedComponents get() {
        return instance;
    }

    public static synchronized void dispose() {
        if (instance != null) {
            instance.fitter.dispose();
            instance.extractor.dispose();
            instance.utility.dispose();
            instance = null;
        }
    }

    public ProcessingBlock getFitter() { return fitter; }
    public ProcessingBlock getExtractor() { return extractor; }
    public ProcessingBlock getUtility() { return utility; }

    private static ProcessingBlock createBlock(FacerecService s, String type, String mod) {
        Context c = s.createContext();
        c.get("unit_type").setString(type);
        if (!mod.isEmpty()) c.get("modification").setString(mod);
        ProcessingBlock block = s.createProcessingBlock(c);
        c.dispose();
        return block;
    }
}
