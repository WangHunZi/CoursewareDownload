#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define KB * 1024LL
#define MB KB * 1024LL
#define GB MB * 1024LL

#define N_BLOCKS 8
#define N_THREADS_PER_BLOCK 32
#define WARP_SIZE 32
#define GLOBAL_MEM_SIZE (16 GB)
#define SHARED_MEM_SIZE (64 KB)

struct thread_state {
    bool active;

    // Constant for each thread
    uint8_t threadIdx_x;
    uint8_t threadIdx_y;
    uint8_t threadIdx_z;

    // General-purpose registers
    uint64_t regs[32];

    // Threads don't have PCs!
};

struct warp_state {
    struct thread_state threads[WARP_SIZE];

    // Threads in a "thread warp" share a
    // single program counter.
    int warp_pc;
};

struct block_state {
    struct warp_state warps[N_THREADS_PER_BLOCK / WARP_SIZE];
    uint8_t shared_memory[SHARED_MEM_SIZE];
};

struct gpu_state {
    // This is massive parallel!
    //     Blocks * Warps * Threads
    struct block_state blocks[N_BLOCKS];

    uint8_t global_memory[GLOBAL_MEM_SIZE];
};
