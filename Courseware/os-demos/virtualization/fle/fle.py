#!/usr/bin/env python3

# The binary utilities for FLE (Funny Little Executable)
# 
# - cc: compiles a C file (freestanding, nostdlib) to FLE object file
# - ld: link FLE object files as an FLE executable
# - objdump: display object file information
# - readfle: display FLE file information
# - nm: display symbols (for executable FLEs only)

import ctypes
import json
import mmap
import re
import subprocess
import sys
from collections import defaultdict, namedtuple
from pathlib import Path
from pprint import pprint

CURRENT, RELOC, GLOBL, LOCAL, BYTES = '📍', '❓', '📤', '🏷️', '🔢'


def load_fle(file):
    content = Path(file).read_text()
    if content.startswith('#!'):
        content = '\n'.join(
            content.splitlines()[1:]
        ).strip()

    return json.loads(content)


def FLE_objdump(files):
    '''
    Display information from object files.
    (FLE files are self-explanatory.)
    '''
    for file in files:
        pprint(load_fle(file))


def FLE_readfle(files):
    '''
    Display information about FLE files.
    (FLE files are self-explanatory.)
    '''
    for file in files:
        pprint(load_fle(file))


def FLE_nm(files):
    '''
    Display symbols in an executable FLE file.
    '''
    for file in files:
        fle = load_fle(file)
        assert fle['type'] == '.exe'
        pprint(fle['symbols'])


def FLE_exec(files):
    '''
    Exectue an FLE file. The FLE must have:

        type  : ".exe"
        entry : the offset of FLE entry point
        .load : bytes to be loaded
    '''

    # Must be an "executable"
    fle = load_fle(files[0])
    assert fle['type'] == ".exe"

    # Collect all bytes in the ".load"
    bs = bytes()
    for line in fle['.load']:
        if line.startswith(f'{BYTES}:'):
            bs += bytes([
                int(b, 16) for b in 
                line.removeprefix(f'{BYTES}:').split()
            ])
    
    # Write the bytes to memory with RWX permission
    # In practice, we assign segments with different permissions.
    mem = mmap.mmap(
        fileno=-1, length=len(bs),
        prot=mmap.PROT_READ | mmap.PROT_WRITE | mmap.PROT_EXEC,
        flags=mmap.MAP_PRIVATE | mmap.MAP_ANONYMOUS,
    )

    mem.write(bs)
    mem.flush()

    # Jump to (mem + _start).
    func_type = ctypes.CFUNCTYPE(ctypes.c_int)
    func_ptr = ctypes.c_void_p(
        ctypes.addressof(
            ctypes.c_char.from_buffer(mem)
        ) + fle['symbols']['_start']
    )

    func_type(func_ptr.value)()  # Not expect a return
    exit(0)


def FLE_ld(args):
    '''
    Link FLE files as a single executable.
    Resolves all relocations.
    '''
    sections, fles = defaultdict(list), {}

    for i, fle in enumerate(args):
        if fle == '-o': continue
        if i > 0 and args[i - 1] == '-o':
            dest = Path(args[i])
        else:
            fle_content = json.loads(Path(fle).read_text())
            assert fle_content['type'] == '.obj'

            fles[fle] = fle_content
            for k, v in fle_content.items():
                if k.startswith('.'):
                    sections[k] += [(fle, v)]

    assert dest

    # This is a hack. 
    symbols = dict(
        i32=lambda x: x.to_bytes(4, 'little', signed=True),
    )

    for iter in range(2):
        # The first pass resolves all symbols (local and global).
        # The second pass evaluates all relocations.
        res = b''

        for sec_name, sec in sections.items():
            for fle, sec_body in sec:
                symbols[f'{fle}.{sec_name.replace(".", "_")}'] = len(res)

                for item in sec_body:
                    match item.split(': '):
                        case '📤', symb:
                            # Add global symbol to the table
                            if iter == 0 and symb in symbols:
                                raise Exception(f'Multiple definition of {symb}')
                            symbols[symb] = len(res)
                        case '🏷️', symb:
                            # Add local symbol like "xxx.fle._text" to the table
                            symbols[f'{fle}.{symb}'] = len(res)
                        case '🔢', xs:
                            # Collect bytes
                            res += bytes.fromhex(xs)
                        case '❓', expr:
                            # Resolve and collect a symbol, like:
                            #     "i32(print - 0x4 - ⬅)"

                            class Symbols(dict):
                                def __getitem__(self, key):
                                    if iter == 0:
                                        return symbols.get(key, 0)
                                    else:
                                        if (key_l := f'{fle}.{key}') in symbols:
                                            return symbols[key_l]
                                        elif key in symbols:
                                            return symbols[key]
                                        else:
                                            raise Exception(f'Undefined symbol: {key}')

                            reloc = eval(
                                expr.replace(CURRENT, f'({len(res)})'),
                                {}, Symbols(),
                            )
                            res += reloc
    
    symbols_global = {
        k: v for k, v in symbols.items()
            if type(v) == int and '.' not in k
    }

    parts = [res[i:i+16] for i in range(0, len(res), 16)]

    # Write the bytes as a Shebang (#!) followed by JSON contents.
    dest.write_text(
        '#!./exec\n\n' +
        json.dumps({
            'type': '.exe',
            'symbols': symbols_global,
            '.load': [
                f'{BYTES}: ' + ' '.join([f'{b:02x}' for b in part])
                for part in parts
            ]
        },
        ensure_ascii=False, indent=4))
    dest.chmod(0o755)


