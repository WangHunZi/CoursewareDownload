**理解 exit**: 除了 libc 为我们提供的 `exit` 函数之外，Linux 提供了两个系统调用：`exit` 和 `exit_group`，它们可以在 syscalls(2) 的手册中看到。同时，你也可以在这份手册中看到 Linux 肩负的 “历史包袱”。strace 可以查看应用程序是如何 “退出” 的。
