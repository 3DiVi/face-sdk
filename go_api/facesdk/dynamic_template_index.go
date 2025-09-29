package facesdk

/*
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define UUID_DEFAULT_SIZE 36
#define MAX_METHOD_NAME_SIZE 16

typedef void (*binary_stream_write_func_type)(
		void* stream,
		const void* data,
		uint64_t bytes_count);

typedef struct {
	char* data;
	size_t size;
	size_t offset;
} WriteData;

uint64_t __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_size(void* implementation, void** exception);
uint64_t __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_capacity(void* implementation, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_add_3(void* templateIndex, const void* contextTemplate, const char* uuid, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_add_4(void* templateIndex, const void** contextTemplates, const char** uuids, uint64_t size, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_remove_1(void* templateIndex, const char* uuid, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_remove_2(void* templateIndex, const char** uuids, uint64_t size, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_concatenate(void* templateIndex, void* otherIndex, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_at_by_uuid(void* templateIndex, const char* uuid, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_at_by_index(void* templateIndex, int64_t index, void* stream, binary_stream_write_func_type binary_stream_write_func, void** exception);
void* __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_get(void* templateIndex, int64_t index, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_getMethodName(void* templateIndex, void* stream, binary_stream_write_func_type binary_stream_write_func, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_clear(void* templateIndex, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_save(void* templateIndex, const char* filePath, bool allowOverwrite, void** exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_destructor(void* templateIndex);

extern void writeFunction(void* data, const void* buffer, uint64_t bytesCount);

const char* atByIndex(void* templateIndex, int64_t index, size_t* size, void** exception)
{
	WriteData writeData = { (char*)malloc(UUID_DEFAULT_SIZE), UUID_DEFAULT_SIZE, 0 };

	__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_at_by_index(templateIndex, index, &writeData, writeFunction, exception);

	*size = writeData.offset;

	return writeData.data;
}

const char* getDynamicTemplateIndexMethodName(void* templatIndex, size_t* size, void** exception)
{
	WriteData writeData = { (char*)malloc(MAX_METHOD_NAME_SIZE), MAX_METHOD_NAME_SIZE, 0 };

	__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_getMethodName(templatIndex, &writeData, writeFunction, exception);

	*size = writeData.offset;

	return writeData.data;
}
*/
import "C"
import (
	"errors"
	"unsafe"
)

type DynamicTemplateIndex struct {
	implementation unsafe.Pointer
}

// Get size of DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) Size() (uint64, error) {
	exception := createException()

	result := uint64(C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_size(templateIndex.implementation, &exception))

	return result, checkApiException(exception)
}

// Get capacity of DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) Capacity() (uint64, error) {
	exception := createException()

	result := uint64(C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_capacity(templateIndex.implementation, &exception))

	return result, checkApiException(exception)
}

// Add ContextTemplate to DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) AddSingle(contextTemplate ContextTemplate, uuid string) error {
	exception := createException()
	uuidPtr := C.CString(uuid)

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_add_3(
		templateIndex.implementation,
		contextTemplate.implementation,
		uuidPtr,
		&exception)

	C.free(unsafe.Pointer(uuidPtr))

	return checkApiException(exception)
}

// Add []ContextTemplate to DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) AddMultiple(contextTemplates []ContextTemplate, uuids []string) error {
	exception := createException()
	tempTemplates := make([]unsafe.Pointer, len(contextTemplates))
	tempUuids, cleanup := createCStringArray(uuids)

	for i, template := range contextTemplates {
		tempTemplates[i] = template.implementation
	}

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_add_4(
		templateIndex.implementation,
		&tempTemplates[0],
		tempUuids,
		C.uint64_t(len(contextTemplates)),
		&exception)

	cleanup()

	return checkApiException(exception)
}

// Remove ContextTemplate by UUID from DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) RemoveSingle(uuid string) error {
	exception := createException()
	uuidPtr := C.CString(uuid)

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_remove_1(templateIndex.implementation, uuidPtr, &exception)

	C.free(unsafe.Pointer(uuidPtr))

	return checkApiException(exception)
}

// Remove []ContextTemplate by UUIDs from DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) RemoveMultiple(uuids []string) error {
	exception := createException()
	tempUuids, cleanup := createCStringArray(uuids)

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_remove_2(
		templateIndex.implementation,
		tempUuids, C.uint64_t(len(uuids)),
		&exception)

	cleanup()

	return checkApiException(exception)
}

// Append DynamicTemplateIndex to another DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) Concatenate(otherIndex DynamicTemplateIndex) error {
	exception := createException()

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_concatenate(
		templateIndex.implementation,
		otherIndex.implementation,
		&exception)

	return checkApiException(exception)
}

// Get ContextTemplate by UUID from DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) GetContextTemplateByUuid(uuid string) (ContextTemplate, error) {
	exception := createException()
	uuidPtr := C.CString(uuid)

	result := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_at_by_uuid(
		templateIndex.implementation,
		uuidPtr,
		&exception)

	C.free(unsafe.Pointer(uuidPtr))

	return ContextTemplate{implementation: result}, checkApiException(exception)
}

// Get UUID by index from DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) GetUuidByIndex(index int64) (string, error) {
	exception := createException()
	size := C.size_t(0)

	temp := C.atByIndex(templateIndex.implementation, C.int64_t(index), &size, &exception)
	result := C.GoStringN(temp, C.int(size))

	C.free(unsafe.Pointer(temp))

	return result, checkApiException(exception)
}

// Get ContextTemplate by index from DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) GetContextTemplateByIndex(index int64) (ContextTemplate, error) {
	exception := createException()

	result := C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_get(
		templateIndex.implementation,
		C.int64_t(index),
		&exception)

	return ContextTemplate{implementation: result}, checkApiException(exception)
}

// Get method name of DynamicTemplateIndex
func (templateIndex DynamicTemplateIndex) GetMethodName() (string, error) {
	exception := createException()
	var size = C.size_t(0)

	temp := C.getDynamicTemplateIndexMethodName(templateIndex.implementation, &size, &exception)
	result := C.GoStringN(temp, C.int(size))

	C.free(unsafe.Pointer(temp))

	return result, checkApiException(exception)
}

// Remove all ContextTemplates from DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) Clear() error {
	exception := createException()

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_clear(templateIndex.implementation, &exception)

	return checkApiException(exception)
}

// Save DynamicTemplateIndex to file
func (templateIndex *DynamicTemplateIndex) Save(filePath string, allowOverwrite bool) error {
	exception := createException()
	filePathPtr := C.CString(filePath)

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_save(templateIndex.implementation, filePathPtr, C.bool(allowOverwrite), &exception)

	C.free(unsafe.Pointer(filePathPtr))

	return checkApiException(exception)
}

// Destroy DynamicTemplateIndex
func (templateIndex *DynamicTemplateIndex) Close() error {
	if templateIndex.implementation == nil {
		return errors.New("DynamicTemplateIndex already destroyed")
	}

	C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_DynamicTemplateIndex_destructor(templateIndex.implementation)

	templateIndex.implementation = nil

	return nil
}
