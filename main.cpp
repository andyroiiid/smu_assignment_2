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

    int word = 0; // buffer for combining bits into words
    int i = 0; // current bit index in the buffer
    static constexpr int wordSizeInBits = sizeof(word) * 8;

    for (unsigned char b : bytes) {
        const auto &encoding = encodings[b];
        numBits += encoding.size();
        for (int bit : encoding) {
            // fill the buffer with bits
            word |= bit << i++;

            if (i == wordSizeInBits) {
                // when the buffer is filled with bits, append the buffer to output
                output.push_back(word);

                // clear the buffer
                word = 0;
                i = 0;
            }
        }
    }
    if (i > 0) {
        // append the remaining bits
        output.push_back(word);
    }

    return numBits;
}

void compress(const std::string &inputFilename, const std::string &outputFilename) {
    // loading a file into memory
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

    if (entropy == 0) {
        fprintf(stderr, "input file has a zero entropy (all bytes are the same)\n");
        return;
    }

    // estimate compression ratio and compressed size
    printf("estimated compression ratio = %f\n", 8 / entropy);
    printf("estimated data compressed size = %f bytes\n", bytes.length() * entropy / 8.0);

    // generate the Huffman tree
    auto huffmanTree = HuffmanNode::generateHuffmanTree(frequencies);
//    huffmanTree->printTree();

    // open output file
    std::ofstream file(outputFilename, std::ios::binary);

    // get tree serialization
    std::vector<short> serialized;
    huffmanTree->serialize(serialized);

    // write serialized tree to file
    size_t treeSize = serialized.size();
    file.write(reinterpret_cast<const char *>(&treeSize), sizeof(treeSize));
    file.write(reinterpret_cast<const char *>(serialized.data()), serialized.size() * sizeof(short));

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
}

void decompress(const std::string &inputFilename, const std::string &outputFilename) {
    // open compressed inputFile
    std::ifstream inputFile(inputFilename, std::ios::binary);

    // load the Huffman tree from inputFile
    auto huffmanTree = HuffmanNode::deserializeFromFile(inputFile);
    if (huffmanTree == nullptr) {
        fprintf(stderr, "failed to deserialize huffman tree\n");
        return;
    }
//    huffmanTree->printTree();

    // read the compressed bits number
    size_t numBits;
    inputFile.read(reinterpret_cast<char *>(&numBits), sizeof(numBits));
    if (!inputFile) {
        fprintf(stderr, "failed to read compressed data size");
        return;
    }

    // calculate the compressed data size
    size_t wordSize = numBits / 32 + (numBits % 32 ? 1 : 0);

    // stream decompress the compressed data
    std::ofstream outputFile(outputFilename, std::ios::binary);
    const HuffmanNode *currentNode = huffmanTree.get();
    size_t iBit = 1;
    size_t bytesDecompressed = 0;
    for (size_t iWord = 0; iWord < wordSize; iWord++) {
        // read a compressed word
        int word;
        inputFile.read(reinterpret_cast<char *>(&word), sizeof(word));
        if (!inputFile) {
            fprintf(stderr, "compressed data is incomplete");
            return;
        }

        for (int i = 0; i < sizeof(word) * 8; i++) {
            // walk the Huffman tree
            int bit = word & (1 << i);
            currentNode = bit ? currentNode->getRight() : currentNode->getLeft();
            int byte = currentNode->getByte();

            if (byte >= 0) {
                // leaf node -> output
                outputFile.write(reinterpret_cast<const char *>(&byte), 1);
                currentNode = huffmanTree.get();
                bytesDecompressed++;
            }

            if (++iBit > numBits) {
                // finished decompressing all bits
                printf("%llu bytes decompressed\n", bytesDecompressed);
                return;
            }
        }
    }
}

void printHelp() {
    printf("usage:\n"
           "compress:\t<executable name> c <input file> <output file>\n"
           "decompress:\t<executable name> d <input file> <output file>\n");
}

int main(int argc, const char *argv[]) {
    if (argc == 4) {
        if (strcmp(argv[1], "c") == 0) {
            printf("compress %s to %s\n", argv[2], argv[3]);
            compress(argv[2], argv[3]);
            return 0;
        } else if (strcmp(argv[1], "d") == 0) {
            printf("decompress %s to %s\n", argv[2], argv[3]);
            decompress(argv[2], argv[3]);
            return 0;
        }
    }

    printHelp();
    return -1;
}
