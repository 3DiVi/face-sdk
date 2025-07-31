package main

import (
	"encoding/base64"
	"encoding/json"
	"errors"
	"facesdk"
	"flag"
	"fmt"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"sync"

	"github.com/schollz/progressbar/v3"
)

type process func(data facesdk.Context) error

func main() {
	sdkPath := flag.String("sdk_path", "", "Path to Face SDK(required)")
	imagesPath := flag.String("images_path", "", "Path to directory with images(required)")
	detectorConfig := flag.String("detector_config", "./detector.json", "Config file with face detector configuration")
	fitterConfig := flag.String("fitter_config", "./fitter.json", "Config file with face fitter configuration")
	faceTemplateExtractorConfig := flag.String("face_template_extractor_config", "./face_template_extractor.json", "Config file with face template extractor configuration")
	matcherConfig := flag.String("matcher_config", "./matcher.json", "Config file with matcher configuration")
	verificationConfig := flag.String("verification_config", "./verification.json", "Config file with verification configuration")
	maxLicenseCount := flag.Int("max_license_count", 1000, "License usage for DynamicTemplateIndex")

	flag.Parse()

	fmt.Println("All flags:")
	flag.VisitAll(func(f *flag.Flag) {
		fmt.Printf("  -%s: value=%q (default=%q) - %s\n", f.Name, f.Value, f.DefValue, f.Usage)
	})

	service, err := facesdk.CreateFacerecService(fmt.Sprintf("%s/conf/facerec", *sdkPath), fmt.Sprintf("%s/license", *sdkPath))
	checkError(err)

	pipeline := make([]facesdk.ProcessingBlock, 3)
	matcher := createProcessingBlock(service, matcherConfig)
	verification := createProcessingBlock(service, verificationConfig)
	modification, version := getModificationAndVersion(*verificationConfig)
	var mutex sync.Mutex

	pipeline[0] = createProcessingBlock(service, detectorConfig)
	pipeline[1] = createProcessingBlock(service, fitterConfig)
	pipeline[2] = createProcessingBlock(service, faceTemplateExtractorConfig)

	runPipeline := func(data facesdk.Context) error {
		mutex.Lock()
		defer mutex.Unlock()

		for _, block := range pipeline {
			err = block.Process(data)
			if err != nil {
				return err
			}
		}

		return nil
	}

	templateIndex := initializeDatabase(imagesPath, runPipeline, service, modification, version, *maxLicenseCount)

	http.HandleFunc("/", func(writer http.ResponseWriter, request *http.Request) {
		switch request.Method {
		case http.MethodGet:
			size, _ := templateIndex.Size()
			capacity, _ := templateIndex.Capacity()

			json.NewEncoder(writer).Encode(map[string]any{"templateIndexSize": size, "templateIndexCapacity": capacity})

		default:
			http.Error(writer, "Method not allowed", http.StatusMethodNotAllowed)
		}
	})
	http.HandleFunc("/matching", func(writer http.ResponseWriter, request *http.Request) {
		switch request.Method {
		case http.MethodPost:
			writer.Header().Set("Content-Type", "application/json")

			search(writer, request, runPipeline, templateIndex, matcher)

		default:
			http.Error(writer, "Method not allowed", http.StatusMethodNotAllowed)
		}
	})
	http.HandleFunc("/verification", func(writer http.ResponseWriter, request *http.Request) {
		switch request.Method {
		case http.MethodPost:
			writer.Header().Set("Content-Type", "application/json")

			verify(writer, request, runPipeline, verification)

		default:
			http.Error(writer, "Method not allowed", http.StatusMethodNotAllowed)
		}
	})

	fmt.Println("Starting server on http://127.0.0:8080...")
	log.Fatal(http.ListenAndServe("127.0.0.1:8080", nil))
}

func checkError(err error) {
	if err != nil {
		panic(err)
	}
}

func handleError(err error, writer http.ResponseWriter) bool {
	if err == nil {
		return false
	}

	jsonData := map[string]any{"Error": err.Error()}

	json.NewEncoder(writer).Encode(jsonData)

	return true
}

func checkObjects(data facesdk.Context) bool {
	objects, err := data.GetOrInsertByKey("objects")

	if err != nil {
		return false
	}

	length, err := objects.GetLength()

	if err != nil {
		return false
	}

	return length == 1
}

