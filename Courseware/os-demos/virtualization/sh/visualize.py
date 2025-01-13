import gdb
import subprocess
import re

class ProcDump(gdb.Command):
    def __init__(self):
        super(ProcDump, self).__init__(
            "pdump", gdb.COMMAND_DATA, gdb.COMPLETE_SYMBOL
        )

    def invoke(self, *_):
        print()

        for proc in gdb.inferiors():
            pid = proc.pid
            if int(pid) == 0:
                continue

            print(f'Process {proc.num} ({pid})', end='')
            if proc is gdb.selected_inferior():
                print('*')
            else:
                print()

            for fd_desc in subprocess.check_output(
                ['ls', '-l', f'/proc/{pid}/fd'], encoding='utf-8'
            ).splitlines()[1:]:
                perm, *_, fd, _, fname = fd_desc.split()

                if int(fd) < 10:
                    if 'rw' in perm: rw = '<->'
                    elif 'r' in perm: rw = '<--'
                    elif 'w' in perm: rw = '-->'
                    if 'pipe:' in fname:
                        pipe_id = re.search(f'[0-9]+', fname).group()
                        print(f'    {fd} {rw} [=== {pipe_id} ===]')
                    else:
                        print(f'    {fd} {rw} {fname}')

            print()

ProcDump()
