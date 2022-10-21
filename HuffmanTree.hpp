#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <map>
#include <stack>
#include "HuffmanBase.hpp"
#include "HeapQueue.hpp"

class HuffmanTree : HuffmanTreeBase
{
    public:
        HuffmanTree();
        ~HuffmanTree();
        std::string compress(const std::string inputStr);
        std::string serializeTree() const;
        std::string decompress(const std::string inputCode, const std::string serializedTree);
    private:
        HuffmanNode* root;
        // Methods only used for class
        void encode(HuffmanNode* root, std::string code, std::map<char, std::string>& codeMap);
        void serialize(HuffmanNode* root, std::string& str) const;
        void deleteTreeNodes(HuffmanNode* root);
};

#endif