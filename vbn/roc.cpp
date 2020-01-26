#include "roc.h"

int f() {
    int a = g();
    return 1;
}

int g() {
    return 2;
}

int h() {
    return 3;
}