func createProcessingBlock(service facesdk.FacerecService, configPath *string) facesdk.ProcessingBlock {
	config, err := facesdk.CreateContextFromJsonFile(*configPath)
	checkError(err)
	defer config.Close()

	block, err := service.CreateProcessingBlock(config)
	checkError(err)

	return block
}

func supportedImageFormat(extension string) bool {
	extension = strings.ToLower(extension)

	return extension == ".jpg" || extension == ".png" || extension == ".jpeg" || extension == ".webp" || extension == ".bmp"
}

func initializeDatabase(imagesPath *string, runPipeline process, service facesdk.FacerecService, modification string, version int, maxLicenseCount int) facesdk.DynamicTemplateIndex {
	fmt.Println("Creating database...")

	config, err := facesdk.CreateContext()
	checkError(err)
	defer config.Close()

	context, err := config.GetOrInsertByKey("max_license_count")
	checkError(err)

	err = context.SetInt(int64(maxLicenseCount))
	checkError(err)

	context, err = config.GetOrInsertByKey("capacity")
	checkError(err)

	err = context.SetInt(int64(1000))
	checkError(err)

	context, err = config.GetOrInsertByKey("modification")
	checkError(err)

	err = context.SetString(modification)
	checkError(err)

	context, err = config.GetOrInsertByKey("version")
	checkError(err)

	err = context.SetInt(int64(version))
	checkError(err)

	templateIndex, err := service.CreateDynamicTemplateIndex(config)
	checkError(err)

	images := make([]string, 0, 1000)

	err = filepath.WalkDir(*imagesPath, func(path string, entry os.DirEntry, err error) error {
		if err != nil {
			return err
		}

		if entry.IsDir() || !supportedImageFormat(filepath.Ext(path)) {
			return nil
		}

		images = append(images, path)

		return nil
	})
	checkError(err)

	limit := min(maxLicenseCount, len(images))
	bar := progressbar.NewOptions(limit,
		progressbar.OptionSetDescription("Processing"),
		progressbar.OptionSetWidth(100),
		progressbar.OptionShowCount(),
		progressbar.OptionShowBytes(false),
		progressbar.OptionSetTheme(progressbar.Theme{
			Saucer:        "=",
			SaucerPadding: " ",
			BarStart:      "[",
			BarEnd:        "]",
		}),
	)

	for _, path := range images {
		size, err := templateIndex.Size()
		checkError(err)

		if size == uint64(limit) {
			break
		}

		imageBytes, err := os.ReadFile(path)
		checkError(err)

		data, err := facesdk.CreateContextFromEncodedImage(imageBytes)
		checkError(err)
		defer data.Close()

		err = runPipeline(data)
		checkError(err)

		if !checkObjects(data) {
			continue
		}

		objects, _ := data.GetOrInsertByKey("objects")
		temp, _ := objects.GetByIndex(0)
		temp, _ = temp.GetOrInsertByKey("face_template")
		temp, _ = temp.GetOrInsertByKey("template")
		contextTemplate, _ := temp.GetContextTemplate()
		defer contextTemplate.Close()

		templateIndex.AddSingle(contextTemplate, filepath.Base(path))

		bar.Add(1)
	}

	fmt.Println()
	fmt.Println("Database created")

	return templateIndex
}

func getImageFromBase64(base64Data *string) ([]byte, error) {
	image, err := base64.StdEncoding.DecodeString(*base64Data)

	return []byte(image), err
}

