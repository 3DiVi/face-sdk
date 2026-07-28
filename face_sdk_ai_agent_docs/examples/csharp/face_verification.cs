using System;
using System.IO;
using System.Collections.Generic;
using VDT.FaceRecognition.SDK;

class face_verification
{
    static void Main()
    {
        try
        {
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
            using var verifier = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "VERIFICATION_MODULE" },
                { "modification", "1000" }
            });

            // Process first face
            using Context ioData1 = service.CreateContextFromEncodedImage(
                File.ReadAllBytes("face1.jpg"));
            detector.Invoke(ioData1);
            fitter.Invoke(ioData1);
            extractor.Invoke(ioData1);
            if ((int)ioData1["objects"].Length() == 0)
            {
                Console.WriteLine("No face in face1.jpg");
                return;
            }

            // Process second face
            using Context ioData2 = service.CreateContextFromEncodedImage(
                File.ReadAllBytes("face2.jpg"));
            detector.Invoke(ioData2);
            fitter.Invoke(ioData2);
            extractor.Invoke(ioData2);
            if ((int)ioData2["objects"].Length() == 0)
            {
                Console.WriteLine("No face in face2.jpg");
                return;
            }

            // Compare templates
            using Context verData = service.CreateContext(new Dictionary<object, object>
            {
                { "template1", ioData1["objects"][0]["face_template"] },
                { "template2", ioData2["objects"][0]["face_template"] }
            });
            verifier.Invoke(verData);

            double score = verData["result"]["score"].GetDouble();
            double distance = verData["result"]["distance"].GetDouble();
            Console.WriteLine($"Score: {score:F4} Distance: {distance:F4}");
            Console.WriteLine($"Verdict: {(score >= 0.85 ? "match" : "no match")}");
        }
        catch (ProcessingBlockError e)
        {
            Console.WriteLine($"SDK error: {e.Message}");
        }
    }
}
