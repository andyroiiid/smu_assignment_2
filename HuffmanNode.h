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

    [[nodiscard]] int getByte() const { return byte; }

    [[nodiscard]] const HuffmanNode *getLeft() const { return left.get(); }

    [[nodiscard]] const HuffmanNode *getRight() const { return right.get(); }

    static UniquePtr generateHuffmanTree(const ByteFrequencies &frequencies);

    // recursively generate the encodings
    void generateEncodings(Encodings &encodings) {
        std::vector<int> pathStack;
        generateEncodings(pathStack, encodings);
    }

    // recursively print the tree
    void printTree(int indent = 0) const;

    // serialize the tree into a buffer
    void serialize(std::vector<short> &serialized) const;

    // load the tree from a file
    static UniquePtr deserializeFromFile(std::ifstream &file);

private:
    // helper function for printing the node with indent
    void printWithIndent(int indent) const;

    // the actual recursive function
    void generateEncodings(std::vector<int> &pathStack, Encodings &encodings) const;

    int byte = 0; // >= 0 for leaf nodes
    double frequency = 0.0;

    UniquePtr left;
    UniquePtr right;
};

#endif //SMU_ASSIGNMENT_2_HUFFMANNODE_H
