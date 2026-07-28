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

        pbio::Context config = service->createContext();
        config["unit_type"] = "FACE_DETECTOR";
        config["modification"] = "ssyv_light";
        auto detector = service->createProcessingBlock(config);

        auto data = readFile("input.jpg");
        pbio::Context ioData = service->createContextFromEncodedImage(data);
        detector(ioData);

        for (const auto& obj : ioData["objects"]) {
            auto bbox = obj["bbox"];
            long w = ioData["image"]["shape"][1].getLong();
            long h = ioData["image"]["shape"][0].getLong();
            double conf = obj["confidence"].getDouble();
            std::cout << "Face: (" << int(bbox[0].getDouble() * w) << ", "
                      << int(bbox[1].getDouble() * h) << ")-("
                      << int(bbox[2].getDouble() * w) << ", "
                      << int(bbox[3].getDouble() * h) << ")"
                      << " confidence: " << conf << std::endl;
        }

        if (ioData["objects"].size() == 0) {
            std::cout << "No faces detected" << std::endl;
            return 1;
        }

        return 0;
    } catch (const pbio::Error& e) {
        std::cerr << "SDK error: " << e.what() << std::endl;
        return 1;
    }
}
