#include <stdlib.h>

void goPrintStack(void);

void c3(void) {
    goPrintStack();
    abort();
}

void c2(void) {
    c3();
}

void c1(void) {
    c2();
}
