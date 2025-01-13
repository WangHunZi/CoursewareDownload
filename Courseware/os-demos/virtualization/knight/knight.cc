#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
using std::string, std::to_string;

struct Game {
    string name; // Name of the traced process
    int pid;     // Pid of the traced process
    int fd;      // Memory file of the traced process
    
    std::vector<uintptr_t> remain; // Watched addresses

public:
    Game(string proc_name):
        name(proc_name),
        pid(stoi(run("pidof " + proc_name))) {

        // See: proc(5)
        string memfile = "/proc/" + to_string(pid) + "/mem";

        // We need root permission to open this file;
        // otherwise it would be too dangerous.
        fd = open(memfile.c_str(), O_RDWR);
        if (fd < 0) {
            perror(memfile.c_str());
            exit(1);
        }
    }

    ~Game() {
        close(fd);
    }

    void search_for(uint32_t val) {
        if (remain.size() == 0) {
            // No match. Start a new round of search.

            string maps = run("pmap -x " + to_string(pid));

            std::regex r(
                R"(^([0-9a-f]+)\s+(\d+)\s+(\d+)\s+(\d+)\s+rw.*)"
            );

            std::istringstream iss(maps);
            for (string line; std::getline(iss, line); ) {
                std::smatch match;
                if (std::regex_search(line, match, r)) {
                    uintptr_t start = stoll(match[1].str(), nullptr, 16);
                    uintptr_t size = stoll(match[2], nullptr, 10) * 1024;
                    printf("Scanning %lx--%lx\n", start, start + size);

                    // Copy process memory to local
                    std::unique_ptr<uint32_t[]> mem(new uint32_t [size / 4]);
                    lseek(fd, start, SEEK_SET);
                    size = read(fd, mem.get(), size);

                    for (uintptr_t off = 0; off < size; off += 4) {
                        if (mem[off / 4] == val) {
                            // Found a match!
                            remain.push_back(start + off);
                        }
                    }
                }
            }
        } else {
            // Search in the watched values.

            std::erase_if(remain, [this, val](uintptr_t addr) {
                return load(addr) != val;
            });
        }
        printf("There are %ld match(es).\n", remain.size());
    }

    void reset() {
        remain.clear();
    }

    void overwrite(uint32_t val) {
        int nwrite = 0;
        for (uintptr_t addr : remain) {
            store(addr, val);
            nwrite++;
        }
        printf("%d value(s) written.\n", nwrite);
    }

private:
    uint32_t load(uintptr_t addr) {
        // Load 32-bit value from another address space
        uint32_t val;
        lseek(fd, addr, SEEK_SET);
        read(fd, &val, sizeof(val));
        return val;
    }

    void store(uintptr_t addr, uint32_t val) {
        // Store 32-bit value to another address space
        lseek(fd, addr, SEEK_SET);
        write(fd, &val, sizeof(val));
    }

    static string run(const string &cmd) {
        std::array<char,128> buf;
        string result;

        FILE *pipe = popen(cmd.c_str(), "r");
        if (!pipe) {
            perror(cmd.c_str());
            exit(1);
        }

        while (fgets(buf.data(), buf.size(), pipe) != nullptr) {
            result += buf.data();
        }

        pclose(pipe);
        return result;
    }
};


int main(int argc, char *argv[]) {
    Game g(argv[1]);
    uint32_t val;
    char buf[64];

    printf(
        "Usage:\n"
        "  - s 100: search for value\n"
        "  - w 99999: overwrite value (for search matches)\n"
        "  - r: reset search\n\n"
    );

    while (!feof(stdin)) {
        printf("(%s %d) ", g.name.c_str(), g.pid);
        scanf("%s", buf);

        switch (buf[0]) {
            case 'q': return 0;
            case 's': scanf("%d", &val); g.search_for(val); break;
            case 'w': scanf("%d", &val); g.overwrite(val); break;
            case 'r': g.reset(); break;
        }
    }
}
