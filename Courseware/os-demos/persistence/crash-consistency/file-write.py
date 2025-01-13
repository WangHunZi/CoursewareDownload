def main():
    # The initial file system layout
    sys_bwrite('inode', 'inode: #1')
    sys_bwrite('used', ['#1'])

    xb = [f'#{x+1}' for x in range(2)]
    sys_bwrite('#1', '#1 (old)')
    sys_bwrite('#2', '#2 (unused)')

    # Persist this file system layout
    sys_sync()


    # Rewrite (and append) to file:
    # 1. Write inode
    sys_bwrite('inode', f'inode: #1 #2')

    # 2. Write bitmap (allocate)
    sys_bwrite('used', xb)

    # 3. Write data blocks
    for i in range(2):
        sys_bwrite(f'#{i+1}', f'#{i+1} (new)')


    # Crash (and recovery)
    sys_crash()

    # Display the current file system state
    inode = sys_bread('inode')
    used = sys_bread('used')

    res = f'{inode:12}  |  used: {" ".join(used):6}  |  '
    for i in range(2):
        if f'#{i+1}' in inode:
            res += sys_bread(f'#{i+1}')
            res += ' '
    sys_write(res)
