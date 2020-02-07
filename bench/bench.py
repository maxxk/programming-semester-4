def b0(a):
    return a + 1

b1 = lambda a: b0(b0(b0(b0(a))))
b2 = lambda a: b1(b1(b1(b1(a))))
b3 = lambda a: b2(b2(b2(b2(a))))
b4 = lambda a: b3(b3(b3(b3(a))))
b5 = lambda a: b4(b4(b4(b4(a))))
b6 = lambda a: b5(b5(b5(b5(a))))
b7 = lambda a: b6(b6(b6(b6(a))))
b8 = lambda a: b7(b7(b7(b7(a))))
b9 = lambda a: b8(b8(b8(b8(a))))
ba = lambda a: b9(b9(b9(b9(a))))
bb = lambda a: ba(ba(ba(ba(a))))
bc = lambda a: bb(bb(bb(bb(a))))
bd = lambda a: bc(bc(bc(bc(a))))
be = lambda a: bd(bd(bd(bd(a))))
print(be(1))
