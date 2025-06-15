// CatRunner.h
#ifndef CATRUNNER_H
#define CATRUNNER_H

#include <cstddef>

class CatRunner {
public:
    static void play(std::size_t cycles = 3);
private:
    static void delay();
};

#endif
