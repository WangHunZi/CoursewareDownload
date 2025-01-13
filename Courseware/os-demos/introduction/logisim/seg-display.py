import fileinput

TEMPLATE = '''
     AAAAAAAAA
    FF       BB
    FF       BB
    FF       BB
    FF       BB
     GGGGGGGG
   EE       CC
   EE       CC
   EE       CC
   EE       CC
    DDDDDDDDD
''' 

# These are ANSI Escape Codes
CLEAR = '\033[2J\033[1;1f'  # Clear screen and move cursor to top-left
WHITE = '\033[37m░\033[0m'  # A white block
BLACK = '\033[31m█\033[0m'  # A black block

for line in fileinput.input():
    # Execute the input line (like "A=0; B=1; ...") as Python code; the
    # variables A, B, ... will be stored in ctx.
    exec(line, (ctx := {}))

    # Initialize the display with a clear screen and the template.
    disp = CLEAR + TEMPLATE

    for ch in 'ABCDEFG':
        # Determine the block color.
        block = {
            0: WHITE,
            1: BLACK,
        }.get(ctx.get(ch, 0), '?')

        # Replace each character in the template with its block.
        disp = disp.replace(ch, block)

    print(disp)
