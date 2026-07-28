import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.SDKException;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class face_detection {
    public static void main(String[] args) {
        FacerecService service = null;
        ProcessingBlock detector = null;
        Context ioData = null;
        try {
            service = FacerecService.createService(
                "../lib/libfacerec.so", "../conf/facerec", "../license");

            Context config = service.createContext();
            config.get("unit_type").setString("FACE_DETECTOR");
            config.get("modification").setString("ssyv_light");
            detector = service.createProcessingBlock(config);

            byte[] bytes = Files.readAllBytes(Paths.get("input.jpg"));
            ioData = service.createContextFromEncodedImage(bytes);
            detector.process(ioData);

            for (int j = 0; j < ioData.get("objects").size(); j++) {
                Context obj = ioData.get("objects").get(j);
                Context bbox = obj.get("bbox");
                long w = ioData.get("image").get("shape").get(1).getLong();
                long h = ioData.get("image").get("shape").get(0).getLong();
                double conf = obj.get("confidence").getDouble();
                System.out.printf("Face: (%d, %d)-(%d, %d) confidence: %.3f%n",
                    (int)(bbox.get(0).getDouble() * w),
                    (int)(bbox.get(1).getDouble() * h),
                    (int)(bbox.get(2).getDouble() * w),
                    (int)(bbox.get(3).getDouble() * h), conf);
            }

            if (ioData.get("objects").size() == 0) {
                System.out.println("No faces detected");
            }
        } catch (SDKException | IOException e) {
            System.err.println("Error: " + e.getMessage());
        } finally {
            if (ioData != null) ioData.dispose();
            if (detector != null) detector.dispose();
            if (service != null) service.dispose();
        }
    }
}
