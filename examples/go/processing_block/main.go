package main

import (
	"facesdk"
	"flag"
	"fmt"
	"os"
)

type printResultFunction func(data map[string]any)

func main() {
	sdkPath := flag.String("sdk_path", "", "Path to Face SDK(required)")
	unitType := flag.String("unit_type", "face", "face|fitter|emotions|age|gender|mask|glasses|eye_openness|liveness|quality(required)")
	modification := flag.String("modification", "", "Modification of unit_type")
	imagePath := flag.String("image_path", "", "Path to image(required)")
	version := flag.Int("version", -1, "Version of modification of unit_type")

	flag.Parse()

	fmt.Println("All flags:")
	flag.VisitAll(func(f *flag.Flag) {
		fmt.Printf("  -%s: value=%q (default=%q) - %s\n", f.Name, f.Value, f.DefValue, f.Usage)
	})

	unitTypes := map[string]string{
		"face":         "FACE_DETECTOR",
		"fitter":       "FACE_FITTER",
		"emotions":     "EMOTION_ESTIMATOR",
		"age":          "AGE_ESTIMATOR",
		"gender":       "GENDER_ESTIMATOR",
		"mask":         "MASK_ESTIMATOR",
		"glasses":      "GLASSES_ESTIMATOR",
		"eye_openness": "EYE_OPENNESS_ESTIMATOR",
		"liveness":     "LIVENESS_ESTIMATOR",
		"deepfake":     "DEEPFAKE_ESTIMATOR",
		"quality":      "QUALITY_ASSESSMENT_ESTIMATOR",
	}
	printResults := map[string]printResultFunction{
		"FACE_DETECTOR":                printFaceDetector,
		"FACE_FITTER":                  printFaceFitter,
		"EMOTION_ESTIMATOR":            printEmotionEstimator,
		"AGE_ESTIMATOR":                printAgeEstimator,
		"GENDER_ESTIMATOR":             printGenderEstimator,
		"MASK_ESTIMATOR":               printMaskEstimator,
		"GLASSES_ESTIMATOR":            printGlassesEstimator,
		"EYE_OPENNESS_ESTIMATOR":       printEyeOpennessEstimator,
		"LIVENESS_ESTIMATOR":           printLivenessEstimator,
		"DEEPFAKE_ESTIMATOR":           printDeepFakeEstimator,
		"QUALITY_ASSESSMENT_ESTIMATOR": printQualityAssessmentEstimator,
	}

	if value, ok := unitTypes[*unitType]; ok {
		*unitType = value
	} else {
		panic("Not supported unit_type")
	}

	service, err := facesdk.CreateFacerecService(fmt.Sprintf("%s/conf/facerec", *sdkPath), fmt.Sprintf("%s/license", *sdkPath))
	checkError(err)

	pipeline := make([]facesdk.ProcessingBlock, 0, 3)

	if *unitType != "FACE_DETECTOR" {
		tempUnitType := "FACE_DETECTOR"
		tempModification := "ssyv_light"
		tempVersion := 1

		pipeline = append(pipeline, createProcessingBlock(service, &tempUnitType, &tempModification, &tempVersion))

		if *unitType != "FACE_FITTER" {
			tempUnitType = "FACE_FITTER"
			tempModification = "fda"
			tempVersion = 1

			pipeline = append(pipeline, createProcessingBlock(service, &tempUnitType, &tempModification, &tempVersion))
		}
	}

	pipeline = append(pipeline, createProcessingBlock(service, unitType, modification, version))

	imageBytes, err := os.ReadFile(*imagePath)
	checkError(err)

	data, err := facesdk.CreateContextFromEncodedImage(imageBytes)
	checkError(err)
	defer data.Close()

	for _, block := range pipeline {
		err = block.Process(data)
		defer block.Close()
		checkError(err)
	}

	result, err := data.ToMap()
	checkError(err)

	checkObjects(result)

	printResults[*unitType](result)
}

func checkError(err error) {
	if err != nil {
		panic(err)
	}
}

func checkObjects(data map[string]any) {
	if objects, ok := data["objects"].([]any); ok {
		if len(objects) == 0 {
			panic("No faces detected")
		}
	} else {
		panic("No faces detected")
	}
}

func getImageWidthAndHeight(data map[string]any) (float64, float64) {
	if image, ok := data["image"].(map[string]any); ok {
		shape := image["shape"].([]any)
		width := shape[1].(float64)
		height := shape[0].(float64)

		return width, height
	}

	panic("Can't find image")
}

