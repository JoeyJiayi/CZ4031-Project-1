#include "b_plus_tree.h"
#include <algorithm>

Node *BPlusTree::searchNode(float key)
{
    // if tree is empty
    if (this->root == nullptr)
    {
        std::cout << "Tree is empty!" << std::endl;
        return nullptr;
    }

    // else iterate through root node and to find the correct key
    else {
        Node *curNode = this->root; // start from root node and increment number of nodes searched
        int idx;
        this->numOfNodesSearched++; 

        while (!curNode->isLeaf) // while current node is not a leaf node
        {
            // iterate through the nodes until we reach the leaf node containing the correct key
            idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
            curNode = curNode->pointers.at(idx);
            this->numOfNodesSearched++;
        }

        return curNode;
    }
}

std::vector<Record *> *BPlusTree::searchRecord(float key)
{
    if (this->root == nullptr)
    {
        return nullptr;
    }

    Node *curNode = this->root;
    int idx;
    this->numOfNodesSearched++;

    while (!curNode->isLeaf)
    {
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin(); //returns an iterator pointing to the last element in the range
        curNode = curNode->pointers.at(idx);
        this->numOfNodesSearched++;
    }

    idx = std::lower_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
    if (idx < curNode->keys.size() && curNode->keys.at(idx) == key)
    {
        return &(curNode->records.at(idx));
    }

    return nullptr;
}