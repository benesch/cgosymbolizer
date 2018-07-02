#include <dlfcn.h>
#include <libunwind.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cgosymbolizer_darwin.h"

struct cgo_context {
    bool used;
    unw_context_t unw_ctx;
    unw_cursor_t unw_cursor;
};

__thread struct cgo_context thread_ctx;

void cgo_traceback(void* p) {
    struct {
	    uintptr_t ctx;
	    uintptr_t sig_ctx;
	    uintptr_t* buf;
	    uintptr_t max;
    }* arg = p;

    struct cgo_context ctx;
    if (arg->ctx)
        ctx = *(struct cgo_context*) arg->ctx;
    else {
        if (unw_getcontext(&ctx.unw_ctx) != 0)
            return;
        if (unw_init_local(&ctx.unw_cursor, &ctx.unw_ctx) != 0)
            return;
        // Skip the cgo_traceback, x_cgo_callers, and _sigtramp frames.
        for (int i = 0; i < 3; i++)
            if (unw_step(&ctx.unw_cursor) == 0)
                return;
    }

    int i = 0;
    while (i < arg->max) {
        unw_word_t pc;
        unw_get_reg(&ctx.unw_cursor, UNW_REG_IP, &pc);
        if (pc == 0)
            break;
        arg->buf[i++] = pc;
        if (unw_step(&ctx.unw_cursor) == 0)
            break;
    }
    if (i < arg->max)
        arg->buf[i] = 0;
}

void cgo_context(void* p) {
    struct {
        uintptr_t ctx;
    }* arg = p;

    if (arg->ctx != 0) {
        struct cgo_context* ctx = (struct cgo_context*) arg->ctx;
        ctx->used = 0;
        return;
    }

    if (thread_ctx.used)
        return;
    if (unw_getcontext(&thread_ctx.unw_ctx) != 0)
        return;
    if (unw_init_local(&thread_ctx.unw_cursor, &thread_ctx.unw_ctx) != 0)
        return;
    // Step the saved state past this frame. It will cease to exist momentarily.
    if (unw_step(&thread_ctx.unw_cursor) <= 0)
        return;
    arg->ctx = (uintptr_t) &thread_ctx;
}

void cgo_symbolizer(void* p) {
    struct {
        uintptr_t pc;
        const char* file;
        uintptr_t lineno;
        const char* func;
        uintptr_t entry;
        uintptr_t more;
        uintptr_t data;
    }* arg = p;

    Dl_info dlinfo;
    if (dladdr((void *) arg->pc, &dlinfo) == 0) {
        arg->file = "?";
        arg->func = "?";
        arg->entry = 0;
        return;
    }
    arg->file = dlinfo.dli_fname;
    arg->func = dlinfo.dli_sname;
    arg->entry = (uintptr_t) dlinfo.dli_saddr;
}