func search(writer http.ResponseWriter, request *http.Request, runPipeline process, templateIndex facesdk.DynamicTemplateIndex, matcher facesdk.ProcessingBlock) {
	var jsonData map[string]any
	json.NewDecoder(request.Body).Decode(&jsonData)
	base64Data, ok := jsonData["image"].(string)
	var err error

	if !ok {
		err = errors.New("can't parse image")
	}

	if handleError(err, writer) {
		return
	}

	var data facesdk.Context

	bytes, err := getImageFromBase64(&base64Data)
	if handleError(err, writer) {
		return
	}

	data, err = facesdk.CreateContextFromEncodedImage(bytes)
	if handleError(err, writer) {
		return
	}
	defer data.Close()

	err = runPipeline(data)
	if handleError(err, writer) {
		return
	}

	defer data.Close()

	if !checkObjects(data) {
		err = errors.New("image must contain 1 face")

		if handleError(err, writer) {
			return
		}
	}

	objects, _ := data.GetOrInsertByKey("objects")
	temp, _ := objects.GetByIndex(0)
	temp, _ = temp.GetOrInsertByKey("face_template")
	temp, _ = temp.GetOrInsertByKey("template")
	contextTemplate, _ := temp.GetContextTemplate()
	defer contextTemplate.Close()

	matcherData, err := facesdk.CreateContext()
	if handleError(err, writer) {
		return
	}
	defer matcherData.Close()

	temp, err = matcherData.GetOrInsertByKey("queries")
	if handleError(err, writer) {
		return
	}

	err = temp.SetContextTemplate(contextTemplate)
	if handleError(err, writer) {
		return
	}

	temp, err = matcherData.GetOrInsertByKey("template_index")
	if handleError(err, writer) {
		return
	}

	err = temp.SetDynamicTemplateIndex(templateIndex)
	if handleError(err, writer) {
		return
	}

	err = matcher.Process(matcherData)
	if handleError(err, writer) {
		return
	}

	result, err := matcherData.ToMap()
	if handleError(err, writer) {
		return
	}

	json.NewEncoder(writer).Encode(result)
}

func verify(writer http.ResponseWriter, request *http.Request, runPipeline process, verification facesdk.ProcessingBlock) {
	var jsonData map[string]any
	json.NewDecoder(request.Body).Decode(&jsonData)
	base64Data1, ok1 := jsonData["image1"].(string)
	base64Data2, ok2 := jsonData["image2"].(string)
	var err error

	if !ok1 {
		err = errors.New("can't parse image1")
	}

	if handleError(err, writer) {
		return
	}

	if !ok2 {
		err = errors.New("can't parse image2")
	}

	if handleError(err, writer) {
		return
	}

	template1, err := processImage(&base64Data1, runPipeline)
	if handleError(err, writer) {
		return
	}
	defer template1.Close()

	template2, err := processImage(&base64Data2, runPipeline)
	if handleError(err, writer) {
		return
	}
	defer template2.Close()

	verificationContext, err := facesdk.CreateContext()
	if handleError(err, writer) {
		return
	}
	defer verificationContext.Close()

	context, err := verificationContext.GetOrInsertByKey("template1")
	if handleError(err, writer) {
		return
	}

	err = context.Copy(template1)
	if handleError(err, writer) {
		return
	}

	context, err = verificationContext.GetOrInsertByKey("template2")
	if handleError(err, writer) {
		return
	}

	err = context.Copy(template2)
	if handleError(err, writer) {
		return
	}

	err = verification.Process(verificationContext)
	if handleError(err, writer) {
		return
	}

	resultContext, err := verificationContext.GetOrInsertByKey("result")
	if handleError(err, writer) {
		return
	}

	result, err := resultContext.ToMap()
	if handleError(err, writer) {
		return
	}

	json.NewEncoder(writer).Encode(result)
}

func processImage(base64Data *string, runPipeline process) (facesdk.Context, error) {
	image, err := getImageFromBase64(base64Data)

	if err != nil {
		return facesdk.Context{}, err
	}

	context, err := facesdk.CreateContextFromEncodedImage(image)

	if err != nil {
		return facesdk.Context{}, err
	}
	defer context.Close()

	err = runPipeline(context)

	if err != nil {
		return facesdk.Context{}, err
	}

	if checkObjects(context) {
		objects, err := context.GetOrInsertByKey("objects")

		if err != nil {
			return context, err
		}

		context, err := objects.GetByIndex(0)

		if err != nil {
			return context, err
		}

		result, err := context.GetOrInsertByKey("face_template")

		if err != nil {
			return context, err
		}

		return result.Clone()
	} else {
		return context, errors.New("image must contain 1 face")
	}
}

func getModificationAndVersion(path string) (string, int) {
	file, err := os.Open(path)
	checkError(err)

	var data map[string]any

	json.NewDecoder(file).Decode(&data)

	modificationVal, ok := data["modification"].(string)
	if !ok {
		modificationVal = "1000"
	}

	versionVal, ok := data["version"].(int)
	if !ok {
		versionVal = 1
	}

	return modificationVal, versionVal
}
