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
    bitarr.print();    
    bitarr.SetBit(63);
    bitarr.SetBit(64);
    bitarr.print();
    bitarr.ShiftDown(1);
    bitarr.print();
    bitarr.ShiftUp(1);
    bitarr.print();
}
