//
// Created by andyroiiid on 2/1/2021.
//

#ifndef SMU_ASSIGNMENT_2_HUFFMANNODE_H
#define SMU_ASSIGNMENT_2_HUFFMANNODE_H

#include <memory>
#include <vector>

#include "Utils.h"

class HuffmanNode {
public:
    using UniquePtr = std::unique_ptr<HuffmanNode>;
    using Encodings = std::array<std::vector<int>, ByteTotalNum>;

    HuffmanNode(int byte, double frequency)
            : byte(byte),
              frequency(frequency) {}

    HuffmanNode(UniquePtr left, UniquePtr right)
            : byte(-1),
              frequency(left->frequency + right->frequency),
              left(std::move(left)),
              right(std::move(right)) {}

    static UniquePtr generateHuffmanTree(const ByteFrequencies &frequencies);

    void generateEncodings(Encodings &encodings) {
        std::vector<int> pathStack;
        generateEncodings(pathStack, encodings);
    }

    void serialize(std::vector<short> &serialized) {
        if (byte == -1) {
            left->serialize(serialized);
            right->serialize(serialized);
            serialized.push_back(-1);
        } else {
            serialized.push_back(byte);
        }
    }

private:
    void generateEncodings(std::vector<int> &pathStack, Encodings &encodings) const;

    int byte = 0;
    double frequency = 0.0;

    UniquePtr left;
    UniquePtr right;
};

#endif //SMU_ASSIGNMENT_2_HUFFMANNODE_H
