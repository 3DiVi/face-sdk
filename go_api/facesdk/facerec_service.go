package facesdk

/*
#include <stdlib.h>
#include <stdint.h>

#define MAX_VERSION_SIZE 32

typedef void (*binary_stream_write_func_type)(
		void* stream,
		const void* data,
		uint64_t bytes_count);

typedef struct {
	char* data;
	size_t size;
	size_t offset;
} WriteData;

void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_constructor3(void* ae_ptr, char const* conf_dir, char const* license_dir, char const* dll_path, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_ProcessingBlock_createProcessingBlock(void* service, void* context, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_1(void* service, const void** contextTemplates, const char** uuids, uint64_t size, const void* config, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_2(void* service, const void* config, void** exception);

void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_get_version(void* version_stream, binary_stream_write_func_type writeFunction, void** exception);

extern void writeFunction(void* data, const void* buffer, uint64_t bytesCount);

const char* getVersion(size_t* size, void** exception)
{
	WriteData writeData = { (char*)malloc(MAX_VERSION_SIZE), MAX_VERSION_SIZE, 0 };

	__4848a76477c449608aa5deb15c5495e4_facerec_v3_get_version(&writeData, writeFunction, exception);

	*size = writeData.offset;

	return writeData.data;
}
*/
import "C"
import (
	"fmt"
	"runtime"
	"unsafe"
)

type FacerecService struct {
	implementation unsafe.Pointer
}

// Create FacerecService
func CreateFacerecService(confPath string, licensePath string) (FacerecService, error) {
	exception := createException()
	dynamicLibraryName := ""
	apiVersion := "3.28.00"

	if runtime.GOOS == "windows" {
		dynamicLibraryName = "facerec.dll"
	} else {
		dynamicLibraryName = "libfacerec.so"
	}

	dllPathPtr := C.CString(dynamicLibraryName)
	confPathPtr := C.CString(confPath)
	licensePathPtr := C.CString(licensePath)

	service := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_constructor3(nil, confPathPtr, licensePathPtr, dllPathPtr, &exception)

	C.free(unsafe.Pointer(dllPathPtr))
	C.free(unsafe.Pointer(confPathPtr))
	C.free(unsafe.Pointer(licensePathPtr))

	result, err := FacerecService{implementation: service}, checkApiException(exception)

	if err != nil {
		return result, err
	}

	version, err := result.GetVersion()

	if version != apiVersion {
		fmt.Printf("WARNING: The version in the facerec_service.go does not match the version in the library. facerec_service.go version: %s, library version: %s\n", apiVersion, version)
	}

	return result, err
}

// Create ProcessingBlock
func (service FacerecService) CreateProcessingBlock(context Context) (ProcessingBlock, error) {
	exception := createException()

	block := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_ProcessingBlock_createProcessingBlock(service.implementation, context.implementation, &exception)

	return ProcessingBlock{implementation: block}, checkApiException(exception)
}

// Create DynamicTemplateIndex
func (service FacerecService) CreateDynamicTemplateIndex(config Context) (DynamicTemplateIndex, error) {
	exception := createException()

	templateIndex := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_2(
		service.implementation,
		config.implementation,
		&exception)

	return DynamicTemplateIndex{implementation: templateIndex}, checkApiException(exception)
}

// Create DynamicTemplateIndex with []ContextTemplate
func (service FacerecService) CreateDynamicTemplateIndexWithTemplates(templates []ContextTemplate, uuids []string, config Context) (DynamicTemplateIndex, error) {
	exception := createException()
	cArray := C.malloc(C.size_t(len(uuids)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	cStrings := make([]*C.char, len(uuids))
	tempTemplates := make([]unsafe.Pointer, len(templates))
	tempUuids := (**C.char)(cArray)

	for i, template := range templates {
		tempTemplates[i] = template.implementation
	}

	for i, s := range uuids {
		cStr := C.CString(s)
		cStrings[i] = cStr
		ptr := (**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(tempUuids)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		*ptr = cStr
	}

	templateIndex := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_1(
		service.implementation,
		&tempTemplates[0],
		tempUuids,
		C.uint64_t(len(templates)),
		config.implementation,
		&exception)

	for _, cStr := range cStrings {
		C.free(unsafe.Pointer(cStr))
	}

	C.free(unsafe.Pointer(cArray))

	return DynamicTemplateIndex{implementation: templateIndex}, checkApiException(exception)
}

// Get libfacerec.so version
func (service FacerecService) GetVersion() (string, error) {
	exception := createException()
	var size = C.size_t(0)

	temp := C.getVersion(&size, &exception)
	result := C.GoStringN(temp, C.int(size))

	C.free(unsafe.Pointer(temp))

	return result, checkApiException(exception)
}
