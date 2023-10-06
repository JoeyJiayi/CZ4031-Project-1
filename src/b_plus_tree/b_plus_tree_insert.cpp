#include "b_plus_tree.h"
#include <algorithm>

void BPlusTree::insert(float key, Record *recordPtr)
{
    // Case 1: duplicate records being inserted
    std::vector<Record *> *records = this->searchRecord(key);
    if (records != nullptr)
    {
        records->push_back(recordPtr);
        return;
    }

    // Case 2a:if no root exists, instantiate a B+ tree
    if (this->root == nullptr)
    {
        this->root = new Node(true); // create a new node in main memory and set it as root
        this->numOfNodes++;
        this->levels++;
        this->root->nextLeaf = nullptr;
        this->root->keys.push_back(key);
        this->root->records.push_back(std::vector<Record *>(1, recordPtr));
        return;
    }

    Node *curNode = this->root;
    std::vector<Node *> parentNodes(1, nullptr);
    int idx = 0;

    //  Case 2b:Else if root exists already, find the leaf node where key should be inserted
    while (!curNode->isLeaf)
    {
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        parentNodes.push_back(curNode);
        curNode = curNode->pointers.at(idx);
    }

    // If leaf node < max keys, insert the key and record into the leaf node at the sorted index
    idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
    curNode->keys.insert(curNode->keys.begin() + idx, key);
    curNode->records.insert(curNode->records.begin() + idx, std::vector<Record *>(1, recordPtr));

    // Case 3: Leaf node keys == max keys
    if (curNode->keys.size() > this->maxNumOfKeys)
    {
        Node *newNode = this->splitLeafNode(curNode);
        Node *parentNode = parentNodes.back();
        parentNodes.pop_back();
        key = newNode->keys.front();

        while (parentNode != nullptr && parentNode->keys.size() == this->maxNumOfKeys)
        {
            // Iteratively check if parent is not NULL and has max children
            // Case 3b : Parent node = max keys, split required
            idx = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + idx, key);
            parentNode->pointers.insert(parentNode->pointers.begin() + idx + 1, newNode);

            newNode = this->splitInternalNode(parentNode, &key);
            curNode = parentNode;

            parentNode = parentNodes.back();
            parentNodes.pop_back();
        }

        if (parentNode == nullptr)
        {
            // Root has been reached
            parentNode = new Node(false);
            this->numOfNodes++;
            parentNode->keys.push_back(key);
            parentNode->pointers.push_back(curNode);
            parentNode->pointers.push_back(newNode);
            this->root = parentNode;
            this->levels++;
            return;
        }
        else
        {
            // Case 3a: Parent node < max keys
            idx = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
            parentNode->keys.insert(parentNode->keys.begin() + idx, key);
            parentNode->pointers.insert(parentNode->pointers.begin() + idx + 1, newNode);
        }
    }
}

Node *BPlusTree::splitLeafNode(Node *curNode)
{
    //Redistribute keys and record pointers among current node and new node

    Node *splitNode = new Node(true);
    this->numOfNodes++;

    for (int i = 0; i < (this->maxNumOfKeys + 1) / 2; i++)
    {
        splitNode->keys.insert(splitNode->keys.begin(), curNode->keys.back());
        curNode->keys.pop_back();
        splitNode->records.insert(splitNode->records.begin(), curNode->records.back());
        curNode->records.pop_back();
    }
    splitNode->nextLeaf = curNode->nextLeaf;
    curNode->nextLeaf = splitNode;

    // Old node has ceiling[(N+1)/2] keys
    // New node (split node) will have floor[(N+1)/2] keys

    return splitNode;
}

Node *BPlusTree::splitInternalNode(Node *curNode, float *key)
{
    // Redistribute keys and record pointers among current node and new node (For non-leaf node)


    Node *splitNode = new Node(false);
    this->numOfNodes++;

    for (int i = 0; i < this->maxNumOfKeys / 2; i++)
    {
        splitNode->keys.insert(splitNode->keys.begin(), curNode->keys.back());
        curNode->keys.pop_back();
        splitNode->pointers.insert(splitNode->pointers.begin(), curNode->pointers.back());
        curNode->pointers.pop_back();
    }

    splitNode->pointers.insert(splitNode->pointers.begin(), curNode->pointers.back());
    curNode->pointers.pop_back();
    *key = curNode->keys.back();
    curNode->keys.pop_back();

    // Current node has ceiling(N/2) keys
    // New node (split node) has floor(N/2) keys

    return splitNode;
}