func createProcessingBlock(service facesdk.FacerecService, unitType *string, modification *string, version *int) facesdk.ProcessingBlock {
	config, err := facesdk.CreateContext()
	checkError(err)
	defer config.Close()

	unitTypeContext, err := config.GetOrInsertByKey("unit_type")
	checkError(err)

	err = unitTypeContext.SetString(*unitType)
	checkError(err)

	if *unitType == "QUALITY_ASSESSMENT_ESTIMATOR" && *modification == "" {
		*modification = "assessment"

		configName, err := config.GetOrInsertByKey("config_name")
		checkError(err)

		err = configName.SetString("quality_assessment.xml")
		checkError(err)
	}

	if *modification != "" {
		modificationContext, err := config.GetOrInsertByKey("modification")
		checkError(err)

		err = modificationContext.SetString(*modification)
		checkError(err)
	}

	if *version != -1 {
		versionContext, err := config.GetOrInsertByKey("version")
		checkError(err)

		err = versionContext.SetInt((int64)(*version))
		checkError(err)
	}

	block, err := service.CreateProcessingBlock(config)
	checkError(err)

	return block
}

func printFaceDetector(data map[string]any) {
	width, height := getImageWidthAndHeight(data)
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		bbox, _ := object.(map[string]any)["bbox"].([]any)

		x1, _ := bbox[0].(float64)
		y1, _ := bbox[1].(float64)
		x2, _ := bbox[2].(float64)
		y2, _ := bbox[3].(float64)

		x1 = x1 * width
		y1 = y1 * height

		x2 = x2 * width
		y2 = y2 * height

		fmt.Printf("Bounding box coordinates: (%d, %d) (%d, %d)\n", (int)(x1), (int)(y1), (int)(x2), (int)(y2))
	}
}

func printFaceFitter(data map[string]any) {
	width, height := getImageWidthAndHeight(data)
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		keypoints, _ := object.(map[string]any)["keypoints"].(map[string]any)
		points, _ := keypoints["points"].([]any)

		for _, value := range points {
			proj, _ := value.(map[string]any)["proj"].([]any)

			x := (int)(proj[0].(float64) * width)
			y := (int)(proj[1].(float64) * height)

			fmt.Printf("Point: (%d, %d)\n", x, y)
		}

		fmt.Println()
	}
}

func printEmotionEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		emotions, _ := object.(map[string]any)["emotions"].([]any)

		for _, emotion := range emotions {
			emotionObject, _ := emotion.(map[string]any)

			fmt.Println(emotionObject["emotion"].(string), emotionObject["confidence"].(float64))
		}

		fmt.Println()
	}
}

func printAgeEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		age, _ := object.(map[string]any)["age"].(float64)

		fmt.Printf("Age: %0.f\n", age)
	}
}

func printGenderEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		gender, _ := object.(map[string]any)["gender"].(string)

		fmt.Printf("Gender: %s\n", gender)
	}
}

func printMaskEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		mask, _ := object.(map[string]any)["has_medical_mask"].(map[string]any)

		fmt.Printf("Mask: %t, confidence: %0.3f\n", mask["value"].(bool), mask["confidence"].(float64))
	}
}

func printGlassesEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		glasses, _ := object.(map[string]any)["glasses"].(map[string]any)

		fmt.Printf("Glasses: %t, confidence: %0.3f\n", glasses["value"].(bool), glasses["confidence"].(float64))
	}
}

func printDeepFakeEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		deepfake, _ := object.(map[string]any)["deepfake"].(map[string]any)

		fmt.Printf("Deepfake: %t, confidence: %0.3f\n", deepfake["value"].(bool), deepfake["confidence"].(float64))
	}
}

func printEyeOpennessEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		leftEyeOpen, _ := object.(map[string]any)["is_left_eye_open"].(map[string]any)
		rightEyeOpen, _ := object.(map[string]any)["is_right_eye_open"].(map[string]any)

		fmt.Printf("Left eye: %t, confidence: %0.3f\n", leftEyeOpen["value"].(bool), leftEyeOpen["confidence"].(float64))
		fmt.Printf("Right eye: %t, confidence: %0.3f\n", rightEyeOpen["value"].(bool), rightEyeOpen["confidence"].(float64))
		fmt.Println()
	}
}

func printLivenessEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		liveness, _ := object.(map[string]any)["liveness"].(map[string]any)

		fmt.Printf("Liveness: %s, confidence: %0.3f\n", liveness["value"].(string), liveness["confidence"].(float64))
	}
}

func printQualityAssessmentEstimator(data map[string]any) {
	objects, _ := data["objects"].([]any)

	for _, object := range objects {
		quality, _ := object.(map[string]any)["quality"].(map[string]any)

		fmt.Printf("Quality total score: %f\n", quality["total_score"].(float64))
	}
}
