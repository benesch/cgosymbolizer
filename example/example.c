#include <stdlib.h>

void goPrintStack(void);

void c3(void) {
    goPrintStack();
    volatile char p = *(volatile char*) NULL;
}

void c2(void) {
    c3();
}

void c1(void) {
    c2();
}
