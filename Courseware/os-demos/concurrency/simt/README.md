**SIMT 模型**：在这个不能执行的模型中，我们模仿 NVIDIA GPU “定义” 了一个 SIMT 处理器，其中最显著的特性是并没有 per-thread 的 program counter，而是一个线程束共享一个 program counter，同步执行。线程束里的线程也可以理解成是一种 “可编程” 的 SIMD。
