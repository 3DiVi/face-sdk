using System;
using System.IO;
using System.Collections.Generic;
using VDT.FaceRecognition.SDK;

class face_identification
{
    static void Main()
    {
        try
        {
            // Adjust paths to your SDK installation directory
            // sdk_root is provided by the user
            using FacerecService service = FacerecService.createService(
                "../conf/facerec/", "../license/");

            using var detector = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "FACE_DETECTOR" },
                { "modification", "ssyv_light" }
            });
            using var fitter = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "FACE_FITTER" },
                { "modification", "fda" }
            });
            using var extractor = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "FACE_TEMPLATE_EXTRACTOR" },
                { "modification", "1000" }
            });
            using var matcher = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "MATCHER_MODULE" },
                { "modification", "1000" }
            });

            // Create DynamicTemplateIndex and enroll faces
            using Context indexConfig = service.CreateContext(
                new Dictionary<object, object>
                {
                    { "modification", "1000" },
                    { "version", 1L },
                    { "max_license_count", 10000L }
                });
            using DynamicTemplateIndex index = service.CreateDynamicTemplateIndex(indexConfig);

            foreach (string imgPath in new[] { "face1.jpg", "face2.jpg", "face3.jpg" })
            {
                using Context ioData = service.CreateContextFromEncodedImage(
                    File.ReadAllBytes(imgPath));
                detector.Invoke(ioData);
                fitter.Invoke(ioData);
                extractor.Invoke(ioData);

                for (int j = 0; j < (int)ioData["objects"].Length(); j++)
                {
                    Context obj = ioData["objects"][j];
                    if (obj["class"].GetString() != "face") continue;
                    ContextTemplate templ = obj["face_template"]["template"]
                        .GetContextTemplate();
                    string uuid = obj["face_template"]["uuid"].GetString();
                    index.Add(templ, uuid);
                    Console.WriteLine($"Enrolled: {uuid}");
                }
            }

            // Search with query face
            using Context queryData = service.CreateContextFromEncodedImage(
                File.ReadAllBytes("query.jpg"));
            detector.Invoke(queryData);
            fitter.Invoke(queryData);
            extractor.Invoke(queryData);

            if ((int)queryData["objects"].Length() == 0) {
                Console.WriteLine("No faces detected");
                return;
            }

            using Context matcherData = service.CreateContext(
                new Dictionary<object, object>
                {
                    { "knn", 5L },
                    { "template_index", index },
                    { "queries", queryData["objects"][0]["face_template"]["template"] }
                });
            matcher.Invoke(matcherData);

            for (int i = 0; i < (int)matcherData["results"].Length(); i++)
            {
                Context result = matcherData["results"][i];
                long idx = result["index"].GetLong();
                double score = result["score"].GetDouble();
                double distance = result["distance"].GetDouble();
                Console.WriteLine($"Match index: {idx} Score: {score:F4} " +
                    $"Distance: {distance:F4}");
            }
        }
        catch (ProcessingBlockError e)
        {
            Console.WriteLine($"SDK error: {e.Message}");
        }
    }
}
