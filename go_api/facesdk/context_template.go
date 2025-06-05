package facesdk

/*
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define SAVE_TEMPLATE_DEFAULT_SIZE 300
#define MAX_METHOD_NAME_SIZE 16

typedef void (*binary_stream_write_func_type)(
		void* stream,
		const void* data,
		uint64_t bytes_count);

typedef void (*binary_stream_read_func_type)(
		void* stream,
		void* data,
		uint64_t bytes_count);

typedef struct {
	char* data;
	size_t size;
	size_t offset;
} WriteData;

typedef struct {
	const char* data;
	size_t offset;
} ReadData;

void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_loadTemplate(void* binaryStream, binary_stream_read_func_type readFunction, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_getMethodName(void* templ, void* binaryStream, binary_stream_write_func_type writeFunction, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_save(void* templ, void* binaryStream, binary_stream_write_func_type writeFunction, void** exception);
int32_t __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_size(void* templ, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_convert(void* context, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_destructor(void* implementation);

extern void readFunction(void* data, void* buffer, uint64_t bytesCount);
extern void writeFunction(void* data, const void* buffer, uint64_t bytesCount);

void* loadTemplate(void* data, void** exception)
{
	ReadData readData = { (const char*)data, 0 };

	return __4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_loadTemplate(&readData, readFunction, exception);
}

const uint8_t* saveTemplate(void* contextTemplate, size_t* size, void** exception)
{
	WriteData writeData = { (char*)malloc(SAVE_TEMPLATE_DEFAULT_SIZE), SAVE_TEMPLATE_DEFAULT_SIZE, 0 };

	__4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_save(contextTemplate, &writeData, writeFunction, exception);

	*size = writeData.offset;

	return (const uint8_t*)writeData.data;
}

const char* getContextTemplateMethodName(void* contextTemplate, size_t* size, void** exception)
{
	WriteData writeData = { (char*)malloc(MAX_METHOD_NAME_SIZE), MAX_METHOD_NAME_SIZE, 0 };

	__4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_getMethodName(contextTemplate, &writeData, writeFunction, exception);

	*size = writeData.offset;

	return writeData.data;
}
*/
import "C"
import (
	"errors"
	"unsafe"
)

type ContextTemplate struct {
	implementation unsafe.Pointer
}

// Create ContextTemplate from []bytes
func LoadContextTemplate(data []byte) (ContextTemplate, error) {
	exception := createException()

	result := C.loadTemplate(unsafe.Pointer(&data[0]), &exception)

	return ContextTemplate{implementation: result}, checkApiException(exception)
}

// Convert blob from Context to ContextTemplate
func ConvertToContextTemplate(context Context) (ContextTemplate, error) {
	exception := createException()

	result := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_convert(context.implementation, &exception)

	return ContextTemplate{implementation: result}, checkApiException(exception)
}

// Serialize ContextTemplate to []byte
func (template ContextTemplate) Save() ([]byte, error) {
	exception := createException()
	var size = C.size_t(0)

	temp := unsafe.Pointer(C.saveTemplate(template.implementation, &size, &exception))
	result := C.GoBytes(temp, C.int(size))

	C.free(temp)

	return result, checkApiException(exception)
}

// Get size of ContextTemplate in bytes
func (template ContextTemplate) Size() (int32, error) {
	exception := createException()

	result := int32(C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_size(template.implementation, &exception))

	return result, checkApiException(exception)
}

// Get method name of ContextTemplate
func (template ContextTemplate) GetMethodName() (string, error) {
	exception := createException()
	var size = C.size_t(0)

	temp := C.getContextTemplateMethodName(template.implementation, &size, &exception)
	result := C.GoStringN(temp, C.int(size))

	C.free(unsafe.Pointer(temp))

	return result, checkApiException(exception)
}

// Destroy ContextTemplate
func (template *ContextTemplate) Close() error {
	if template.implementation == nil {
		return errors.New("ContextTemplate already destroyed")
	}

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_ContextTemplate_destructor(template.implementation)

	template.implementation = nil

	return nil
}
