#include <sstream>
#include <fstream>

#include "Utils.h"
#include "HuffmanNode.h"

std::string encode(const std::string &bytes, const HuffmanNode::Encodings &encodings) {
    std::stringstream ss;
    for (unsigned char b : bytes) {
        const auto &encoding = encodings[b];
        for (int bit : encoding) {
            ss << bit;
        }
    }
    return ss.str();
}

int main() {
    // Loading a file into memory
    const std::string bytes = readFile("smu_assignment_2.exe");

    if (bytes.empty()) {
        fprintf(stderr, "empty input file");
        return -1;
    }

    // counting the frequency of occurrence of each byte
    const ByteFrequencies frequencies(bytes);
    frequencies.print();

    // generate the Huffman tree
    auto huffmanTree = HuffmanNode::generateHuffmanTree(frequencies);

    // generate encodings from the tree
    HuffmanNode::Encodings encodings;
    huffmanTree->generateEncodings(encodings);

    auto s = encode(bytes, encodings);
    printf("%s\n", s.c_str());

//    for (int i = 0; i < frequencies.size(); i++) {
//        printf("%d\t", i);
//        for (int bit : encodings[i]) {
//            printf("%d", bit);
//        }
//        putchar('\n');
//    }

    size_t size = 0;
    for (unsigned char byte :bytes) {
        size += encodings[byte].size();
    }

    printf("%llu %llu\n", size, s.length());

    return 0;
}
