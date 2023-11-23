import sys, re, graphviz, jinja2, json, markdown, argparse
from pathlib import Path
from collections import namedtuple

EMPTY = '␡'
COLORS = {None: '#f1f5f9',
          'red': '#fecaca',
          'yellow': '#fef08a',
          'green': '#bbf7d0',
          'blue': '#bfdbfe',
          'purple': '#e9d5ff'
          }

Vertex = namedtuple('Vertex', 'name state marks')
Edge = namedtuple('Edge', 'name u v t')
State = namedtuple('State', 'pcs lvars gvars')

vertices, edges, threads = {}, [], []
code_lines, pcmap, gvar, lvar = [], {}, [], []


def parse_input():
    def CLASS(cl):
        global hacked_src
        hacked_src = cl

    def STATE(u, state, marks):
        vertices[u] = Vertex(name=u, state=state, marks=marks)

    def TRANS(u, v, t):
        edges.append(Edge(name=f'{u}-{v}', u=vertices[u], v=vertices[v], t=t))
        if t not in threads:
            threads.append(t)

    for line in sys.stdin.readlines():
        eval(line)


def parse_src():
    md_lines = ['    :::python']
    for line in hacked_src.splitlines():
        if 'yield' not in line:
            md_lines.append(f'    {line.replace("    ", "  ")}')
    html = markdown.markdown('\n'.join(md_lines), extensions=['codehilite'])
    code_lines.extend(re.search(r'<code.*?>(.*)</code>', html, re.DOTALL).group(1).rstrip().splitlines())

    new_pc = 0
    for pc, line in enumerate(hacked_src.splitlines()):
        if 'yield' not in line:
            pcmap[pc + 1] = (new_pc := new_pc + 1)


def parse_vars():
    gv, lv = set(), set()
    for v in vertices.values():
        for t, val in v.state.items():
            if t not in threads:
                gv.add(t)
            else:
                lv |= set((t, var) for var in val[1])
    gvar.extend(sorted(list(gv)))
    lvar.extend(sorted(list(lv)))


def parse_state(s):
    gvars = [s[g] for g in gvar]
    lvars = ['⊥' for _ in enumerate(lvar)]
    pcs = [-1 for _ in enumerate(threads)]

    for t in [t for t in threads if t in s]:
        tpc, tstate = s[t]
        pcs[threads.index(t)] = pcmap[tpc + 1]
        for vname, val in tstate.items():
            lvars[lvar.index((t, vname))] = val

    return State(pcs, lvars, gvars)


def reduce(tree_only=False):
    tree, depth, leaf = [], {'s0': 0}, {'s0': True}
    updated, rnd = 0, 0
    while updated + len(threads) > rnd:
        rnd += 1

        def expand():
            found = False
            for e in edges:
                _, u, v, t = e
                if u.name in depth and v.name not in depth and t == threads[-rnd % len(threads)]:
                    depth[v.name] = depth[u.name] + 1
                    leaf[v.name] = True
                    leaf[u.name] = False
                    tree.append(e)
                    found = True
            return found

        while expand():
            updated = rnd

    others = []
    if not tree_only:
        tnames = set(e.name for e in tree)
        for e in edges:
            name, u, v, t = e
            if name not in tnames:
                w = filter(lambda e1: e1.v.name == u.name, edges).__next__()
                if (leaf[e.u.name] and w.t == e.t) or (depth[u.name] >= depth[v.name]):
                    others.append(e)
    return tree, others


parse_input()
parse_src()
parse_vars()

parser = argparse.ArgumentParser(description='Visualize modeler checker outputs')
parser.add_argument('--tree', '-t', help='Draw tree', action='store_true')
parser.add_argument('--reduce', '-r', help='Draw reduced graph', action='store_true')
args = parser.parse_args()

if args.reduce or args.tree:
    edges, others = reduce(args.tree)
else:
    edges, others = edges, []

COLORS |= dict(zip(threads, ['#be123c', '#1d4ed8', '#eab308', '#4d7c0f']))

g = graphviz.Digraph('G', filename='/tmp/a.gv',
                     graph_attr={'fontsize': '12', 'layout': 'dot', 'nodesep': '0.75'})
metadata = {'s0': vertices['s0'].name}

