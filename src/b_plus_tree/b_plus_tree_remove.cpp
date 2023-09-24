#include "b_plus_tree.h"
#include <algorithm>

void BPlusTree::deleteKey(float key)
{
    if (this->root == nullptr)
    {
        std::cout << "Tree is empty!" << std::endl;
        return;
    }

    std::vector<Record *> *r = this->searchRecord(key);
    if (r == nullptr)
    {
        // Key not in tree
        std::cout << "Key " << key << " is not found!" << std::endl;
        return;
    }

    Node *curNode = this->root;
    std::vector<Node *> parents(1, nullptr);
    std::vector<int> prevIdxs;
    short idx, minKeys = (this->maxNumOfKeys + 1) / 2;

    // Find the leaf node where the key belongs, saving the internal nodes visited and indexes used
    while (!curNode->isLeaf)
    {
        idx = std::upper_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
        parents.push_back(curNode);
        prevIdxs.push_back(idx);
        curNode = curNode->pointers.at(idx);
    }

    // Find the key's index in the leaf node, and delete the key and its records
    idx = std::lower_bound(curNode->keys.begin(), curNode->keys.end(), key) - curNode->keys.begin();
    curNode->keys.erase(curNode->keys.begin() + idx);
    curNode->records.erase(curNode->records.begin() + idx);

    if (curNode == this->root && curNode->keys.size() == 0)
    {
        // Check if the there are no more keys in the tree, in which case set root to null
        this->setRoot(nullptr);
    }

    if (curNode->keys.size() >= minKeys)
    {
        // Case 1: Leaf node has > minimum num of keys
        if (idx != 0)
        {
            // Case 1a: key is not the left most key, just delete
            return;
        }

        while (parents.back() != nullptr)
        {
            // Case 1b: key is the left most in the leaf node
            if (prevIdxs.back() == 0)
            {
                // If parent's idx was 0 also, iteratively check its parent
                parents.pop_back();
                prevIdxs.pop_back();
            }
            else
            {
                // If parent's idx was not 0, update the key with the new left most key of the leaf node
                key = curNode->keys.front();
                curNode = parents.back();
                idx = prevIdxs.back();
                curNode->keys[idx - 1] = key;
                return;
            }
        }
    }
    else
    {
        // Case 2: Leaf node now has less than min num of keys
        int parentIdx = prevIdxs.back();
        prevIdxs.pop_back();
        Node *leftNeighbour = nullptr, *rightNeighbour = nullptr;
        Node *parentNode = parents.back();
        parents.pop_back();

        if (parentIdx > 0)
        {
            // Left neighbour from the same parent node exists
            leftNeighbour = parentNode->pointers.at(parentIdx - 1);
            if (leftNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from left neighbour
                curNode->keys.insert(curNode->keys.begin(), leftNeighbour->keys.back());
                curNode->records.insert(curNode->records.begin(), leftNeighbour->records.back());
                leftNeighbour->keys.pop_back();
                leftNeighbour->records.pop_back();
                parentNode->keys[parentIdx] = curNode->keys.front();
                return;
            }
        }
        if (parentIdx < parentNode->keys.size() - 1)
        {
            // Right neighbour from the same parent node exists
            rightNeighbour = parentNode->pointers.at(parentIdx + 1);
            if (rightNeighbour->keys.size() > minKeys)
            {
                // Case 2a: Borrow key from right neighbour
                curNode->keys.insert(curNode->keys.end(), rightNeighbour->keys.front());
                curNode->records.insert(curNode->records.end(), rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
                parentNode->keys[parentIdx] = rightNeighbour->keys.front();

                if (!idx)
                {
                    this->updateParentKeys(curNode, parentNode, parentIdx, parents, prevIdxs);
                }
                return;
            }
        }

        // Check if left neighbour exists, if it does merge with it, else merge with right neighbour
        if (leftNeighbour != nullptr)
        {
            // Left neighbour exists but has min keys -> MERGE
            while (curNode->keys.size() != 0)
            {
                leftNeighbour->keys.push_back(curNode->keys.front());
                leftNeighbour->records.push_back(curNode->records.front());
                curNode->keys.erase(curNode->keys.begin());
                curNode->records.erase(curNode->records.begin());
            }

            leftNeighbour->nextLeaf = curNode->nextLeaf;
            this->removeInternal(parentNode->keys.at(parentIdx - 2), parentNode, curNode);
        }
        else
        {
            // Right neighbour exists but has min keys -> MERGE
            while (rightNeighbour->keys.size() != 0)
            {
                curNode->keys.push_back(rightNeighbour->keys.front());
                curNode->records.push_back(rightNeighbour->records.front());
                rightNeighbour->keys.erase(rightNeighbour->keys.begin());
                rightNeighbour->records.erase(rightNeighbour->records.begin());
            }

            curNode->nextLeaf = rightNeighbour->nextLeaf;
            this->removeInternal(parentNode->keys.at(parentIdx), parentNode, rightNeighbour);

            // if (!idx){
            //     this->updateParentKeys(curNode, parentNode, parentIdx, parents, prevIdxs);
            // }
        }
    }
}

Node *BPlusTree::findParentNode(Node *parentNode, Node *childNode)
{
    int key, idx;
    Node *curNode = childNode;
    while (!curNode->isLeaf)
    {
        curNode = curNode->pointers.front();
    }
    key = curNode->keys.front();

    while (!parentNode->isLeaf)
    {
        idx = std::upper_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
        if (parentNode->pointers.at(idx) == childNode)
        {
            return parentNode;
        }
        else
        {
            parentNode = parentNode->pointers.at(idx);
        }
    }

    return nullptr;
}

void BPlusTree::removeInternal(float key, Node *parentNode, Node *nodeToDelete)
{
    if (parentNode == this->root)
    {
        if (parentNode->keys.size() == 1)
        {
            if (parentNode->pointers.at(0) == nodeToDelete)
            {
                this->setRoot(parentNode->pointers.at(1));
            }
            else
            {
                this->setRoot(parentNode->pointers.at(0));
            }
            return;
        }
    }

    // Delete the nodeToDelete
    int idx = std::lower_bound(parentNode->keys.begin(), parentNode->keys.end(), key) - parentNode->keys.begin();
    parentNode->keys.erase(parentNode->keys.begin() + idx);
    for (idx = 0; idx < parentNode->pointers.size(); idx++)
    {
        if (parentNode->pointers[idx] == nodeToDelete)
        {
            break;
        }
    }
    parentNode->pointers.erase(parentNode->pointers.begin() + idx);
    this->numOfNodes--;

    // Return if the parentNode has more than the min number of keys
    if (parentNode->keys.size() >= this->maxNumOfKeys / 2)
    {
        return;
    }

    // Find the parentNode's left and right neighbours
    Node *ancestorNode = this->findParentNode(this->root, parentNode);
    for (idx = 0; idx < ancestorNode->pointers.size(); idx++)
    {
        if (ancestorNode->pointers.at(idx) == parentNode)
        {
            break;
        }
    }
    Node *leftNeighbour, *rightNeighbour;
    if (idx > 0)
    {
        leftNeighbour = parentNode->pointers.at(idx - 1);
        if (leftNeighbour->keys.size() > this->maxNumOfKeys / 2)
        {
            parentNode->keys.insert(parentNode->keys.begin(), ancestorNode->keys.at(idx - 1));
            ancestorNode->keys[idx - 1] = leftNeighbour->keys.back();

            parentNode->pointers.insert(parentNode->pointers.begin(), leftNeighbour->pointers.back());

            leftNeighbour->keys.pop_back();
            leftNeighbour->pointers.pop_back();
        }
    }

    if (idx < ancestorNode->pointers.size() - 1)
    {
        rightNeighbour = ancestorNode->pointers.at(idx + 1);

        if (rightNeighbour->keys.size() > this->maxNumOfKeys / 2)
        {
            parentNode->keys.push_back(ancestorNode->keys.at(idx));
            parentNode->keys[idx] = rightNeighbour->keys.front();
            rightNeighbour->keys.erase(rightNeighbour->keys.begin());

            parentNode->pointers.push_back(rightNeighbour->pointers.front());
            rightNeighbour->pointers.erase(rightNeighbour->pointers.begin());

            return;
        }
    }

    if (idx > 0)
    {
        leftNeighbour->keys.push_back(ancestorNode->keys.at(idx - 1));

        while (parentNode->keys.size() != 0)
        {
            leftNeighbour->keys.push_back(parentNode->keys.front());
        }

        while (parentNode->pointers.size() != 0)
        {
            leftNeighbour->pointers.push_back(parentNode->pointers.front());
        }

        this->removeInternal(ancestorNode->keys.at(idx - 1), ancestorNode, parentNode);
    }
    else if (idx < ancestorNode->pointers.size() - 1)
    {
        parentNode->keys.push_back(ancestorNode->keys.at(idx));

        while (rightNeighbour->keys.size() != 0)
        {
            parentNode->keys.push_back(rightNeighbour->keys.front());
        }

        while (rightNeighbour->pointers.size() != 0)
        {
            parentNode->pointers.push_back(rightNeighbour->pointers.front());
        }

        this->removeInternal(ancestorNode->keys.at(idx), ancestorNode, rightNeighbour);
    }
}

void BPlusTree::updateParentKeys(Node *curNode, Node *parentNode, int parentIdx, std::vector<Node *> &parents, std::vector<int> &prevIdxs)
{
    while (parentNode != nullptr)
    {
        // Iteratively check and update the parent nodes
        if (parentIdx == 0)
        {
            // If parent's idx was 0 also, iteratively check its parent
            parentNode = parents.back();
            parents.pop_back();
            parentIdx = prevIdxs.back();
            prevIdxs.pop_back();
        }
        else
        {
            // If parent's idx was not 0, update the key with the new left most key of the leaf node
            int key = curNode->keys.front();
            parentNode->keys[parentIdx - 1] = key;
            break;
        }
    }
}