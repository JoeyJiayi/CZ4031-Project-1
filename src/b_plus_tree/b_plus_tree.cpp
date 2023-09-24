#include "b_plus_tree.h"

// constructors for node
Node::Node(bool isLeaf){
    this->isLeaf = isLeaf;
}

Node::~Node(){}

// methods for node
Node* Node::getNextLeaf(){
    return this->nextLeaf;
}

std::vector<float> Node::getKeys(){
    return this->keys;
}

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

// methods for b_tree
Node* BPlusTree::getRoot(){
    return this->root;
}

void BPlusTree::setRoot(Node *r){
    this->root = r;
    return;
}

int BPlusTree::getMaxNumOfKeys(){
    return this->maxNumOfKeys;
}

int BPlusTree::getTotalNumOfNodes(){
    return this->numOfNodes;
}

int BPlusTree::getNumOfLevels(){
    return this->levels;
}

short BPlusTree::getBlockSize(){
    return this->blockSize;
}

short BPlusTree::getNumOfNodesSearched(){
    return this->numOfNodesSearched;
}

void BPlusTree::setNumOfNodesSearched(int num){
    this->numOfNodesSearched = num;
}

void BPlusTree::printKeys(Node *node){
    std::cout << "{ ";
    for (float key : node->keys){
        std::cout << key << " ";
    }
    std::cout << "}\n";
}

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