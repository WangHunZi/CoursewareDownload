#!/usr/bin/env python3

from lark import Lark, Transformer, v_args
from pathlib import Path
import sys

grammar = r"""
    ?start: NAME ":" sum    -> fun

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

@v_args(inline=True)
class Compiler(Transformer):
    @staticmethod
    def build(left, right, inst):
        return left + right + ['pop %rax', 'pop %rsi', f'{inst} %rsi, %rax', 'push %rax']

    fun = lambda _, name, seq: [f'.globl {name}', f'{name}:'] + seq + ['pop %rax', 'ret']
    num = lambda _, x: [f'push ${x}']
    arg = lambda _: [f'push %rdi']
    add = lambda _, left, right: Compiler.build(left, right, 'add')
    sub = lambda _, left, right: Compiler.build(left, right, 'sub')
    mul = lambda _, left, right: Compiler.build(left, right, 'imul')

def compile(file: Path):
    if file.suffix == '.e':
        parser = Lark(grammar, parser='lalr', transformer=Compiler())
        inst = parser.parse(file.read_text())
        Path(f'{file.stem}.s').write_text('\n'.join(inst))
    else:
        exit(1)

compile(Path(sys.argv[1]))
