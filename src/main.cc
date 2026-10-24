#include <cstdio>
#include "test_header.h"

using namespace std;

/*
Shortcuts in emacs:

M-x . // Go to definition
M-x , // Go back to where you were
*/

I32 main() {
    U32 a = 5;
    U32 b = 10;
    printf("Before: %d, %d\n", a, b);
    my_swap(a, b);
    printf("After: %d, %d\n", a, b);
    for (int i = 0; i < 10; ++i) {
        printf("%d\n", i);
    }
}
