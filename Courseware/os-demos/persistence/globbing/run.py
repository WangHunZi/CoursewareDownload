from pathlib import Path

for f in Path('/proc').glob('*/status'):
    print(
        f.parts[-2],
        (f.parent / 'cmdline').read_text()
            or '[kernel]'
    )