def elf_to_fle(binary, section):
    '''
    Convert an ELF binary's section to FLE JSON format.
    Dirty code (needs revisions).
    '''

    section_data = subprocess.check_output(
        ['objcopy', '--dump-section', f'{section}=/dev/stdout', binary],
        stderr=subprocess.DEVNULL,
    )

    relocs = subprocess.check_output(
        ['readelf', '-r', binary],
        stderr=subprocess.DEVNULL, text=True,
    )

    names = subprocess.check_output(
        ['objdump', '-t', binary],
        stderr=subprocess.DEVNULL, text=True,
    )

    Symbol = namedtuple('Symbol', 'symb_type section offset name')

    symbols = []
    for line in names.splitlines():
        pattern = r'^([0-9a-fA-F]+)\s+(l|g)\s+(\w+)?\s+([.a-zA-Z0-9_]+)\s+([0-9a-fA-F]+)\s+(.*)$'
        if match := re.match(pattern, line):
            offset, symb_type, _, sec, _, name = match.groups()
            symbols.append(
                Symbol(symb_type, sec, int(offset, 16), name.replace('.', '_'))
            )

    relocations, enabled = {}, True
    for line in relocs.splitlines():
        if 'Relocation section' in line:
            if ('.rela' + section) in line:
                enabled = True
            else:
                enabled = False
        elif enabled:
            pattern = r'^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+(\S+)\s+([0-9a-fA-F]+)\s+(.*)$'
            if match := re.match(pattern, line):
                offset, _, reloc, *_, expr = match.groups()

                rs = expr.split()
                expr = ' '.join([rs[0]] + [re.sub(r'([0-9a-fA-F]+)', lambda m: '0x' + m.group(1), r) for r in rs[1:]])
                expr += f' - {CURRENT}'

                if reloc not in ['R_X86_64_PC32', 'R_X86_64_PLT32', 'R_X86_64_32']:
                    raise Exception(f'Unsupported relocation {reloc}')

                skip, expr = 4, f'i32({expr})'
                relocations[int(offset, 16)] = (skip, expr.replace('.', '_'))

    res, skip, holding = [], 0, []

    def do_dump(holding: list):
        if holding:
            res.append(
                f'{BYTES}: ' +
                ' '.join([f'{x:02x}' for x in holding])
            )
            holding.clear()
    
    dump = lambda holding=holding: do_dump(holding)

    for i, b in enumerate(section_data):
        for sym in symbols:
            if sym.section == section and sym.offset == i:
                dump()
                if sym.symb_type == 'l':
                    res.append(f'{LOCAL}: {sym.name}')
                else:
                    res.append(f'{GLOBL}: {sym.name}')
        if i in relocations:
            skip, reloc = relocations[i]
            dump()
            res.append(f'{RELOC}: {reloc}')
        if skip > 0:
            skip -= 1
        else:
            holding.append(b)
            if len(holding) == 16:
                dump()
    dump()

    return res


def FLE_cc(options):
    '''
    Parse an ELF .o file and convert it to FLE. Calls gcc for compilation.
    '''

    CFLAGS = ['-static', '-fPIE', '-nostdlib', '-ffreestanding', '-fno-asynchronous-unwind-tables']
    gcc_cmd = ['gcc', '-c', *CFLAGS] + options
    binary = gcc_cmd[gcc_cmd.index('-o') + 1]

    subprocess.run(gcc_cmd, check=True)

    lines = list(subprocess.check_output(
        ['objdump', '-h', binary],
        stderr=subprocess.DEVNULL, text=True,
    ).splitlines())

    res = {}

    for i, line in enumerate(lines):
        pattern = r'^\s*([0-9]+)\s+(\.(\w|\.)+)\s+([0-9a-fA-F]+)\s+.*$'
        if match := re.match(pattern, line):
            section = match.group(2)
            flags = [x.strip() for x in lines[i+1].split(',')]
            if 'ALLOC' in flags and 'note.gnu.property' not in section:
                res[section] = elf_to_fle(binary, section)

    Path(binary).with_suffix('.fle').write_text(json.dumps({
        'type': '.obj',
    } | res, ensure_ascii=False, indent=4))


def main():
    tool = 'FLE_' + Path(sys.argv[0]).name
    if tool not in globals():
        print(f'{tool} is not implemented.')
        exit(1)

    globals()[tool](sys.argv[1:])


if __name__ == '__main__':
    main()
