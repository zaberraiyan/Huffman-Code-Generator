/*
Project 3 Huffmancodes, COP 3514 Section 2
Group: 7
Rier Nymoen | Driver
Vasyl Sidelnikov | Navigator
Zaber Raiyan Choudhury | Driver
*/

#include "HuffmanTree.hpp"

HuffmanTree::HuffmanTree()
{
    root = nullptr; // Initialize tree
}

HuffmanTree::~HuffmanTree()
{
    deleteTreeNodes(root);
}

void HuffmanTree::deleteTreeNodes(HuffmanNode* root)
{
    if (root == nullptr)
        return;

    if (!root->isLeaf()) // delete the subtrees before deleting root
    {
        deleteTreeNodes(root->left);
        deleteTreeNodes(root->right);
    }
    delete root; // delete the root
}

void HuffmanTree::encode(HuffmanNode* root, std::string code, std::map<char, std::string> &codeMap) 
{// Made a separate method for recursion
    if (root == nullptr)
        return;
    
    if (root->isLeaf())
    {
        codeMap[root->getCharacter()] = code;
    }

    encode(root->left, code + "0", codeMap); // Add a 0 when traversal goes left
    encode(root->right, code + "1", codeMap); // Add a 1 when traversal goes right
}

void HuffmanTree::serialize(HuffmanNode* root, std::string& str) const
{
    if (root == nullptr)
        return;
    
    serialize(root->left, str);
    serialize(root->right, str);

    if (root->isLeaf())
    {//It is a Leaf
        str += "L";
        str += root->getCharacter();
    }
    else // It is a Branch
        str += "B";
}

std::string HuffmanTree::compress(const std::string inputStr)
{
    // 1. Generate a list of the frequency in which characters appear in the string using a map
    // Key of map can be the character, with the value being the frequency. This is because the keys must be unique, which is each letter. Different characters can have the same frequency.
    std::map<char, size_t> charMap;
    // The key is a character and the value is the frequency (size_t)
    for (size_t i = 0; i < inputStr.size(); i++)
    {
        char currChar = inputStr.at(i);
        if (charMap.find(currChar) == charMap.end()) // If the character does not currently exist in the map
        {
            charMap.insert(std::pair<char, size_t>(currChar, 1)); // Initial frequency is just 1
        }
        else // The character has an existing key, and we have to then increment its frequency
        {
            std::map<char, size_t>::iterator it = charMap.find(currChar);
            size_t currFrequency = it->second;
            it->second = ++currFrequency; // Update the frequency
        }
    }

    // 2. Insert the characters and their frequencies into a priority queue (sorted first by the lowest frequency and then lexicographically)
    HeapQueue<HuffmanNode*, HuffmanNode::Compare> priorityQueue; // Using a pointer to a HuffmanNode in the priority queue because compiler was throwing errors
    for (std::map<char, size_t>::iterator it = charMap.begin(); it != charMap.end(); ++it) // Iterate through the map to add to the priority queue
    {
        char character = it->first;
        size_t frequency = it->second;
        HuffmanNode* n = new HuffmanNode(character, frequency);
        priorityQueue.insert(n);
    }

    /* 3. Until there is one element left in the priority queue
            •	Remove two characters/frequencies pairs from the priority queue
            •	Turn them into leaf nodes on a binary tree
            •	Create an intermediate node to be their parent using the sum of the frequencies for those children
            •	Put that intermediate node back in the priority queue
    */

   while (priorityQueue.size() != 1)
   {
       HuffmanNode* min1 = priorityQueue.min();
       priorityQueue.removeMin();
       HuffmanNode* min2 = priorityQueue.min();
       priorityQueue.removeMin();

       size_t freqSum = min1->getFrequency() + min2->getFrequency(); // The sum of the frequencies for the intermediate node that will be created next
       HuffmanNode* intermediate = new HuffmanNode('\0', freqSum, nullptr, min1, min2);
       min1->parent = intermediate;
       min2->parent = intermediate;
       priorityQueue.insert(intermediate);
   }

    root = priorityQueue.min();
    /* 4. Using this new tree, encode the characters in the string using a map with their prefix code by traversing the tree to find where the character’s leaf is. When traversal 
       goes left, add a 0 to the code, when it goes right, add a 1 to the code 
    */
    std::map<char, std::string> codeMap;
    std::string code = "";
    encode(root, code, codeMap);

    std::string compressedStr;
    // 5. With this encoding, replace the characters in the string with their new variable-length prefix codes
    for (size_t i = 0; i < inputStr.size(); i++)
    {
        compressedStr += codeMap[inputStr.at(i)];
    }

    return compressedStr;
}

std::string HuffmanTree::serializeTree() const
{
    std::string serializedStr;

    serialize(root, serializedStr);

    return serializedStr;
}

std::string HuffmanTree::decompress(const std::string inputCode, const std::string serializedTree)
{
    std::string decompressedStr;

    std::stack<HuffmanNode*> decompressStack; // For building the tree

    // 1. Your Huffman tree will have to be built by deserializing the tree string by using the leaves and branches indicators.
    for (size_t i = 0; i < serializedTree.size(); i++)
    {
        // Serialized tree string was built through a post-order traversal, so leaves (L) would be first.
        char currChar = serializedTree.at(i);

        if (currChar == 'L')
        {
            i++; // Next character to L is the targeted character of the string
            currChar = serializedTree.at(i);
            HuffmanNode* n = new HuffmanNode(currChar, 0);
            decompressStack.push(n);
        }
        else if (currChar == 'B')
        {
            HuffmanNode* right = decompressStack.top();
            decompressStack.pop();
            HuffmanNode* left = decompressStack.top();
            decompressStack.pop();

            HuffmanNode* intermediate = new HuffmanNode('\0', 0, nullptr, left, right); // When compressing, we used the null character for the intermediate
            right->parent = intermediate;
            left->parent = intermediate;
            decompressStack.push(intermediate);
        }
    }

    root = decompressStack.top(); // Tree is built

    /* 2. After you have your tree back, you can decompress the Huffman Code by tracing the tree to figure out what variable length codes represent actual characters 
     * from the original string. */
    HuffmanNode* currNode = root;
    for (size_t i = 0; i < inputCode.size(); i++)
    {
        char currChar = inputCode.at(i);

        if (currChar == '0') // left
        {
            currNode = currNode->left;
        }
        else // it's 1, so right
        {
            currNode = currNode->right;
        }

        if (currNode->isLeaf()) // We reach the full character path
        {
            decompressedStr += currNode->getCharacter();
            currNode = root;
        }
    }
    
    return decompressedStr;
}