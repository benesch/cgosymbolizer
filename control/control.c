#include <libunwind.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void handlesegvimpl(void) {
    unw_context_t ctx;
    unw_cursor_t cursor;
    if (unw_getcontext(&ctx) != 0)
        return;
    if (unw_init_local(&cursor, &ctx) != 0)
        return;
    int i = 0;
    do {
        unw_word_t pc, offset;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0)
            break;
        char sym[256];
        unw_get_proc_name(&cursor, sym, sizeof(sym), &offset);
        fprintf(stderr, "#%d: 0x%012llx\t%s+0x%llx\n", i++, pc, sym, offset);
    } while (unw_step(&cursor) > 0);
}

void handlesegv(int _) {
    handlesegvimpl();
    exit(1);
}

void segfault(void) {
    volatile char q = *(volatile char *)NULL;
}

// This program, when compiled with -O0 -g, should output something like
// the following:
//
//     #0: 0x000105861d65	handlesegvimpl+0x25
//     #1: 0x000105861ea0	handlesegv+0x10
//     #2: 0x7fff5fdfbf5a	_sigtramp+0x1a
//     #3: 0x000105861eb9	segfault+0x9
//     #4: 0x000105861eee	main+0x2e
//
int main(int argc, const char* argv[]) {
    signal(SIGSEGV, handlesegv);
    segfault();
}
