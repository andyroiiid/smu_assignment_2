//
// Created by andyroiiid on 2/1/2021.
//

#ifndef SMU_ASSIGNMENT_2_UTILS_H
#define SMU_ASSIGNMENT_2_UTILS_H

#include <array>
#include <string>

std::string readFile(const std::string &filename);

static constexpr size_t ByteTotalNum = std::numeric_limits<unsigned char>::max() + 1;

class ByteFrequencies : public std::array<double, ByteTotalNum> {
public:
    explicit ByteFrequencies(const std::string &bytes);

    void print() const;

    [[nodiscard]] double calcEntropy() const;
};

#endif //SMU_ASSIGNMENT_2_UTILS_H
