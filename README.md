# cgosymbolizer

[![GoDoc](https://godoc.org/github.com/benesch/cgosymbolizer?status.svg)](https://godoc.org/github.com/benesch/cgosymbolizer)

cgosymbolizer teaches the Go runtime to include cgo frames in backtraces.

## Usage

Just import the package for its side effects:

```go
import _ github.com/benesch/cgosymbolizer
```

## Example output

When you get a backtrace from a Go function that's called from a C function,
you'll see the C frames in the backtrace:

```
./example
goroutine 1 [running, locked to thread]:
runtime/debug.Stack(0x405ac8d, 0x8, 0xc000000180)
	/Users/benesch/Sites/go/src/runtime/debug/stack.go:24 +0xa7
runtime/debug.PrintStack()
	/Users/benesch/Sites/go/src/runtime/debug/stack.go:16 +0x22
main.goPrintStack()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:32 +0x20
main._cgoexpwrap_0981d921ddb7_goPrintStack()
	_cgo_gotypes.go:57 +0x20
goPrintStack
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f4e5
c3
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f519
c2
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f529
c1
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f539
main._Cfunc_c1()
	_cgo_gotypes.go:41 +0x41
main.go2()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:27 +0x20
main.go1()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:23 +0x20
main.main()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:19 +0x20
```

Without cgosymbolizer, c1, c2, and c3 would be missing.

You'll also see C frames in backtraces generated due to fatal signals. Here's an
example:

```
goroutine 1 [syscall]:
x_cgo_callers
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407fb83
abort
	/usr/lib/system/libsystem_c.dylib:0 pc=0x7fffb9b2d420
c3
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f51e
c2
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f529
c1
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/./example:0 pc=0x407f539
runtime.cgocall(0x407f500, 0xc000067f58, 0xc00001e118)
	/Users/benesch/Sites/go/src/runtime/cgocall.go:128 +0x65 fp=0xc000067f28 sp=0xc000067ef0 pc=0x40043f5
main._Cfunc_c1()
	_cgo_gotypes.go:41 +0x41 fp=0xc000067f58 sp=0xc000067f28 pc=0x407f2e1
main.go2()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:27 +0x20 fp=0xc000067f68 sp=0xc000067f58 pc=0x407f410
main.go1()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:23 +0x20 fp=0xc000067f78 sp=0xc000067f68 pc=0x407f3e0
main.main()
	/Users/benesch/go/src/github.com/benesch/cgosymbolizer/example/example.go:19 +0x20 fp=0xc000067f88 sp=0xc000067f78 pc=0x407f3b0
runtime.main()
	/Users/benesch/Sites/go/src/runtime/proc.go:198 +0x207 fp=0xc000067fe0 sp=0xc000067f88 pc=0x402a417
runtime.goexit()
	/Users/benesch/Sites/go/src/runtime/asm_amd64.s:1385 +0x1 fp=0xc000067fe8 sp=0xc000067fe0 pc=0x40501d1
```

# Platform support

Only Linux and macOS are supported.

Collecting C stack traces when C code receives a signal—the second example
above—requires additional support from the Go runtime. Go 1.10 only provides
this support for linux/amd64 and linux/ppc64le. Go 1.11 will add support for
freebsd/amd64, and possibly darwin/amd64. For details, follow [golang/go#24518].

[golang/go#24518]: https://github.com/golang/go/issues/24518