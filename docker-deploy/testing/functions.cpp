#include "functions.h"

void convertStringToFile(string fileName, string s) {
    fstream out(fileName.c_str(), ios::out);
    if (out.is_open()) {
        out << s;
    }
    out.close();
}

int getRandomINT(int min, int max) {
    struct timespec tp;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
    srand(tp.tv_nsec);
    // unsigned seed = time(0);
    // srand(seed);

    return rand() % (max - min + 1) + min;
}