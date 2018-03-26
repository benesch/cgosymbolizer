// Package cgosymbolizer teaches the Go runtime to include cgo frames in
// backtraces.
//
// To use the package, import it for its side effects:
//
//     import _ github.com/benesch/cgosymbolizer
//
// cgosymbolizer only supports Linux and macOS. Importing it on another
// operating system will have no effect.
//
// Support is further limited by Go's implementation of runtime.SetCgoTraceback.
// On at least linux/amd64 and linux/ppc64le, Go can collect a C traceback, with
// cgosymbolizer's help, if the C code receives a signal. This permits
// visibility into C code in CPU profiles and fatal signals. Otherwise
// cgosymbolizer can only collect a C stack trace when a C function calls a Go
// function. For details, follow https://github.com/golang/go/issues/24518.
//
// Note that the Linux implementation is provided by
// github.com/ianlancetaylor/cgosymbolizer. The macOS implementation is original
// work.
package cgosymbolizer
