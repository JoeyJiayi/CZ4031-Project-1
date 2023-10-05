#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <vector>
#include "../storage/disk.h"

// define a node in the b+ tree
class Node
{
private:
    bool isLeaf; // whether this node is a leaf node
    Node *nextLeaf;
    std::vector<Node *> pointers; // a pointer to an array of nodes in this disk
    std::vector<float> keys;      // a pointer to an array of keys in this node
    std::vector<std::vector<Record *> > records;
    int numOfKeys; // current number of keys in this node

public:
    // constructors
    Node(bool isLeaf);
    ~Node();

    Node *getNextLeaf();
    std::vector<float> getKeys();
    std::vector<Record *> getRecords(int idx);

    friend class BPlusTree;
};

// the b+ tree itself
class BPlusTree
{
private:
    Node *root;       // pointer to root if it's loaded
    int maxNumOfKeys; // maximum number of keys in a node
    int numOfNodes;   // number of nodes in B+ tree
    int levels;
    short blockSize;          // size of block == size of node
    short numOfNodesSearched; // number of nodes that is accessed when searching

public:
    // constructors
    BPlusTree(int blockSize);
    ~BPlusTree();

    // methods
    Node *getRoot();
    void setRoot(Node *node);

    int getMaxNumOfKeys();
    int getTotalNumOfNodes();
    int getNumOfLevels();
    short getBlockSize();

    short getNumOfNodesSearched();
    void setNumOfNodesSearched(int num);

    void printKeys(Node *node);
    void printTree(Node *node);
    float findLargestKey();
    Node *searchNode(float key);
    std::vector<Record *> *searchRecord(float key);
    void insert(float key, Record *recordPtr);
    Node *splitLeafNode(Node *curNode);
    Node *splitInternalNode(Node *curNode, float *key);
    void deleteKeyBelow(float key);
    Node *findParentNode(Node *currentNode, Node *childNode);
    void removeInternal(float key, Node *parentNode, Node *nodeToDelete);
    void updateParentKeys(Node *curNode, Node *parentNode, int parentIdx, std::vector<Node *> &parents, std::vector<int> &prevIdxs);
};

#endif
