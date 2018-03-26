package cgosymbolizer

import (
	"runtime"
)

// #include "cgosymbolizer_darwin.h"
import "C"

func init() {
	const structVersion = 0
	runtime.SetCgoTraceback(structVersion, C.cgo_traceback, C.cgo_context, C.cgo_symbolizer)
}
