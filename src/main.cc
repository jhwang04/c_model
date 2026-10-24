#include <stdio.h>
#include "bitarray.h"
#include "test_header.h"

using namespace std;

/*
Shortcuts in emacs:

M-x . // Go to definition
M-x , // Go back to where you were
*/

I32 main() {
    printf("Before...\n");
    BitArray<72> bitarr;
    for (U64 i = 0; i < 72; ++i) {
      printf("bitarr at %llu is %d\n", i, bitarr.IsBitSet(i));
    }
}
