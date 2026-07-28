import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.ContextTemplate;
import com.vdt.face_recognition.sdk.SDKException;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class face_verification {
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
        ProcessingBlock extractor = null;
        ProcessingBlock verifier = null;
        Context ioData1 = null;
        Context ioData2 = null;
        Context verData = null;
        try {
            service = FacerecService.createService(
                "../lib/libfacerec.so", "../conf/facerec", "../license");

            detector = createBlock(service, "FACE_DETECTOR", "ssyv_light");
            fitter = createBlock(service, "FACE_FITTER", "fda");
            extractor = createBlock(service, "FACE_TEMPLATE_EXTRACTOR", "1000");
            verifier = createBlock(service, "VERIFICATION_MODULE", "1000");

            // Process first face
            byte[] bytes1 = Files.readAllBytes(Paths.get("face1.jpg"));
            ioData1 = service.createContextFromEncodedImage(bytes1);
            detector.process(ioData1);
            fitter.process(ioData1);
            extractor.process(ioData1);
            if (ioData1.get("objects").size() == 0) {
                System.out.println("No face in face1.jpg");
                return;
            }

            // Process second face
            byte[] bytes2 = Files.readAllBytes(Paths.get("face2.jpg"));
            ioData2 = service.createContextFromEncodedImage(bytes2);
            detector.process(ioData2);
            fitter.process(ioData2);
            extractor.process(ioData2);
            if (ioData2.get("objects").size() == 0) {
                System.out.println("No face in face2.jpg");
                return;
            }

            // Compare templates
            verData = service.createContext();
            verData.get("template1").setContextTemplate(
                ioData1.get("objects").get(0).get("face_template").getContextTemplate());
            verData.get("template2").setContextTemplate(
                ioData2.get("objects").get(0).get("face_template").getContextTemplate());
            verifier.process(verData);

            double score = verData.get("result").get("score").getDouble();
            double distance = verData.get("result").get("distance").getDouble();
            System.out.printf("Score: %.4f Distance: %.4f%n", score, distance);
            System.out.println("Verdict: " + (score >= 0.85 ? "match" : "no match"));
        } catch (SDKException | IOException e) {
            System.err.println("Error: " + e.getMessage());
        } finally {
            if (verData != null) verData.dispose();
            if (ioData1 != null) ioData1.dispose();
            if (ioData2 != null) ioData2.dispose();
            if (detector != null) detector.dispose();
            if (fitter != null) fitter.dispose();
            if (extractor != null) extractor.dispose();
            if (verifier != null) verifier.dispose();
            if (service != null) service.dispose();
        }
    }
}
