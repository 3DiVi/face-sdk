import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.SDKException;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class attribute_estimation {
    static ProcessingBlock createBlock(FacerecService s, String type, String mod) {
        Context c = s.createContext();
        c.get("unit_type").setString(type);
        if (!mod.isEmpty()) c.get("modification").setString(mod);
        return s.createProcessingBlock(c);
    }

    public static void main(String[] args) {
        FacerecService service = null;
        ProcessingBlock detector = null;
        ProcessingBlock fitter = null;
        ProcessingBlock ageEst = null;
        ProcessingBlock genderEst = null;
        ProcessingBlock emotionEst = null;
        Context ioData = null;
        try {
            service = FacerecService.createService(
                "../lib/libfacerec.so", "../conf/facerec", "../license");

            detector = createBlock(service, "FACE_DETECTOR", "ssyv_light");
            fitter = createBlock(service, "FACE_FITTER", "fda");
            ageEst = createBlock(service, "AGE_ESTIMATOR", "");
            genderEst = createBlock(service, "GENDER_ESTIMATOR", "");
            emotionEst = createBlock(service, "EMOTION_ESTIMATOR", "");

            byte[] bytes = Files.readAllBytes(Paths.get("input.jpg"));
            ioData = service.createContextFromEncodedImage(bytes);
            detector.process(ioData);
            fitter.process(ioData);
            ageEst.process(ioData);
            genderEst.process(ioData);
            emotionEst.process(ioData);

            if (ioData.get("objects").size() == 0) {
                System.out.println("No faces detected");
                return;
            }

            for (int j = 0; j < ioData.get("objects").size(); j++) {
                Context obj = ioData.get("objects").get(j);
                System.out.println("Face " + (j + 1) + ":");
                if (obj.contains("age"))
                    System.out.println("  Age: " + obj.get("age").getLong());
                if (obj.contains("gender"))
                    System.out.println("  Gender: " + obj.get("gender").getString());
                if (obj.contains("emotions"))
                    for (int k = 0; k < obj.get("emotions").size(); k++) {
                        Context em = obj.get("emotions").get(k);
                        System.out.printf("  %s: %.3f%n",
                            em.get("emotion").getString(),
                            em.get("confidence").getDouble());
                    }
            }
        } catch (SDKException | IOException e) {
            System.err.println("Error: " + e.getMessage());
        } finally {
            if (ioData != null) ioData.dispose();
            if (detector != null) detector.dispose();
            if (fitter != null) fitter.dispose();
            if (ageEst != null) ageEst.dispose();
            if (genderEst != null) genderEst.dispose();
            if (emotionEst != null) emotionEst.dispose();
            if (service != null) service.dispose();
        }
    }
}
