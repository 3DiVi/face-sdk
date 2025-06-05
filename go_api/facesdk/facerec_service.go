package facesdk

/*
#include <stdlib.h>
#include <stdint.h>

void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_constructor3(void* ae_ptr, char const* conf_dir, char const* license_dir, char const* dll_path, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_ProcessingBlock_createProcessingBlock(void* service, void* context, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_1(void* service, const void** contextTemplates, const char** uuids, uint64_t size, const void* config, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_FacerecService_createDynamicTemplateIndex_2(void* service, const void* config, void** exception);
*/
import "C"
import (
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

	return FacerecService{implementation: service}, checkApiException(exception)
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
