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
        auto ageEst = mkBlock("AGE_ESTIMATOR");
        auto genderEst = mkBlock("GENDER_ESTIMATOR");
        auto emotionEst = mkBlock("EMOTION_ESTIMATOR");

        auto data = readFile("input.jpg");
        pbio::Context ioData = service->createContextFromEncodedImage(data);
        detector(ioData);
        fitter(ioData);
        ageEst(ioData);
        genderEst(ioData);
        emotionEst(ioData);

        if (ioData["objects"].size() == 0) {
            std::cout << "No faces detected" << std::endl;
            return 1;
        }

        for (const auto& obj : ioData["objects"]) {
            if (obj.contains("age"))
                std::cout << "Age: " << obj["age"].getLong() << std::endl;
            if (obj.contains("gender"))
                std::cout << "Gender: " << obj["gender"].getString() << std::endl;
            if (obj.contains("emotions"))
                for (const auto& em : obj["emotions"])
                    std::cout << em["emotion"].getString() << ": "
                              << em["confidence"].getDouble() << std::endl;
        }

        return 0;
    } catch (const pbio::Error& e) {
        std::cerr << "SDK error: " << e.what() << std::endl;
        return 1;
    }
}
