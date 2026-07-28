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

    // sdk_root is provided by the user
    // Adjust paths to your SDK installation directory
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
    auto matcher = mkBlock("MATCHER_MODULE", "1000");

    // Create DynamicTemplateIndex and enroll faces
    pbio::Context indexConfig = service->createContext();
    indexConfig["modification"] = "1000";
    indexConfig["version"] = 1l;
    indexConfig["max_license_count"] = 10000;
    auto index = service->createDynamicTemplateIndex(indexConfig);

    std::vector<std::string> enroll_images = {"face1.jpg", "face2.jpg", "face3.jpg"};
    for (const auto& img_path : enroll_images) {
        auto data = readFile(img_path);
        pbio::Context ioData = service->createContextFromEncodedImage(data);
        detector(ioData);
        fitter(ioData);
        extractor(ioData);
        for (const auto& obj : ioData["objects"]) {
            if (obj["class"].getString() != "face") continue;
            auto templ = obj["face_template"]["template"].getContextTemplate();
            std::string uuid = obj["face_template"]["uuid"].getString();
            index->add(templ, uuid);
            std::cout << "Enrolled: " << uuid << std::endl;
        }
    }

    // Search with query face
    auto query_data = readFile("query.jpg");
    pbio::Context queryData = service->createContextFromEncodedImage(query_data);
    detector(queryData);
    fitter(queryData);
    extractor(queryData);

    pbio::Context matcherData = service->createContext();
    matcherData["knn"] = 5l;
    matcherData["template_index"].setDynamicTemplateIndex(index);
    if (queryData["objects"].size() == 0) {
        std::cout << "No faces detected" << std::endl;
        return 1;
    }
    matcherData["queries"] = queryData["objects"][0]["face_template"]["template"];
    matcher(matcherData);

    for (const auto& result : matcherData["results"]) {
        long idx = result["index"].getLong();
        double score = result["score"].getDouble();
        double distance = result["distance"].getDouble();
        std::cout << "Match index: " << idx << " Score: " << score
                  << " Distance: " << distance << std::endl;
    }

    return 0;
    } catch (const pbio::Error& e) {
        std::cerr << "SDK error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
