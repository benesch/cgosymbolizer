#include <dlfcn.h>
#include <libunwind.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "cgosymbolizer_darwin.h"

struct cgo_context {
    unw_context_t unw_ctx;
    unw_cursor_t unw_cursor;
};

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
    }

    int i = 0;
    while (i < arg->max && unw_step(&ctx.unw_cursor) > 0) {
        unw_word_t pc;
        unw_get_reg(&ctx.unw_cursor, UNW_REG_IP, &pc);
        if (pc == 0)
            break;
        arg->buf[i++] = pc;
    }
    if (i < arg->max)
        arg->buf[i] = 0;
}

void cgo_context(void* p) {
    struct {
        uintptr_t ctx;
    }* arg = p;

    if (arg->ctx != 0) {
        free((struct cgo_context*) arg->ctx);
        return;
    }

    struct cgo_context* ctx = malloc(sizeof(cgo_context));
    if (ctx == NULL)
        return;
    if (unw_getcontext(&ctx->unw_ctx) != 0)
        return;
    if (unw_init_local(&ctx->unw_cursor, &ctx->unw_ctx) != 0)
        return;
    // Step the saved state past this frame. It will cease to exist momentarily.
    if (unw_step(&ctx->unw_cursor) <= 0)
        return;
    arg->ctx = (uintptr_t) ctx;
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
