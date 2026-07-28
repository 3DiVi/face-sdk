#include <facerec/libfacerec.h>
#include <iostream>
#include <fstream>
#include <vector>

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
}

int main() {
    try {
        const std::string dll_path = "../lib/libfacerec.so";
        const std::string config_dir = "../conf/facerec";
        const std::string license_dir = "../license";

        auto service = pbio::FacerecService::createService(dll_path, config_dir, license_dir);

        auto mkBlock = [&](const std::string& type, const std::string& mod = "") {
            pbio::Context c = service->createContext();
            c["unit_type"] = type;
            if (!mod.empty()) c["modification"] = mod;
            return service->createProcessingBlock(c);
        };

        auto detector = mkBlock("FACE_DETECTOR", "ssyv_light");
        auto fitter = mkBlock("FACE_FITTER", "fda");
        auto extractor = mkBlock("FACE_TEMPLATE_EXTRACTOR", "1000");
        auto verifier = mkBlock("VERIFICATION_MODULE", "1000");

        // Process first face
        auto data1 = readFile("face1.jpg");
        pbio::Context ioData1 = service->createContextFromEncodedImage(data1);
        detector(ioData1);
        fitter(ioData1);
        extractor(ioData1);
        if (ioData1["objects"].size() == 0) {
            std::cout << "No face in face1.jpg" << std::endl;
            return 1;
        }

        // Process second face
        auto data2 = readFile("face2.jpg");
        pbio::Context ioData2 = service->createContextFromEncodedImage(data2);
        detector(ioData2);
        fitter(ioData2);
        extractor(ioData2);
        if (ioData2["objects"].size() == 0) {
            std::cout << "No face in face2.jpg" << std::endl;
            return 1;
        }

        // Compare templates
        pbio::Context verData = service->createContext();
        verData["template1"] = ioData1["objects"][0]["face_template"];
        verData["template2"] = ioData2["objects"][0]["face_template"];
        verifier(verData);

        double score = verData["result"]["score"].getDouble();
        double distance = verData["result"]["distance"].getDouble();
        std::cout << "Score: " << score << " Distance: " << distance << std::endl;
        std::cout << "Verdict: " << (score >= 0.85 ? "match" : "no match") << std::endl;

        return 0;
    } catch (const pbio::Error& e) {
        std::cerr << "SDK error: " << e.what() << std::endl;
        return 1;
    }
}
