package facesdk

/*
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void* TDVContext_create(void** eh);
void TDVContext_destroy(void* context, void** eh);

void* TDVContext_createFromEncodedImage(const uint8_t* data, uint64_t dataSize, void** eh);
void* TDVContext_createFromFrame(uint8_t* data, int32_t width, int32_t height, int32_t format, int32_t baseAngle, void** eh);
void* TDVContext_createFromJsonFile(const char* path, void** eh);

void* TDVContext_getByIndex(void* ctx, int key, void** eh);
void* TDVContext_getByKey(void* ctx, const char* key, void** eh);
void* TDVContext_getOrInsertByKey(void* ctx, const char* key, void** eh);

void TDVContext_putStr(void* ctx, const char* str, void** eh);
void TDVContext_putLong(void* ctx, int64_t val, void** eh);
void TDVContext_putDouble(void* ctx, double val, void** eh);
void TDVContext_putBool(void* ctx, bool val, void** eh);
unsigned char* TDVContext_putDataPtr(void* ctx, unsigned char* val, uint64_t copy_sz, void** eh);
void TDVContext_putContextTemplate(void* context, void* value, void** errorHandler);
void TDVContext_putDynamicTemplateIndex(void* context, void* value, void** errorHandler);

const char* TDVContext_getStr(void* ctx, void** eh);
uint64_t TDVContext_getStrSize(void* ctx, void** eh);
int64_t TDVContext_getLong(void* ctx, void** eh);
uint64_t TDVContext_getUnsignedLong(void* ctx, void** eh);
double TDVContext_getDouble(void* ctx, void** eh);
bool TDVContext_getBool(void* ctx, void** eh);
unsigned char* TDVContext_getDataPtr(void* ctx, void** eh);
void* TDVContext_getContextTemplate(void* ctx, void** errorHandler);
void* TDVContext_getDynamicTemplateIndex(void* ctx, void** eh);

bool TDVContext_isNone(void* ctx, void** eh);
bool TDVContext_isArray(void* ctx, void** eh);
bool TDVContext_isObject(void* ctx, void** eh);

bool TDVContext_isBool(void* ctx, void** eh);
bool TDVContext_isLong(void* ctx, void** eh);
bool TDVContext_isUnsignedLong(void* ctx, void** eh);
bool TDVContext_isDouble(void* ctx, void** eh);
bool TDVContext_isString(void* ctx, void** eh);
bool TDVContext_isDataPtr(void* ctx, void** eh);
bool TDVContext_isDynamicTemplateIndex(void* ctx, void** eh);
bool TDVContext_isContextTemplate(void* ctx, void** eh);

void TDVContext_copy(void* src, void* dst, void** eh);
void* TDVContext_clone(void* ctx, void** eh);
void TDVContext_clear(void* ctx, void** eh);
void TDVContext_erase(void* ctx, const char* key, void** eh);
void TDVContext_reserve(void* ctx, const uint64_t size, void** eh);
bool TDVContext_contains(void* ctx, const char* key, void** eh);
bool TDVContext_empty(void* ctx, void** eh);
bool TDVContext_compare(void* ctx, void* ctx2, void** eh);

uint64_t TDVContext_getLength(void* ctx, void** eh);
char** TDVContext_getKeys(void* ctx, uint64_t length, void** eh);
void TDVContext_pushBack(void* handle_, void* data, bool copy, void** eh);

const char* TDVContext_serializeToJson(void* ctx, void** eh);
void TDVContext_saveToJsonFile(void* ctx, const char* path, void** eh);
void TDVContext_deleteString(const char* str, void** eh);
*/
import "C"
import (
	"encoding/json"
	"errors"
	"unsafe"
)

type Format int

const (
	FORMAT_BGR Format = iota
	FORMAT_RGB
	FORMAT_BGRA8888
	FORMAT_YUV420
	FORMAT_YUV_NV12
	FORMAT_NV21
)

type Context struct {
	implementation unsafe.Pointer
}

// Create empty Context
func CreateContext() (Context, error) {
	exception := createException()
	context := C.TDVContext_create(&exception)

	return Context{implementation: context}, checkProcessingBlockException(exception)
}

// Create Context with prepared fields for using with ProcessingBlock from images like .jpeg, .png, ...
func CreateContextFromEncodedImage(data []byte) (Context, error) {
	exception := createException()
	dataPtr := C.CBytes(data)
	context := C.TDVContext_createFromEncodedImage((*C.uint8_t)(dataPtr), C.uint64_t(len(data)), &exception)

	C.free(dataPtr)

	return Context{implementation: context}, checkProcessingBlockException(exception)
}

