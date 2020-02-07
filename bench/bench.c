#include <stdio.h>

long b0(long a) { return a + 1; }
long b1(long a) { return b0(b0(b0(b0(a)))); }
long b2(long a) { return b1(b1(b1(b1(a)))); }
long b3(long a) { return b2(b2(b2(b2(a)))); }
long b4(long a) { return b3(b3(b3(b3(a)))); }
long b5(long a) { return b4(b4(b4(b4(a)))); }
long b6(long a) { return b5(b5(b5(b5(a)))); }
long b7(long a) { return b6(b6(b6(b6(a)))); }
long b8(long a) { return b7(b7(b7(b7(a)))); }
long b9(long a) { return b8(b8(b8(b8(a)))); }
long ba(long a) { return b9(b9(b9(b9(a)))); }
long bb(long a) { return ba(ba(ba(ba(a)))); }
long bc(long a) { return bb(bb(bb(bb(a)))); }
long bd(long a) { return bc(bc(bc(bc(a)))); }
long be(long a) { return bd(bd(bd(bd(a)))); }

int main(void) {
    printf("%ld", be(1));
    return 0;
}
