**mmap**: Linux 系统使用 anonymous 的 mmap 来实现大段连续内存的分配——甚至在系统调用返回的瞬间，进程可以没有得到任何实际的内存，而是只要在首次访问时 (触发缺页异常) 分配即可。AddressSanitizer 就用 mmap 分配了 shadow memory，我们可以使用 strace 观察到这一点。
