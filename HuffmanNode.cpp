//
// Created by andyroiiid on 2/1/2021.
//

#include "HuffmanNode.h"

#include <algorithm>

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

void HuffmanNode::serialize(std::vector<short> &serialized) {
    // depth-first, post order serialization
    if (byte == -1) {
        left->serialize(serialized);
        right->serialize(serialized);
        serialized.push_back(-1);
    } else {
        serialized.push_back(byte);
    }
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