with g.subgraph(name='legend') as c:
    c.node_attr = {'style': 'filled', 'fontname': 'Courier New', 'shape': 'Mrecord'}

    # draw vertices and code blocks
    for v in vertices.values():
        ps = parse_state(v.state)
        color = ([None] + [col for col in v.marks if col])[-1]

        gvals, lvals = '| '.join([f'{g}={v.state[g]}' for g in gvar]), ''
        for i, t in enumerate(threads):
            if i != 0: lvals += '|'
            lvals += '{' + '| '.join(
                [f'{t}.{l}={v.state.get(t, (0, {}))[1].get(l, EMPTY)}' for t1, l in lvar if t1 == t]) + '}'
        label = '{' + '{' + gvals + '}' + '|' + '{' + lvals + '}' + '}'
        label = label.replace('True', 'T').replace('False', 'F')
        c.node(v.name, id=v.name, label=label, fillcolor=COLORS.get(color, color))

        lines = ['<div class="codehilite">']
        for i, line in enumerate(code_lines):
            cl = 'new' if i + 1 in ps.pcs else ''
            lines.append(f'<pre class="{cl}"><code>{line if line else " "}</pre></code>')
        lines.append('<div class="vars">')
        for i, nm in enumerate(gvar):
            lines.append(f'<pre><code>{nm} = {ps.gvars[i]}</code></pre>')
        for i, (t, nm) in enumerate(lvar):
            lines.append(f'<pre><code>{t}.{nm} = {ps.lvars[i]}</code></pre>')
        lines.append('</div></div>')
        metadata[v.name] = '\n'.join(lines)

    # draw edges
    for idx, (name, u, v, t) in enumerate(edges + others):
        ps_old, ps_new = parse_state(u.state), parse_state(v.state)
        c.edge(u.name, v.name, id=name, fontcolor=COLORS[t], color=COLORS[t], label=t, fontname='Helvetica')

        diff = lambda x, y: f'<span class="new">{y}</span> <span class="old">({x})</span>' if x != y else x
        lines = ['<div class="codehilite">']
        for i, line in enumerate(code_lines):
            if i + 1 in ps_new.pcs:
                cl = 'new'
            elif i + 1 in ps_old.pcs:
                cl = 'old'
            else:
                cl = ''
            lines.append(f'<pre class="{cl}"><code>{line if line else " "}</pre></code>')
        lines.append('<div class="vars">')
        for i, nm in enumerate(gvar):
            lines.append(f'<pre><code>{nm} = {diff(ps_old.gvars[i], ps_new.gvars[i])}</code></pre>')
        for i, (tname, nm) in enumerate(lvar):
            lines.append(f'<pre><code>{tname}.{nm} = {diff(ps_old.lvars[i], ps_new.lvars[i])}</code></pre>')
        lines.append('</div></div>')
        metadata[name] = '\n'.join(lines)

