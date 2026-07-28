using System;
using System.IO;
using System.Collections.Generic;
using VDT.FaceRecognition.SDK;

class face_detection
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

            using Context ioData = service.CreateContextFromEncodedImage(
                File.ReadAllBytes("input.jpg"));
            detector.Invoke(ioData);

            for (int j = 0; j < (int)ioData["objects"].Length(); j++)
            {
                Context obj = ioData["objects"][j];
                Context bbox = obj["bbox"];
                long w = ioData["image"]["shape"][1].GetLong();
                long h = ioData["image"]["shape"][0].GetLong();
                double conf = obj["confidence"].GetDouble();
                Console.WriteLine($"Face: ({bbox[0].GetDouble() * w:F0}, " +
                    $"{bbox[1].GetDouble() * h:F0})-({bbox[2].GetDouble() * w:F0}, " +
                    $"{bbox[3].GetDouble() * h:F0}) confidence: {conf:F3}");
            }

            if ((int)ioData["objects"].Length() == 0)
            {
                Console.WriteLine("No faces detected");
                return;
            }
        }
        catch (ProcessingBlockError e)
        {
            Console.WriteLine($"SDK error: {e.Message}");
        }
    }
}
