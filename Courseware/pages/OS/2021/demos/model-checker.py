# LIFE IS SHORT. USE PYTHON!
import sys, re, yaml

def parse(code):
  for line in code.strip().splitlines():
    yield re.sub(r'GOTO\((.*?)\)', r'pc = \1 - 1', line.strip() + '\npc += 1')

src       = yaml.safe_load(sys.stdin.read())
P         = { t: list(parse(code)) for t, code in src.items() if not t.startswith('_') }
bug_on    = src.get('_bug_on', 'False')
mark_on   = src.get('_mark_on', 'False')
variables = list(src['_init'].keys()) + ['PC']
threads   = list(P.keys())
s0        = { 'PC': { t: 0 for t in threads }, **src['_init'] }

def expand(s, t):
  pc   = s['PC'][t]
  stmt = P[t][pc]
  s1   = { **s, 'PC': s['PC'].copy(), 'pc': pc }
  exec(stmt, {}, s1) # hah! we're cheating!
  s1['PC'][t] = s1['pc']
  return { k: s1[k] for k in variables }

def H(s):
  return hash(str(s))

queue, explored, nodes, edges = [ s0 ], { H(s0) }, [ s0 ], []
while queue:
  s, queue = queue[0], queue[1:]
  for t in threads:
    s1 = expand(s, t)
    if H(s1) not in explored:
      nodes.append(s1)
      queue.append(s1)
      explored.add(H(s1))
    edges.append((H(s), H(s1), t))

print(r'digraph G { rankdir=TB; node [shape=record,fontname="Sans Serif"];')
for s in nodes:
  if eval(bug_on, {}, s):
    color = '#cc3300'
  elif eval(mark_on, {}, s):
    color = '#3366ff'
  else:
    color = '#dfffdf'

  node_text = '{' + \
      '{' + ', '.join(f'{t}@{s["PC"][t]}' for t in threads) + '}' + '|' + \
      '{' + '| '.join(f'{v} = {s[v]}' for v in variables if v != 'PC') + '}' + \
    '}'
  print(f'{H(s)} [label="{node_text}" style="filled,rounded" fillcolor="{color}"];')
for src, dest, t in edges:
  print(f'{src} -> {dest} [label="{t}"];')
print('}')
