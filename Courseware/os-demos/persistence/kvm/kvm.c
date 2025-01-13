#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define GUEST_CODE_SIZE 0x1000
#define GUEST_MEM_SIZE 0x1000

int main() {
    int kvm, vmfd, vcpufd;
    struct kvm_run *run;
    struct kvm_sregs sregs;
    struct kvm_regs regs;
    size_t mmap_size;

    // Open the KVM device
    kvm = open("/dev/kvm", O_RDWR | O_CLOEXEC);
    if (kvm < 0) {
        perror("open /dev/kvm");
        exit(1);
    }

    // Create a VM
    vmfd = ioctl(kvm, KVM_CREATE_VM, 0);
    if (vmfd < 0) {
        perror("ioctl KVM_CREATE_VM");
        exit(1);
    }

    // Allocate guest memory
    void *guest_mem = mmap(NULL, GUEST_MEM_SIZE, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (guest_mem == MAP_FAILED) {
        perror("mmap guest memory");
        exit(1);
    }

    // Map the guest memory into the VM's address space
    struct kvm_userspace_memory_region mem_region = {
        .slot = 0,
        .guest_phys_addr = 0x1000,
        .memory_size = GUEST_MEM_SIZE,
        .userspace_addr = (uint64_t)guest_mem,
    };
    if (ioctl(vmfd, KVM_SET_USER_MEMORY_REGION, &mem_region) < 0) {
        perror("ioctl KVM_SET_USER_MEMORY_REGION");
        exit(1);
    }

    // Create a vCPU
    vcpufd = ioctl(vmfd, KVM_CREATE_VCPU, 0);
    if (vcpufd < 0) {
        perror("ioctl KVM_CREATE_VCPU");
        exit(1);
    }

    // Map the vCPU's run structure
    mmap_size = ioctl(kvm, KVM_GET_VCPU_MMAP_SIZE, 0);
    if (mmap_size < 0) {
        perror("ioctl KVM_GET_VCPU_MMAP_SIZE");
        exit(1);
    }
    run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpufd, 0);
    if (run == MAP_FAILED) {
        perror("mmap vcpu");
        exit(1);
    }

    // Set up the guest code
    unsigned char guest_code[] = {
        0xba, 0xf8, 0x03,  // mov $0x3f8, %dx
        0x00, 0xd8,        // add %bl, %al
        0x04, '0',         // add $'0', %al
        0xee,              // out %al, (%dx)
        0xf4,              // hlt
    };
    memcpy(guest_mem, guest_code, sizeof(guest_code));

    // Set the vCPU's special registers
    if (ioctl(vcpufd, KVM_GET_SREGS, &sregs) < 0) {
        perror("ioctl KVM_GET_SREGS");
        exit(1);
    }
    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    if (ioctl(vcpufd, KVM_SET_SREGS, &sregs) < 0) {
        perror("ioctl KVM_SET_SREGS");
        exit(1);
    }

    // Set the vCPU's general purpose registers
    regs.rip = 0;
    regs.rax = 2;
    regs.rbx = 2;
    regs.rflags = 0x2;
    if (ioctl(vcpufd, KVM_SET_REGS, &regs) < 0) {
        perror("ioctl KVM_SET_REGS");
        exit(1);
    }

    // Run the vCPU
    while (1) {
        if (ioctl(vcpufd, KVM_RUN, 0) < 0) {
            perror("ioctl KVM_RUN");
            exit(1);
        }

        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("KVM_EXIT_HLT\n");
                goto done;
            case KVM_EXIT_IO:
                if (run->io.direction == KVM_EXIT_IO_OUT &&
                    run->io.size == 1 &&
                    run->io.port == 0x3f8 &&
                    run->io.count == 1) {
                    putchar(*(((char *)run) + run->io.data_offset));
                }
                break;
            default:
                fprintf(stderr, "Unhandled KVM exit reason: %d\n", run->exit_reason);
                exit(1);
        }
    }

done:
    munmap(run, mmap_size);
    close(vcpufd);
    close(vmfd);
    close(kvm);
    return 0;
}
