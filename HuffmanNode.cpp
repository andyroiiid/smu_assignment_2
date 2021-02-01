//
// Created by andyroiiid on 2/1/2021.
//

#include "HuffmanNode.h"

#include <algorithm>
#include <fstream>

HuffmanNode::UniquePtr HuffmanNode::generateHuffmanTree(const ByteFrequencies &frequencies) {
    // comparator for std::make_heap, std::push_heap and std::pop_heap
    static auto compare = [](const HuffmanNode::UniquePtr &lhs, const HuffmanNode::UniquePtr &rhs) {
        return lhs->frequency > rhs->frequency;
    };

    // create leaf nodes for each byte that has a frequency > 0
    std::vector<HuffmanNode::UniquePtr> nodes;
    nodes.reserve(ByteTotalNum);
    for (int i = 0; i < ByteTotalNum; ++i) {
        const double &frequency = frequencies[i];
        if (frequency <= 0)
            continue;
        nodes.push_back(std::make_unique<HuffmanNode>(i, frequency));
    }

    std::make_heap(nodes.begin(), nodes.end(), compare);
    while (nodes.size() > 1) {
        // extract the node with the lowest frequency
        std::pop_heap(nodes.begin(), nodes.end(), compare);
        auto min1 = std::move(nodes.back());
        nodes.pop_back();

        // extract the node with the second lowest frequency
        std::pop_heap(nodes.begin(), nodes.end(), compare);
        auto min2 = std::move(nodes.back());
        nodes.pop_back();

        // combine
        auto combined = std::make_unique<HuffmanNode>(std::move(min1), std::move(min2));
        nodes.push_back(std::move(combined));
        std::push_heap(nodes.begin(), nodes.end(), compare);
    }

    // move the tree out of the vector so that it won't be freed when the vector destructs
    return std::move(nodes.front());
}

void HuffmanNode::printTree(int indent) const {
    if (byte == -1) {
        left->printTree(indent + 1);
        right->printTree(indent + 1);
    }
    printWithIndent(indent);
}

void HuffmanNode::serialize(std::vector<short> &serialized) const {
    // depth-first, post order serialization
    if (byte == -1) {
        left->serialize(serialized);
        right->serialize(serialized);
        serialized.push_back(-1);
    } else {
        serialized.push_back(byte);
    }
}

HuffmanNode::UniquePtr HuffmanNode::deserializeFromFile(std::ifstream &file) {
    // read tree nodes
    size_t treeSize;
    file.read(reinterpret_cast<char *>(&treeSize), sizeof(treeSize));

    std::vector<HuffmanNode::UniquePtr> nodeStack;
    for (int i = 0; i < treeSize; i++) {
        short node;
        file.read(reinterpret_cast<char *>(&node), sizeof(node));
        if (node >= 0) {
            // if you read a leaf, put it on a stack
            nodeStack.push_back(std::make_unique<HuffmanNode>(node, 0.0));
        } else {
            // if you read a node take 2 children from the stack
            // (the right child node is pushed to stack first)
            auto right = std::move(nodeStack.back());
            nodeStack.pop_back();
            auto left = std::move(nodeStack.back());
            nodeStack.pop_back();
            // and put the node back on
            auto combined = std::make_unique<HuffmanNode>(std::move(left), std::move(right));
            nodeStack.push_back(std::move(combined));
        }
    }
    return std::move(nodeStack.front());
}

void HuffmanNode::printWithIndent(int indent) const {
    for (int i = 0; i < indent; i++)
        putchar(' ');
    printf("%d\n", byte);
}

void HuffmanNode::generateEncodings(std::vector<int> &pathStack, HuffmanNode::Encodings &encodings) const {
    if (byte >= 0) {
        // leaf node
        encodings[byte] = pathStack;
    } else {
        pathStack.push_back(0);
        left->generateEncodings(pathStack, encodings);
        pathStack.pop_back();

        pathStack.push_back(1);
        right->generateEncodings(pathStack, encodings);
        pathStack.pop_back();
    }
}
