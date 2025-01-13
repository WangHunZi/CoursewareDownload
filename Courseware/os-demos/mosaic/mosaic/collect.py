#!/usr/bin/env python3

import json
import sys
from rich import print

G = json.load(sys.stdin)

V, E = G['vertices'], G['edges']

outputs = set()
for v in V:
    if not v['choices']:
        # No choices; program terminated. Collect the output.
        out = v['stdout'].replace('\n', '⏎ ')
        outputs.add(out)

for s in sorted(list(outputs)):
    print(s)
print(f'|V| = {len(V)}, |E| = {len(E)}.')
print(f'There are {len(outputs)} distinct outputs.')