TEMPLATE = '''
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
      body {
        margin: 0; padding: 8px 16px; box-sizing: border-box;
        overflow: hidden;
      }
      #container {
        overflow: auto;
        width: 100%;
        height: 100%;
        position: absolute;
      }
      .codehilite {
        padding: 10px;
      }
      .old {
        background-color: #d4d4d4;
        opacity: 0.5;
      }
      .new {
        background-color: #f0abfc;
      }
      #code {
        font-size: 135%;
        border: 1px solid;
        top: 0;
        right: 0;
        width: 500px;
        position: absolute;
        float: right;
        background: white;
      }
      .vars {
        position: absolute;
        float: right;
        top: 5px; right: 505px;
        text-align: right;
        background-color: white;
        padding: 5px;
      }
      #mouse-circle {
        position: absolute;
        width: 32px;
        height: 32px;
        margin: -16px 0 0 -16px;
        border: 1px solid green;
        background-color: rgba(0, 255, 0, 0.5);
        border-radius: 50%;
        pointer-events: none;
        box-shadow: rgba(0, 255, 0, 0.35) 0px 5px 15px;
      }

      pre { line-height: 125%; margin: 0; }
      td.linenos .normal { color: inherit; background-color: transparent; padding-left: 5px; padding-right: 5px; }
      span.linenos { color: inherit; background-color: transparent; padding-left: 5px; padding-right: 5px; }
      td.linenos .special { color: #000000; background-color: #ffffc0; padding-left: 5px; padding-right: 5px; }
      span.linenos.special { color: #000000; background-color: #ffffc0; padding-left: 5px; padding-right: 5px; }
      .codehilite .hll { background-color: #ffffcc }
      .codehilite .c { color: #177500 } /* Comment */
      .codehilite .err { color: #000000 } /* Error */
      .codehilite .k { color: #A90D91 } /* Keyword */
      .codehilite .l { color: #1C01CE } /* Literal */
      .codehilite .n { color: #000000 } /* Name */
      .codehilite .o { color: #000000 } /* Operator */
      .codehilite .ch { color: #177500 } /* Comment.Hashbang */
      .codehilite .cm { color: #177500 } /* Comment.Multiline */
      .codehilite .cp { color: #633820 } /* Comment.Preproc */
      .codehilite .cpf { color: #177500 } /* Comment.PreprocFile */
      .codehilite .c1 { display: none; color: #177500 } /* Comment.Single */
      .codehilite .cs { color: #177500 } /* Comment.Special */
      .codehilite .kc { color: #A90D91 } /* Keyword.Constant */
      .codehilite .kd { color: #A90D91 } /* Keyword.Declaration */
      .codehilite .kn { color: #A90D91 } /* Keyword.Namespace */
      .codehilite .kp { color: #A90D91 } /* Keyword.Pseudo */
      .codehilite .kr { color: #A90D91 } /* Keyword.Reserved */
      .codehilite .kt { color: #A90D91 } /* Keyword.Type */
      .codehilite .ld { color: #1C01CE } /* Literal.Date */
      .codehilite .m { color: #1C01CE } /* Literal.Number */
      .codehilite .s { color: #C41A16 } /* Literal.String */
      .codehilite .na { color: #836C28 } /* Name.Attribute */
      .codehilite .nb { color: #A90D91 } /* Name.Builtin */
      .codehilite .nc { color: #3F6E75 } /* Name.Class */
      .codehilite .no { color: #000000 } /* Name.Constant */
      .codehilite .nd { color: #000000 } /* Name.Decorator */
      .codehilite .ni { color: #000000 } /* Name.Entity */
      .codehilite .ne { color: #000000 } /* Name.Exception */
      .codehilite .nf { color: #000000 } /* Name.Function */
      .codehilite .nl { color: #000000 } /* Name.Label */
      .codehilite .nn { color: #000000 } /* Name.Namespace */
      .codehilite .nx { color: #000000 } /* Name.Other */
      .codehilite .py { color: #000000 } /* Name.Property */
      .codehilite .nt { color: #000000 } /* Name.Tag */
      .codehilite .nv { color: #000000 } /* Name.Variable */
      .codehilite .ow { color: #000000 } /* Operator.Word */
      .codehilite .mb { color: #1C01CE } /* Literal.Number.Bin */
      .codehilite .mf { color: #1C01CE } /* Literal.Number.Float */
      .codehilite .mh { color: #1C01CE } /* Literal.Number.Hex */
      .codehilite .mi { color: #1C01CE } /* Literal.Number.Integer */
      .codehilite .mo { color: #1C01CE } /* Literal.Number.Oct */
      .codehilite .sa { color: #C41A16 } /* Literal.String.Affix */
      .codehilite .sb { color: #C41A16 } /* Literal.String.Backtick */
      .codehilite .sc { color: #2300CE } /* Literal.String.Char */
      .codehilite .dl { color: #C41A16 } /* Literal.String.Delimiter */
      .codehilite .sd { color: #C41A16 } /* Literal.String.Doc */
      .codehilite .s2 { color: #C41A16 } /* Literal.String.Double */
      .codehilite .se { color: #C41A16 } /* Literal.String.Escape */
      .codehilite .sh { color: #C41A16 } /* Literal.String.Heredoc */
      .codehilite .si { color: #C41A16 } /* Literal.String.Interpol */
      .codehilite .sx { color: #C41A16 } /* Literal.String.Other */
      .codehilite .sr { color: #C41A16 } /* Literal.String.Regex */
      .codehilite .s1 { color: #C41A16 } /* Literal.String.Single */
      .codehilite .ss { color: #C41A16 } /* Literal.String.Symbol */
      .codehilite .bp { color: #5B269A } /* Name.Builtin.Pseudo */
      .codehilite .fm { color: #000000 } /* Name.Function.Magic */
      .codehilite .vc { color: #000000 } /* Name.Variable.Class */
      .codehilite .vg { color: #000000 } /* Name.Variable.Global */
      .codehilite .vi { color: #000000 } /* Name.Variable.Instance */
      .codehilite .vm { color: #000000 } /* Name.Variable.Magic */
      .codehilite .il { color: #1C01CE } /* Literal.Number.Integer.Long */
    </style>
  </head>

  <body>
    <div id="container" data-pan-on-drag data-zoom-on-wheel="min-scale: 0.3; max-scale: 16;">{{ svg }}</div>
    <div id="code"></div>
    <div id="mouse-circle"> </div>

    <script src="https://cdn.jsdelivr.net/npm/svg-pan-zoom-container@0.5.1"></script>
    <script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
    <script> data = {{ data }}; </script> 

    <script>
      $(document).ready(function() {
        $("title").remove();
        $("#code").html(data[data["s0"]]);
        var disp = function() {
          var id = $(this).attr("id");
          var code = data[id];
          $("#code").html(code);
        };
        $(".node").mouseover(disp);
        $(".edge").mouseover(disp);
        $(".edge").css('cursor', 'none');
        $(".node").css('cursor', 'none');
      });

      document.addEventListener('DOMContentLoaded', () => {
        let mousePosX = 0, mousePosY = 0, mouseCircle = document.getElementById('mouse-circle');
        document.onmousemove = (e) => { mousePosX = e.pageX; mousePosY = e.pageY; }
        let delay = 6, revisedMousePosX = 0, revisedMousePosY = 0;

        function delayMouseFollow() {
          requestAnimationFrame(delayMouseFollow);
          revisedMousePosX += (mousePosX - revisedMousePosX) / delay;
          revisedMousePosY += (mousePosY - revisedMousePosY) / delay; 
          mouseCircle.style.top = revisedMousePosY + 'px';
          mouseCircle.style.left = revisedMousePosX + 'px';
        }
        delayMouseFollow();
      });
    </script>
  </body>
</html>
'''

svg_file = Path(g.render(format='svg'))
html = jinja2.Template(TEMPLATE).render(svg=svg_file.read_text(), data=json.dumps(metadata))
print(html)
