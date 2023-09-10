#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// Copied from the manual
// Should be placed in a header file
struct fat32hdr {
  u8  BS_jmpBoot[3];
  u8  BS_OEMName[8];
  u16 BPB_BytsPerSec;
  u8  BPB_SecPerClus;
  u16 BPB_RsvdSecCnt;
  u8  BPB_NumFATs;
  u16 BPB_RootEntCnt;
  u16 BPB_TotSec16;
  u8  BPB_Media;
  u16 BPB_FATSz16;
  u16 BPB_SecPerTrk;
  u16 BPB_NumHeads;
  u32 BPB_HiddSec;
  u32 BPB_TotSec32;
  u32 BPB_FATSz32;
  u16 BPB_ExtFlags;
  u16 BPB_FSVer;
  u32 BPB_RootClus;
  u16 BPB_FSInfo;
  u16 BPB_BkBootSec;
  u8  BPB_Reserved[12];
  u8  BS_DrvNum;
  u8  BS_Reserved1;
  u8  BS_BootSig;
  u32 BS_VolID;
  u8  BS_VolLab[11];
  u8  BS_FilSysType[8];
  u8  __padding_1[420];
  u16 Signature_word;
} __attribute__((packed));

struct fat32dent {
  u8  DIR_Name[11];
  u8  DIR_Attr;
  u8  DIR_NTRes;
  u8  DIR_CrtTimeTenth;
  u16 DIR_CrtTime;
  u16 DIR_CrtDate;
  u16 DIR_LastAccDate;
  u16 DIR_FstClusHI;
  u16 DIR_WrtTime;
  u16 DIR_WrtDate;
  u16 DIR_FstClusLO;
  u32 DIR_FileSize;
} __attribute__((packed));

#define CLUS_INVALID   0xffffff7

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN    0x02
#define ATTR_SYSTEM    0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE   0x20

struct fat32hdr *hdr;

void *map_disk(const char *fname);
void dfs(u32 clusId, int depth, int is_dir);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s fs-image\n", argv[0]);
    exit(1);
  }

  setbuf(stdout, NULL);

  assert(sizeof(struct fat32hdr) == 512); // defensive
  assert(sizeof(struct fat32dent) == 32); // defensive

  // map disk image to memory
  hdr = map_disk(argv[1]);

  // file system traversal
  dfs(hdr->BPB_RootClus, 0, 1);

  munmap(hdr, hdr->BPB_TotSec32 * hdr->BPB_BytsPerSec);
}

void *map_disk(const char *fname) {
  int fd = open(fname, O_RDWR);

  if (fd < 0) {
    goto release;
  }

  off_t size = lseek(fd, 0, SEEK_END);
  if (size == -1) {
    goto release;
  }

  struct fat32hdr *hdr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (hdr == (void *)-1) {
    goto release;
  }

  close(fd);

  assert(hdr->Signature_word == 0xaa55); // this is an MBR
  assert(hdr->BPB_TotSec32 * hdr->BPB_BytsPerSec == size);

  printf("%s: DOS/MBR boot sector, ", fname);
  printf("OEM-ID \"%s\", ", hdr->BS_OEMName);
  printf("sectors/cluster %d, ", hdr->BPB_SecPerClus);
  printf("sectors %d, ", hdr->BPB_TotSec32);
  printf("sectors %d, ", hdr->BPB_TotSec32);
  printf("sectors/FAT %d, ", hdr->BPB_FATSz32);
  printf("serial number 0x%x\n", hdr->BS_VolID);
  return hdr;

release:
  perror("map disk");
  if (fd > 0) {
    close(fd);
  }
  exit(1);
}

u32 next_cluster(int n) {
  // RTFM: Sec 4.1
  u32 *fat = (u32 *)((u8 *)hdr + hdr->BPB_RsvdSecCnt * hdr->BPB_BytsPerSec);
  return fat[n];
}

void *cluster_to_sec(int n) {
  // RTFM: Sec 3.5 and 4 (TRICKY)
  // Don't copy code. Write your own.
  u32 DataSec = hdr->BPB_RsvdSecCnt + hdr->BPB_NumFATs * hdr->BPB_FATSz32;
  DataSec += (n - 2) * hdr->BPB_SecPerClus;
  return ((char *)hdr) + DataSec * hdr->BPB_BytsPerSec;
}

void get_filename(struct fat32dent *dent, char *buf) {
  // RTFM: Sec 6.1
  int len = 0;
  for (int i = 0; i < sizeof(dent->DIR_Name); i++) {
    if (dent->DIR_Name[i] != ' ') {
      if (i == 8) buf[len++] = '.';
      buf[len++] = dent->DIR_Name[i];
    }
  }
  buf[len] = '\0';
}

void dfs(u32 clusId, int depth, int is_dir) {
  // RTFM: Sec 6
  for (; clusId < CLUS_INVALID; clusId = next_cluster(clusId)) {
    if (is_dir) {
      int ndents = hdr->BPB_BytsPerSec * hdr->BPB_SecPerClus / sizeof(struct fat32dent);

      for (int d = 0; d < ndents; d++) {
        struct fat32dent *dent = (struct fat32dent *)cluster_to_sec(clusId) + d;
        if (dent->DIR_Name[0] == 0x00 ||
            dent->DIR_Name[0] == 0xe5 ||
            dent->DIR_Attr & ATTR_HIDDEN) continue;

        char fname[32];
        get_filename(dent, fname);

        for (int i = 0; i < 4 * depth; i++) putchar(' ');
        printf("[%-12s] %6.1lf KiB    ", fname, dent->DIR_FileSize / 1024.0);

        u32 dataClus = dent->DIR_FstClusLO | (dent->DIR_FstClusHI << 16);
        if (dent->DIR_Attr & ATTR_DIRECTORY) {
          printf("\n");
          if (dent->DIR_Name[0] != '.') {
            dfs(dataClus, depth + 1, 1);
          }
        } else {
          dfs(dataClus, depth + 1, 0);
          printf("\n");
        }
      }
    } else {
      printf("#%d ", clusId);
    }
  }
}