// Create Context with prepared fields for using with ProcessingBlock from raw bytes
func CreateContextFromFrame(data []byte, width int, height int, format Format, baseAngle int) (Context, error) {
	exception := createException()
	dataPtr := C.CBytes(data)

	result := C.TDVContext_createFromFrame((*C.uint8_t)(dataPtr), C.int32_t(width), C.int32_t(height), C.int32_t(format), C.int32_t(baseAngle), &exception)

	C.free(dataPtr)

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Create Context from .json file
func CreateContextFromJsonFile(path string) (Context, error) {
	exception := createException()
	pathPtr := C.CString(path)

	result := C.TDVContext_createFromJsonFile(pathPtr, &exception)

	C.free(unsafe.Pointer(pathPtr))

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Get Context field by index(Context must be array)
func (context Context) GetByIndex(index int) (Context, error) {
	exception := createException()

	result := C.TDVContext_getByIndex(context.implementation, C.int32_t(index), &exception)

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Get Context field by key(Context must be object)
func (context Context) GetByKey(key string) (Context, error) {
	exception := createException()
	keyPtr := C.CString(key)

	result := C.TDVContext_getByKey(context.implementation, keyPtr, &exception)

	C.free(unsafe.Pointer(keyPtr))

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Get or if key doesn't exist create Context field(Context must be object)
func (context *Context) GetOrInsertByKey(key string) (Context, error) {
	exception := createException()
	keyPtr := C.CString(key)

	result := C.TDVContext_getOrInsertByKey(context.implementation, keyPtr, &exception)

	C.free(unsafe.Pointer(keyPtr))

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Set string value
func (context *Context) SetString(value string) error {
	exception := createException()
	valuePtr := C.CString(value)

	C.TDVContext_putStr(context.implementation, valuePtr, &exception)

	C.free(unsafe.Pointer(valuePtr))

	return checkProcessingBlockException(exception)
}

// Set int64 value
func (context *Context) SetInt(value int64) error {
	exception := createException()

	C.TDVContext_putLong(context.implementation, C.int64_t(value), &exception)

	return checkProcessingBlockException(exception)
}

// Set float64 value
func (context *Context) SetFloat(value float64) error {
	exception := createException()

	C.TDVContext_putDouble(context.implementation, C.double(value), &exception)

	return checkProcessingBlockException(exception)
}

// Set bool value
func (context *Context) SetBool(value bool) error {
	exception := createException()

	C.TDVContext_putBool(context.implementation, C.bool(value), &exception)

	return checkProcessingBlockException(exception)
}

// Set []byte value
func (context *Context) SetDataPointer(value []byte) (unsafe.Pointer, error) {
	exception := createException()
	dataPtr := C.CBytes(value)

	result := unsafe.Pointer(C.TDVContext_putDataPtr(context.implementation, (*C.uint8_t)(dataPtr), C.uint64_t(len(value)), &exception))

	C.free(dataPtr)

	return result, checkProcessingBlockException(exception)
}

// Set ContextTemplate value
func (context *Context) SetContextTemplate(value ContextTemplate) error {
	exception := createException()

	C.TDVContext_putContextTemplate(context.implementation, value.implementation, &exception)

	return checkProcessingBlockException(exception)
}

// Set DynamicTemplateIndex value
func (context *Context) SetDynamicTemplateIndex(value DynamicTemplateIndex) error {
	exception := createException()

	C.TDVContext_putDynamicTemplateIndex(context.implementation, value.implementation, &exception)

	return checkProcessingBlockException(exception)
}

// Get string value from Context
func (context Context) GetString() (string, error) {
	exception := createException()

	result := C.GoString(C.TDVContext_getStr(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get string size from Context
func (context Context) GetStringSize() (uint64, error) {
	exception := createException()

	result := (uint64)(C.TDVContext_getStrSize(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get int64 value from Context
func (context Context) GetInt() (int64, error) {
	exception := createException()

	result := (int64)(C.TDVContext_getLong(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get uint64 value from Context
func (context Context) GetUnsignedInt() (uint64, error) {
	exception := createException()

	result := (uint64)(C.TDVContext_getUnsignedLong(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get float64 value from Context
func (context Context) GetFloat() (float64, error) {
	exception := createException()

	result := (float64)(C.TDVContext_getDouble(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get ContextTemplate value from Context
func (context Context) GetContextTemplate() (ContextTemplate, error) {
	exception := createException()

	result := C.TDVContext_getContextTemplate(context.implementation, &exception)

	return ContextTemplate{implementation: result}, checkProcessingBlockException(exception)
}

// Get DynamicTemplateIndex value from Context
func (context Context) GetDynamicTemplateIndex() (DynamicTemplateIndex, error) {
	exception := createException()

	result := C.TDVContext_getDynamicTemplateIndex(context.implementation, &exception)

	return DynamicTemplateIndex{implementation: result}, checkProcessingBlockException(exception)
}

// Get bool value from Context
func (context Context) GetBool() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_getBool(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get unsafe.Pointer value from Context
func (context Context) GetDataPointer() (unsafe.Pointer, error) {
	exception := createException()

	result := unsafe.Pointer(C.TDVContext_getDataPtr(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is NULL
func (context Context) IsNone() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isNone(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is array
func (context Context) IsArray() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isArray(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is object
func (context Context) IsObject() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isObject(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is bool
func (context Context) IsBool() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isBool(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is int64
func (context Context) IsInt() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isLong(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is uint64
func (context Context) IsUnsignedInt() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isUnsignedLong(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is float64
func (context Context) IsFloat() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isDouble(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is string
func (context Context) IsString() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isString(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is unsafe.Pointer
func (context Context) IsDataPointer() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isDataPtr(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is DynamicTemplateIndex
func (context Context) IsDynamicTemplateIndex() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isDynamicTemplateIndex(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Check if Context is ContextTemplateIndex
func (context Context) IsContextTemplate() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_isContextTemplate(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Copy values from another Context
func (context *Context) Copy(value Context) error {
	exception := createException()

	C.TDVContext_copy(value.implementation, context.implementation, &exception)

	return checkProcessingBlockException(exception)
}

// Create copy of this Context
func (context Context) Clone() (Context, error) {
	exception := createException()

	result := C.TDVContext_clone(context.implementation, &exception)

	return Context{implementation: result}, checkProcessingBlockException(exception)
}

// Clear all data from Context
func (context *Context) Clear() error {
	exception := createException()

	C.TDVContext_clear(context.implementation, &exception)

	return checkProcessingBlockException(exception)
}

// Remove value from Context
func (context *Context) Erase(key string) error {
	exception := createException()
	keyPtr := C.CString(key)

	C.TDVContext_erase(context.implementation, keyPtr, &exception)

	C.free(unsafe.Pointer(keyPtr))

	return checkProcessingBlockException(exception)
}

// Reserve size for array values
func (context *Context) Reserve(size uint64) error {
	exception := createException()

	C.TDVContext_reserve(context.implementation, C.uint64_t(size), &exception)

	return checkProcessingBlockException(exception)
}

// Check if value with key exists
func (context Context) Contains(key string) (bool, error) {
	exception := createException()
	keyPtr := C.CString(key)

	result := (bool)(C.TDVContext_contains(context.implementation, keyPtr, &exception))

	C.free(unsafe.Pointer(keyPtr))

	return result, checkProcessingBlockException(exception)
}

// Check if Context has no values
func (context Context) IsEmpty() (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_empty(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Compare 2 Contexts
func (context Context) Compare(value Context) (bool, error) {
	exception := createException()

	result := (bool)(C.TDVContext_compare(context.implementation, value.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get Context array size
func (context Context) GetLength() (uint64, error) {
	exception := createException()

	result := (uint64)(C.TDVContext_getLength(context.implementation, &exception))

	return result, checkProcessingBlockException(exception)
}

// Get all keys from object Context
func (context Context) GetKeys() ([]string, error) {
	exception := createException()
	length, err := context.GetLength()

	if err != nil {
		return []string{}, err
	}

	temp := C.TDVContext_getKeys(context.implementation, C.uint64_t(length), &exception)

	err = checkProcessingBlockException(exception)

	if err != nil {
		return []string{}, err
	}

	result := make([]string, 0, length)

	for i := 0; i < int(length); i++ {
		result = append(result, getStringByIndex(temp, i))
	}

	return result, err
}

// Add value to Context array
func (context *Context) PushBack(value Context) error {
	exception := createException()

	C.TDVContext_pushBack(context.implementation, value.implementation, C.bool(true), &exception)

	return checkProcessingBlockException(exception)
}

// Convert Context to JSON string
func (context Context) SerializeToJson() (string, error) {
	serializeException := createException()

	temp := C.TDVContext_serializeToJson(context.implementation, &serializeException)
	err := checkProcessingBlockException(serializeException)

	if err != nil {
		return "", err
	}

	result := C.GoString(temp)
	deleteException := createException()

	C.TDVContext_deleteString(temp, &deleteException)

	return result, checkProcessingBlockException(deleteException)
}

// Save Context to .json file
func (context Context) SaveToJsonFile(path string) error {
	exception := createException()
	pathPtr := C.CString(path)

	C.TDVContext_saveToJsonFile(context.implementation, pathPtr, &exception)

	C.free(unsafe.Pointer(pathPtr))

	return checkProcessingBlockException(exception)
}

// Convert Context to map[string]any
func (context Context) ToMap() (map[string]any, error) {
	jsonString, err := context.SerializeToJson()

	if err != nil {
		return nil, err
	}

	var result map[string]any

	return result, json.Unmarshal([]byte(jsonString), &result)
}

// Destroy Context
func (context *Context) Close() error {
	if context.implementation == nil {
		return errors.New("Context already destroyed")
	}

	exception := createException()

	C.TDVContext_destroy(context.implementation, &exception)

	context.implementation = nil

	return checkProcessingBlockException(exception)
}
