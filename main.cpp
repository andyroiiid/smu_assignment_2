#include <fstream>
#include <cmath>

#include "Utils.h"
#include "HuffmanNode.h"

size_t encode(const std::string &bytes, const HuffmanNode::Encodings &encodings, std::vector<int> &output) {
    size_t numBits = 0;
    int i = 0;
    int word = 0;
    static constexpr int wordSize = sizeof(word) * 8;
    for (unsigned char b : bytes) {
        const auto &encoding = encodings[b];
        numBits += encoding.size();
        for (int bit : encoding) {
            word |= bit << i++;
            if (i == wordSize) {
                output.push_back(word);
                i = 0;
                word = 0;
            }
        }
    }
    output.push_back(word);
    return numBits;
}

int main() {
    // Loading a file into memory
    const std::string bytes = readFile("D:\\aseprite\\aseprite.exe");

    if (bytes.empty()) {
        fprintf(stderr, "empty input file");
        return -1;
    }

    // counting the frequency of occurrence of each byte
    const ByteFrequencies frequencies(bytes);
    // frequencies.print();

    // calculate entropy
    // H = -sum(p * log2(p))
    double entropy = 0.0;
    for (auto frequency : frequencies) {
        entropy -= frequency * log2(frequency);
    }
    printf("input entropy = %f bits\n", entropy);

    // estimate compression ratio and compressed size
    printf("estimated compression ratio = %f\n", 8 / entropy);
    printf("estimated data compressed size = %f bytes\n", bytes.length() * entropy / 8.0);

    // generate the Huffman tree
    auto huffmanTree = HuffmanNode::generateHuffmanTree(frequencies);

    // open output file
    std::ofstream file("out", std::ios::binary);

    // get tree serialization
    std::vector<short> serialized;
    huffmanTree->serialize(serialized);

    // write serialized tree to file
    size_t treeSize = serialized.size();
    file.write(reinterpret_cast<const char *>(&treeSize), sizeof(treeSize));
    file.write(reinterpret_cast<const char *>(serialized.data()), serialized.size() * sizeof(short));

//    printf("%llu\n", treeSize);
//    for (int node : serialized) {
//        printf("%d\n", node);
//    }

    // generate encodings from the tree
    HuffmanNode::Encodings encodings;
    huffmanTree->generateEncodings(encodings);

    // get encoded words
    std::vector<int> words;
    size_t numBits = encode(bytes, encodings, words);

    // write encoded words into file
    file.write(reinterpret_cast<const char *>(&numBits), sizeof(numBits));
    file.write(reinterpret_cast<const char *>(words.data()), words.size() * sizeof(int));

    printf("final compressed size (data section only) = %llu bits = %f bytes\n", numBits, numBits / 8.0);

//    printf("%llu\n", numBits);
//    for (int word : words) {
//        printf("%08X ", word);
//    }
//    putchar('\n');

    return 0;
}
