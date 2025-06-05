package facesdk

/*
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	char* data;
	size_t size;
	size_t offset;
} WriteData;

typedef struct {
	const char* data;
	size_t offset;
} ReadData;

const char* TDVException_getMessage(void* eh);
void TDVException_deleteException(void* eh);
const char* __4848a76477c449608aa5deb15c5495e4_facerec_v3_apiException_what(void* exception);
void __4848a76477c449608aa5deb15c5495e4_facerec_v3_apiObject_destructor(void* object);

void writeFunction(void* data, const void* buffer, uint64_t bytesCount)
{
	WriteData* ptr = (WriteData*)data;

	if (ptr->offset + bytesCount > ptr->size)
	{
		char* temp = ptr->data;

		ptr->data = (char*)malloc(ptr->size * 2);

		memcpy(ptr->data, temp, ptr->offset);

		free(temp);
	}

	memcpy(ptr->data + ptr->offset, buffer, bytesCount);

	ptr->offset += bytesCount;
}

void readFunction(void* data, void* buffer, uint64_t bytesCount)
{
	ReadData* ptr = (ReadData*)data;

	memcpy(buffer, ptr->data + ptr->offset, bytesCount);

	ptr->offset += bytesCount;
}
*/
import "C"
import (
	"errors"
	"unsafe"
)

func createException() unsafe.Pointer {
	var exceptionType unsafe.Pointer

	return C.malloc(C.size_t(unsafe.Sizeof(exceptionType)))
}

func checkProcessingBlockException(exception unsafe.Pointer) error {
	if exception != nil {
		errorMessage := C.GoString(C.TDVException_getMessage(exception))

		C.TDVException_deleteException(exception)

		return errors.New(errorMessage)
	}

	return nil
}

func checkApiException(exception unsafe.Pointer) error {
	if exception != nil {
		errorMessage := C.GoString(C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_apiException_what(exception))

		C.__4848a76477c449608aa5deb15c5495e4_facerec_v3_apiObject_destructor(exception)

		return errors.New(errorMessage)
	}

	return nil
}

func getStringByIndex(cArray **C.char, index int) string {
	ptr := *(**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(cArray)) + uintptr(index)*unsafe.Sizeof(*cArray)))

	return C.GoString(ptr)
}

func createCStringArray(goStrings []string) (**C.char, func()) {
	cArray := C.malloc(C.size_t(len(goStrings)) * C.size_t(unsafe.Sizeof(uintptr(0))))
	stringArray := (**C.char)(cArray)
	cStrings := make([]*C.char, len(goStrings))

	for i, s := range goStrings {
		cStr := C.CString(s)
		cStrings[i] = cStr
		ptr := (**C.char)(unsafe.Pointer(uintptr(unsafe.Pointer(stringArray)) + uintptr(i)*unsafe.Sizeof(uintptr(0))))
		*ptr = cStr
	}

	cleanup := func() {
		for _, cStr := range cStrings {
			C.free(unsafe.Pointer(cStr))
		}
		C.free(unsafe.Pointer(cArray))
	}

	return stringArray, cleanup
}
