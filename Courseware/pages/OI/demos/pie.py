import sys, z3

if len(sys.argv) < 2:
    n = 3
    print(f'Usage: {sys.argv[0]} number-of-sets (default {n=})\n')
else:
    n = int(sys.argv[1])

subsets = [
    { i for i in range(n) if (x >> i) & 1 }
        for x in range(1, 2**n) ]

subsets.sort(key=lambda x: (len(x), tuple(x)))

z3.set_option(max_args=8)
solver = z3.Solver()
X = [z3.Int(f'x{i+1}') for i, _ in enumerate(subsets)]
for i, s in enumerate(subsets):
    cons = z3.Sum([X[j] for j, s1 in enumerate(subsets) if s1.issubset(s)]) == 1
    solver.add(cons)
    print(cons)
print('-' * 60)

if solver.check() == z3.sat:
    coefs = [solver.model().evaluate(x).as_long() for x in X]
    format = lambda s, ch=' U ': ('|' + ch.join(["ABCDEFGHIJK"[i] for i in sorted(list(s))]) + '|')
    print(format(subsets[-1], ' ⋃ '), '=')
    for i, (c, s) in enumerate(zip(coefs, subsets)):
        desc = format(s, ' ⋂ ')
        if i != 0 and coefs[i] != coefs[i - 1]: print('\n  ', end='')
        if i != 0 and c > 0: pm = ' + '
        elif c < 0: pm = ' - '
        else: pm = '  '
        if c == 1: print(f'{pm}{desc}', end='')
        elif c == -1: print(f'{pm}{desc}', end='')
        elif c > 0: print(f'{pm}{c}{desc}', end='')
        elif c < 0: print(f'{pm}{abs(c)}{desc}', end='')
    print()
else:
    raise Exception('No solution!')
