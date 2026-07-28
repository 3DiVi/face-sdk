import com.vdt.face_recognition.sdk.FacerecService;
import com.vdt.face_recognition.sdk.Context;
import com.vdt.face_recognition.sdk.ProcessingBlock;
import com.vdt.face_recognition.sdk.ContextTemplate;
import com.vdt.face_recognition.sdk.DynamicTemplateIndex;
import com.vdt.face_recognition.sdk.SDKException;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class face_identification {
    public static void main(String[] args) {
        FacerecService service = null;
        ProcessingBlock detector = null;
        ProcessingBlock fitter = null;
        ProcessingBlock extractor = null;
        ProcessingBlock matcher = null;
        DynamicTemplateIndex index = null;
        try {
            // Adjust paths to your SDK installation directory
            // sdk_root is provided by the user
            service = FacerecService.createService(
                "../lib/libfacerec.so", "../conf/facerec", "../license");

            detector = createBlock(service, "FACE_DETECTOR", "ssyv_light");
            fitter = createBlock(service, "FACE_FITTER", "fda");
            extractor = createBlock(service, "FACE_TEMPLATE_EXTRACTOR", "1000");
            matcher = createBlock(service, "MATCHER_MODULE", "1000");

            // Create DynamicTemplateIndex and enroll faces
            Context indexConfig = service.createContext();
            indexConfig.get("modification").setString("1000");
            indexConfig.get("version").setLong(1L);
            indexConfig.get("max_license_count").setLong(10000);
            index = service.createDynamicTemplateIndex(indexConfig);

            String[] enrollImages = {"face1.jpg", "face2.jpg", "face3.jpg"};
            for (String imgPath : enrollImages) {
                byte[] bytes = Files.readAllBytes(Paths.get(imgPath));
                Context ioData = service.createContextFromEncodedImage(bytes);
                detector.process(ioData);
                fitter.process(ioData);
                extractor.process(ioData);
                for (int j = 0; j < ioData.get("objects").size(); j++) {
                    Context obj = ioData.get("objects").get(j);
                    if (!obj.get("class").getString().equals("face")) continue;
                    ContextTemplate templ = obj.get("face_template")
                        .get("template").getContextTemplate();
                    String uuid = obj.get("face_template").get("uuid").getString();
                    index.addContextTemplate(templ, uuid);
                    System.out.println("Enrolled: " + uuid);
                }
                ioData.dispose();
            }

            // Search with query face
            byte[] queryBytes = Files.readAllBytes(Paths.get("query.jpg"));
            Context queryData = service.createContextFromEncodedImage(queryBytes);
            try {
                detector.process(queryData);
                fitter.process(queryData);
                extractor.process(queryData);

                if (queryData.get("objects").size() == 0) {
                    System.out.println("No faces detected");
                    return;
                }

                Context matcherData = service.createContext();
                matcherData.get("knn").setLong(5L);
                matcherData.get("template_index").setDynamicTemplateIndex(index);
                matcherData.get("queries").setContextTemplate(
                    queryData.get("objects").get(0).get("face_template")
                        .get("template").getContextTemplate());
                matcher.process(matcherData);

                for (int i = 0; i < matcherData.get("results").size(); i++) {
                    Context result = matcherData.get("results").get(i);
                    long idx = result.get("index").getLong();
                    double score = result.get("score").getDouble();
                    double distance = result.get("distance").getDouble();
                    System.out.printf("Match index: %d Score: %.4f Distance: %.4f%n",
                        idx, score, distance);
                }

                matcherData.dispose();
            } finally {
                queryData.dispose();
            }
        } catch (SDKException | IOException e) {
            System.err.println("Error: " + e.getMessage());
        } finally {
            if (index != null) index.dispose();
            if (detector != null) detector.dispose();
            if (fitter != null) fitter.dispose();
            if (extractor != null) extractor.dispose();
            if (matcher != null) matcher.dispose();
            if (service != null) service.dispose();
        }
    }

    static ProcessingBlock createBlock(FacerecService s, String type, String mod) {
        Context c = s.createContext();
        c.get("unit_type").setString(type);
        if (!mod.isEmpty()) c.get("modification").setString(mod);
        return s.createProcessingBlock(c);
    }
}
