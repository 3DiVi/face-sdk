package facesdk

/*
#include <stdlib.h>

void TDVProcessingBlock_processContext(void* handle_, void* config, void** eh);
void TDVProcessingBlock_destroyBlock(void* handle_, void** eh);
*/
import "C"
import (
	"errors"
	"unsafe"
)

type ProcessingBlock struct {
	implementation unsafe.Pointer
}

// Infer
func (block *ProcessingBlock) Process(context Context) error {
	exception := createException()

	C.TDVProcessingBlock_processContext(block.implementation, context.implementation, &exception)

	return checkProcessingBlockException(exception)
}

// Destroy ProcessingBlock
func (block *ProcessingBlock) Close() error {
	if block.implementation == nil {
		return errors.New("ProcessingBlock already destroyed")
	}

	exception := createException()

	C.TDVProcessingBlock_destroyBlock(block.implementation, &exception)

	block.implementation = nil

	return checkProcessingBlockException(exception)
}
