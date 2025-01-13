# Kill process (QEMU) on gdb exits
define hook-quit
    kill
end

define hook-stop
    printf "Program Counter:\n"
    x/i $rip + ($cs * 16)
    printf "------- Memory around 0x7c00 -------\n"
    x/16b 0x7c00
end

# Connect to remote
target remote localhost:1234
watch *0x7c00
break *0x7c00
layout asm
continue
