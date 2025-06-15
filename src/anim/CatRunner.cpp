#include "CatRunner.h"
#include <iostream>

void CatRunner::delay() {
    for (volatile long d = 0; d < 30000000; ++d);
}
void CatRunner::play(std::size_t cycles) {
    static const char* art[2][4] = {
        {
            "  ／l、             ",
            "（ :D ７         ",
            "  l  ~ヽ           ",
            " じしf_,)ノ        "
        },
        {
            "  ／l、             ",
            "（ :O ７         ",
            "  l   ~ヽ         ",
            " じしf_,)ノ        "
        }
    };
    const std::size_t lines = 4;
    const std::size_t trackLen = 40;
    for (std::size_t c = 0; c < cycles; ++c) {
        for (std::size_t pos = 0; pos <= trackLen; ++pos) {
            for (std::size_t f = 0; f < lines; ++f) {
                std::cout << "\r";
                for (std::size_t s = 0; s < pos; ++s) std::cout << ' ';
                std::cout << art[(pos + c) % 2][f] << '\n';
            }
            std::cout << std::flush;
            delay();
            std::cout << "\033[" << lines << "A";
        }
    }
    std::cout << "\033[" << lines << "BWelcome to ChatSystem!\n";
}
