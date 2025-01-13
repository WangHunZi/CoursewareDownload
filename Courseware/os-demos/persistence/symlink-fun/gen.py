#!/usr/bin/env python3

from pathlib import Path
import shutil

Board = '''
.###C..
A.X#.#.
#..#.#.
...B.#*
'''.strip().splitlines()

root = Path('./game').resolve()
shutil.rmtree(root, ignore_errors=True) 
root.mkdir(exist_ok=True)

for i, row in enumerate(Board):
    for j, ch in enumerate(row):
        if ch != '#':
            d = root / f'{i},{j}'
            d.mkdir(exist_ok=True)
            for x, y, move in \
                    [(i + 1, j, 'Down'),
                     (i, j + 1, 'Right'),
                     (i - 1, j, 'Up'),
                     (i, j - 1, 'Left')]:
                if (0 <= x < len(Board) and
                    0 <= y < len(Board[0]) and
                    Board[x][y] != '#'):
                    (d / move).symlink_to(root / f'{x},{y}')

            match ch:
                case '.': name = 'Nothing.'
                case '*': name = 'Exit.'
                case _: name = ch + '.'
                
            (d / name).mkdir(exist_ok=True)

(root / 'start').symlink_to(root / '0,0')
