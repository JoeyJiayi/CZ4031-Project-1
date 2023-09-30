#include "b_plus_tree.h"

// constructors for node
Node::Node(bool isLeaf){
    this->isLeaf = isLeaf;
}

Node::~Node(){}

// methods for node

//access next leaf node of the tree
Node* Node::getNextLeaf(){
    return this->nextLeaf;
}

//obtain all keys stored within a node
std::vector<float> Node::getKeys(){
    return this->keys;
}

//obtain all records stored in a particular node
std::vector<Record*> Node::getRecords(int idx){
    return this->records.at(idx);
}

// constructors for b+ tree
BPlusTree::BPlusTree(int blockSize){
    this->root = nullptr;
    this->blockSize = blockSize;
    this->maxNumOfKeys = (blockSize - sizeof(int*)) / (sizeof(int) + sizeof(int*));
    this->numOfNodes = 0;
    this->levels = 0;
    this->numOfNodesSearched = 0;
}

BPlusTree::~BPlusTree(){}

// methods for b+ tree
Node* BPlusTree::getRoot(){
    return this->root;
}

//creating the b+ tree (setting its root node)
void BPlusTree::setRoot(Node *r){
    this->root = r;
    return;
}

//maximum number of keys that the b+ tree can contain
int BPlusTree::getMaxNumOfKeys(){
    return this->maxNumOfKeys;
}

//number of nodes currently contained within the b+ tree
int BPlusTree::getTotalNumOfNodes(){
    return this->numOfNodes;
}

//number of levels of the b+ tree
int BPlusTree::getNumOfLevels(){
    return this->levels;
}

//get block size of b+ tree
short BPlusTree::getBlockSize(){
    return this->blockSize;
}

short BPlusTree::getNumOfNodesSearched(){
    return this->numOfNodesSearched;
}

void BPlusTree::setNumOfNodesSearched(int num){
    this->numOfNodesSearched = num;
}

//print all the keys contained within a node
void BPlusTree::printKeys(Node *node){
    std::cout << "{ ";
    for (float key : node->keys){
        std::cout << key << " ";
    }
    std::cout << "}\n";
}

//display the b+ tree
void BPlusTree::printTree(Node *tmp){
    std::vector<Node *> n;
    n.push_back(tmp);
    n.push_back(nullptr);
    while (!n.empty()){
        tmp = n.front();
        n.erase(n.begin());
        if (tmp != nullptr){
            if (tmp->isLeaf) break;
            for (int i = 0; i < tmp->pointers.size(); i++)
            {
                n.push_back(tmp->pointers.at(i));
            }
            n.push_back(nullptr);
            printKeys(tmp);
        }
        else{
            std::cout << "Go To Next Level. \n";
        }
    }

    while (!tmp->isLeaf){
        tmp = tmp->pointers.at(0);
    }

    do {
        printKeys(tmp);
        std::cout << " ";
        tmp = tmp->nextLeaf;
    } while (tmp != nullptr);
    std::cout << "\n";
}

// Function to find the largest key in the B+ tree
float BPlusTree::findLargestKey() {
    Node* currentNode = root;

    // Traverse to the rightmost leaf node, which will contain the largest key
    while (!currentNode->isLeaf) {
        currentNode = currentNode->pointers.back();
    }

    // The largest key will be the last key in the rightmost leaf node
    return currentNode->keys.back();
}
