#include <fstream>

#include "Utils.h"
#include "HuffmanNode.h"

/* COMPRESSED FILE BINARY LAYOUT
 * size_t treeSize;
 * short treeNodes[treeSize];
 * size_t numBits;
 * int words[bitToWordCeil(numBits)];
 * */

size_t encode(const std::string &bytes, const HuffmanNode::Encodings &encodings, std::vector<int> &output) {
    size_t numBits = 0;

    int buffer = 0; // buffer for combining bits into words
    int i = 0; // current bit index in the buffer
    static constexpr int bufferSizeInBits = sizeof(buffer) * 8;

    for (unsigned char b : bytes) {
        const auto &encoding = encodings[b];
        numBits += encoding.size();
        for (int bit : encoding) {
            // fill the buffer with bits
            buffer |= bit << i++;

            if (i == bufferSizeInBits) {
                // when the buffer is filled with bits, append the buffer to output
                output.push_back(buffer);

                // clear the buffer
                buffer = 0;
                i = 0;
            }
        }
    }
    if (i > 0) {
        // append the remaining bits
        output.push_back(buffer);
    }

    return numBits;
}

void compress(const std::string &inputFilename, const std::string &outputFilename) {
    // Loading a file into memory
    const std::string bytes = readFile(inputFilename);

    if (bytes.empty()) {
        fprintf(stderr, "input file is empty / not exists");
        return;
    }

    // counting the frequency of occurrence of each byte
    const ByteFrequencies frequencies(bytes);

    // calculate entropy
    double entropy = frequencies.calcEntropy();
    printf("input entropy = %f bits\n", entropy);

    // estimate compression ratio and compressed size
    printf("estimated compression ratio = %f\n", 8 / entropy);
    printf("estimated data compressed size = %f bytes\n", bytes.length() * entropy / 8.0);

    // generate the Huffman tree
    auto huffmanTree = HuffmanNode::generateHuffmanTree(frequencies);

    // open output file
    std::ofstream file(outputFilename, std::ios::binary);

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
//    printf("\n");
}

void decompress(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);

    size_t treeSize;
    file.read(reinterpret_cast<char *>(&treeSize), sizeof(treeSize));
    printf("treeSize = %llu\n", treeSize);

    for (int i = 0; i < treeSize; i++) {
        short node;
        file.read(reinterpret_cast<char *>(&node), sizeof(node));
        printf("%d\n", node);
    }

    size_t numBits;
    file.read(reinterpret_cast<char *>(&numBits), sizeof(numBits));
    printf("numBits = %llu\n", numBits);

    size_t wordSize = numBits / 32 + (numBits % 32 ? 1 : 0);
    printf("wordSize = %llu\n", wordSize);

    std::vector<int> words;
    words.resize(wordSize);
    file.read(reinterpret_cast<char *>(words.data()), wordSize * sizeof(int));

    for (int word : words) {
        printf("%08X ", word);
    }
    printf("\n");
}

int main() {
    compress("test.txt", "out");

    decompress("out");

    return 0;
}
