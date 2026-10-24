
#include <cstdio>
#include "test_header.h"

using namespace std;

/*
Shortcuts in emacs:

M-x . // Go to definition
M-x , // Go back to where you were


*/

int main()
{
    int a = 5;
    int b = 10;
    printf("Before: %d, %d\n", a, b);
    my_swap(a, b);
    printf("After: %d, %d\n", a, b);
}
