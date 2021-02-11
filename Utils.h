//
// Created by andyroiiid on 2/1/2021.
//

#ifndef SMU_ASSIGNMENT_2_UTILS_H
#define SMU_ASSIGNMENT_2_UTILS_H

#include <array>
#include <string>

/*
 * read binary file into std::string
 */
std::string readFile(const std::string &filename);

static constexpr size_t ByteTotalNum = std::numeric_limits<unsigned char>::max() + 1;

/*
 * a helper class to calculate byte frequencies
 */
class ByteFrequencies : public std::array<double, ByteTotalNum> {
public:
    explicit ByteFrequencies(const std::string &bytes);

    /*
     * print out the frequencies (for debug)
     */
    void print() const;

    /*
     * calculate entropy
     * H = -sum(p * log2(p))
     */
    [[nodiscard]] double calcEntropy() const;
};

#endif //SMU_ASSIGNMENT_2_UTILS_H
