#!/usr/bin/env python3

# Objects from: https://people.sc.fsu.edu/~jburkardt/data/obj/obj.html
# Usage (in Python REPL):
#     exec(open('lt.py').read())
#     R2D(R2D(shuttle))

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from pathlib import Path
from collections import defaultdict
from matplotlib.ticker import MaxNLocator

plt.rcParams['backend'] = 'Qt5Agg'
plt.rcParams['toolbar'] = 'None'
plt.rcParams['grid.color'] = '#ddd'

class Object:
    @staticmethod
    def load(fname):
        '''Parse 3D object from file as drawable parts.'''

        limit, parts, vertices, groups = 0, [], [], []

        for line in Path(fname).read_text().splitlines():
            if not (t := line.split()): continue
            tag, contents = t[0], t[1:]
            if tag == 'g': # group; as separate plot
                groups.append(defaultdict(lambda: set()))
            if tag == 'v': # vertex; simply keep it
                x, y, z = tuple(float(x) for x in contents)
                vertices.append((x, y, z))
                limit = max(limit, abs(x), abs(y), abs(z))
            if tag == 'f': # face; add all edges in the face
                path = [int(i) - 1 for i in contents]
                for i, _ in enumerate(path):
                    xs, u, v = groups[-1], path[i - 1], path[i]
                    xs[u].add(v)
                    xs[v].add(u)

        for G in filter(lambda x: x, groups):
            parts.append([])
            visited = set()

            def dfs(u):
                if u not in visited:
                    visited.add(u)
                    for v in G[u]:
                        yield vertices[v]
                        yield from dfs(v)
                        yield vertices[u]

            for u in G:
                if u not in visited:
                    parts[-1] += list(dfs(u))

        parts = [
            [ (0, 0, 0), (1, 0, 0) ],
            [ (0, 0, 0), (0, 1, 0) ],
            [ (0, 0, 0), (0, 0, 1) ] ] + parts
        return Object(limit, parts)

    def __init__(self, limit, parts):
        self.limit = limit
        self.parts = parts

    def apply(self, M):
        '''Apply linear transformation M to the object.'''

        def mult(M, v):
            x, y, z = v
            x1 = M[0][0] * x + M[0][1] * y + M[0][2] * z
            y1 = M[1][0] * x + M[1][1] * y + M[1][2] * z
            z1 = M[2][0] * x + M[2][1] * y + M[2][2] * z
            return (x1, y1, z1)

        # for all points, do v' = M * v
        lines, s = [], [[mult(M, v) for v in seg] for seg in self.parts]

        fig = plt.figure(figsize=(7,6.89))
        ax = fig.add_subplot(projection='3d')
        ax.cla()
        for fn in [ax.set_xlim, ax.set_ylim, ax.set_zlim]:
            fn(-self.limit, self.limit)
        for axis in [ax.xaxis, ax.yaxis, ax.zaxis]:
            axis.set_major_locator(MaxNLocator(4)) 
        ax.plot([0], [0], [0], 'ko', markersize=2)
        for i, xs in enumerate(self.parts):
            if i < 3: # axis
                fmt, conf = 'rgb'[i] + '-', {}
            else: # other parts to be plotted
                fmt, conf = 'o-', {'linewidth': 0.4, 'markersize': 0.8}
            lines += ax.plot(*[[p[d] for p in xs] for d in range(3)], fmt, **conf)

        def animate(i):
            t = min(i / 20, 1)
            for i, seg in enumerate(self.parts):
                v = [[0] * len(seg) for _ in (0, 1, 2)]
                for j, _ in enumerate(seg):
                    for k in (0, 1, 2):
                        v[k][j] = (1-t) * self.parts[i][j][k] + t * s[i][j][k]
                lines[i].set_data_3d(*v)

        anim = FuncAnimation(fig, animate, interval=100, frames=30)
        assert anim
        mngr = plt.get_current_fig_manager()
        _, _, dx, dy = mngr.window.geometry().getRect()
        mngr.window.setGeometry(7, 0, dx, dy)
        plt.show()
        return Object(self.limit, s)

sqrt2 = 2 ** 0.5

matrices = {
    'I':    # Identity
        ( (1, 0, 0),
          (0, 1, 0),
          (0, 0, 1) ),
    'Z':    # Zero
        ( (0, 0, 0),
          (0, 0, 0),
          (0, 0, 0) ),
    'Ex':
        ( (0, 0, 0),
          (0, 1, 0),
          (0, 0, 1) ),
    'Ey':
        ( (1, 0, 0),
          (0, 0, 0),
          (0, 0, 1) ),
    'Ez':
        ( (1, 0, 0),
          (0, 1, 0),
          (0, 0, 0) ),
    'E':
        ( (1/2, 1, 1/3),
          (1, 2, 2/3),
          (1, 0, 1) ),
    'Half':
        ( (1/2, 0, 0),
          (0, 1/2, 0),
          (0, 0, 1/2) ),
    'HalfNeg':
        ( (-1/2, 0, 0),
          (0, -1/2, 0),
          (0, 0, -1/2) ),
    'Rx': # Transforming an axis
        ( (0.5,  0, 0),
          (-0.3, 1, 0),
          (0.6,  0, 1) ),
    'R1': # A rotation without scaling
        ( ( 1 / sqrt2, 0, 1/sqrt2),
          ( 1 / 2, 1 / sqrt2, -1 / 2),
          ( -1 / 2, 1 / sqrt2, 1 / 2) ),
    'R2D':  # 45-deg rotation in 2D (z=1) plane
        ( (1 / sqrt2, -1 / sqrt2, 0),
          (1 / sqrt2,  1 / sqrt2, 0),
          (0, 0,                    1) ),
    'M2D':  # Shifting in 2D (z=1) plane
        ( (1, 0,  1),
          (0, 1, -1),
          (0, 0,  1) ),
    'Rz':   # Rotate z to negative side of x
        ( (1, 0, 0),
          (0, 1, 0),
          (-1, 0, 1) ),
    'Rz_':   # Rotate z to positive side of x
        ( (1, 0, 0),
          (0, 1, 0),
          (1, 0, 1) ),
    'JT':    # Linear transformations in Ji-Tu Problem
        ( (1, 1, 0),
          (2, 4, 0),
          (0, 0, 1) ),
    'JT1':
        ( (1, 0, 0),
          (0, 1/2, 0),
          (0, 0, 1) ),
    'JT2':
        ( (1, 0, 0),
          (-1, 1, 0),
          (0, 0, 1) ),
    'JT3':
        ( (1, -1, 0),
          (0, 1, 0),
          (0, 0, 1) ),
    'Compress': # Compressing 3D to 2D
        ( (1, 0, 1),
          (1/2, 1/2, 1),
          (0, 1, 1) ),
}

for f in Path('objs').glob('*.obj'):
    print(f'Object: {f.stem}')
    globals()[f.stem] = Object.load(f)

for name, mat in matrices.items():
    print(f'Matrix: {name}')
    globals()[name] = lambda obj, mat=mat: obj.apply(mat)
