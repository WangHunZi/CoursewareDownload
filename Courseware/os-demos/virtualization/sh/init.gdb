set follow-fork-mode child
set detach-on-fork off
set follow-exec-mode same
set confirm off
set pagination off
tui disable

skip function strlen
skip function strcpy
skip function strchr
skip function print
skip function zalloc
skip function peek
skip function gettoken

source visualize.py

break main
break runcmd

run
n
define hook-stop
    pdump
end
