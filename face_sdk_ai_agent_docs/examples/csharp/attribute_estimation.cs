using System;
using System.IO;
using System.Collections.Generic;
using VDT.FaceRecognition.SDK;

class attribute_estimation
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
            using var ageEst = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "AGE_ESTIMATOR" }
            });
            using var genderEst = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "GENDER_ESTIMATOR" }
            });
            using var emotionEst = service.CreateProcessingBlock(new Dictionary<object, object>
            {
                { "unit_type", "EMOTION_ESTIMATOR" }
            });

            using Context ioData = service.CreateContextFromEncodedImage(
                File.ReadAllBytes("input.jpg"));
            detector.Invoke(ioData);
            fitter.Invoke(ioData);
            ageEst.Invoke(ioData);
            genderEst.Invoke(ioData);
            emotionEst.Invoke(ioData);

            if ((int)ioData["objects"].Length() == 0)
            {
                Console.WriteLine("No faces detected");
                return;
            }

            for (int j = 0; j < (int)ioData["objects"].Length(); j++)
            {
                Context obj = ioData["objects"][j];
                Console.WriteLine($"Face {j + 1}:");
                Console.WriteLine($"  Age: {obj["age"].GetLong()}");
                Console.WriteLine($"  Gender: {obj["gender"].GetString()}");
                Console.WriteLine("  Emotions:");
                for (int k = 0; k < (int)obj["emotions"].Length(); k++)
                {
                    Context em = obj["emotions"][k];
                    Console.WriteLine($"    {em["emotion"].GetString()}: {em["confidence"].GetDouble():F3}");
                }
            }
        }
        catch (ProcessingBlockError e)
        {
            Console.WriteLine($"SDK error: {e.Message}");
        }
    }
}
