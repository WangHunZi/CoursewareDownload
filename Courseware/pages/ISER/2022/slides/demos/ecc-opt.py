#!/usr/bin/env python3

from lark import Lark, Transformer, v_args
import sys
from z3 import *

grammar = r"""
    ?start: sum

    ?sum: product
        | sum "+" product   -> add
        | sum "-" product   -> sub

    ?product: atom
        | product "*" atom  -> mul

    ?atom: NUMBER           -> num
         | "x"              -> arg
         | "(" sum ")"

    %import common.CNAME -> NAME
    %import common.NUMBER
    %import common.WS
    %ignore WS
"""

x, c1, c2 = BitVecs('x c1 c2', 32)

@v_args(inline=True)
class Compiler(Transformer):
    num = lambda _, val: val
    arg = lambda _: x
    add = lambda _, x, y: x + y
    sub = lambda _, x, y: x - y
    mul = lambda _, x, y: x * y

parser = Lark(grammar, parser='lalr', transformer=Compiler())

templates = [
    'c1 * x + c2',
    '(x << c1) + (x << c2)',
]

expr = sys.argv[1]
f = simplify(parser.parse(expr))

print(f'Optimize {expr}:')

for t in templates:
    s = Solver()
    s.add(ForAll([x], f == eval(t)))
    s.add([0 <= c1, c1 < 32, 0 <= c2, c2 < 32])
    if s.check() == sat:
        m = s.model()
        res = t
        for v in m:
            res = res.replace(str(v), str(m[v]))
        print(f'  == {res}')
