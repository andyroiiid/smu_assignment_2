//
// Created by andyroiiid on 2/1/2021.
//

#include "Utils.h"

#include <fstream>
#include <sstream>
#include <cmath>

std::string readFile(const std::string &filename) {
    std::ifstream f(filename, std::ios::binary);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

ByteFrequencies::ByteFrequencies(const std::string &bytes) : array{0.0} {
    // count occurrences of bytes
    std::array<size_t, ByteTotalNum> counts{0};
    for (unsigned char b : bytes) {
        counts[b]++;
    }

    // calculate the frequencies
    const double scale = 1.0 / bytes.length();
    for (int i = 0; i < ByteTotalNum; i++) {
        at(i) = counts[i] * scale;
    }
}

void ByteFrequencies::print() const {
    double sum = 0;
    for (size_t i = 0; i < size(); ++i) {
        double frequency = at(i);
        printf("%llu\t%f\n", i, frequency);
        sum += frequency;
    }
    printf("sum\t%f\n", sum);
}


double ByteFrequencies::calcEntropy() const {
    double entropy = 0.0;
    for (auto frequency : *this) {
        if (frequency > 0) {
            entropy -= frequency * log2(frequency);
        }
    }
    return entropy;
}
