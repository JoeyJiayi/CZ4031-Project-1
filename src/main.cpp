#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_set>
#include "storage/disk.h"
#include "b_plus_tree/b_plus_tree.h"

using namespace std;

/*
 =============================================================
 Experiment 1: store the data (which is about NBA games and described in Part 4) on the disk (as specified in Part 1) and report the following statistics:
 =============================================================
*/
void experiment1(Disk *disk, BPlusTree *tree)
{
    cout << "" << endl;
    cout << "Experiment 1:" << endl;

    // opening of data file
    ifstream file("data/games.txt");
    if (!file.is_open())
    {
        cout << "File failed to open." << endl;
        exit(EXIT_FAILURE);
    }
    Record *newRecord = nullptr;
    string line;
    int numRecords = 0;

    // parsing of data
    getline(file, line);
    while (getline(file, line))
    {
        istringstream iss(line);
        string GAME_DATE_EST, TEAM_ID_HOME, PTS_home, FG_PCT_home, FT_PCT_home, FG3_PCT_home, AST_home, REB_home, HOME_TEAM_WINS;
        getline(iss, GAME_DATE_EST, '\t');
        getline(iss, TEAM_ID_HOME, '\t');
        getline(iss, PTS_home, '\t');
        getline(iss, FG_PCT_home, '\t');
        getline(iss, FT_PCT_home, '\t');
        getline(iss, FG3_PCT_home, '\t');
        getline(iss, AST_home, '\t');
        getline(iss, REB_home, '\t');
        getline(iss, HOME_TEAM_WINS, '\t');

        newRecord = (*disk).insertRecord(GAME_DATE_EST, TEAM_ID_HOME, stoi(PTS_home), std::stod(FG_PCT_home), std::stod(FT_PCT_home), std::stod(FG3_PCT_home), stoi(AST_home), stoi(REB_home), stoi(HOME_TEAM_WINS));
        tree->insert(newRecord->FG_PCT_home, newRecord);
        numRecords++;
    }
    file.close();

    // experiment 1 log
    cout << "Number of records: " << numRecords << endl;
    cout << "Size of a record: " << sizeof(Record) << " bytes" << endl;
    cout << "Number of records stored in a block: " << (*disk).getRecordsPerBlock() << endl;
    cout << "Number of blocks for storing the data: " << (*disk).getNumOfBlocksUsed() << endl;
}

/*
  =============================================================
  Experiment 2: build a B+ tree on the attribute "FG_PCT_home" by inserting the records sequentially and report the following statistics:
  the parameter n of the B+ tree;
  =============================================================
*/
void experiment2(BPlusTree *tree)
{
    // experiment 2 log
    cout << "" << endl;
    cout << "Experiment 2:" << endl;
    cout << "Parameter n = " << tree->getMaxNumOfKeys() << endl;
    cout << "Number of nodes of the B+ tree = " << tree->getTotalNumOfNodes() << endl;
    cout << "Number of levels of the B+ tree = " << tree->getNumOfLevels() << endl;
    cout << "Content of Root Node (only the keys): ";
    tree->printKeys(tree->getRoot());
    cout << endl;
}

/*
 =============================================================
 Experiment 3: Retrieve those games with the “FG_PCT_home” equal to 0.5 and report the following statistics:
 =============================================================
 */

void experiment3(Disk *disk, BPlusTree *tree)
{
    tree->setNumOfNodesSearched(0);

    auto before = chrono::steady_clock::now();
    vector<Record *> *retrieved = tree->searchRecord(0.5);
    auto after = chrono::steady_clock::now();
    auto timeTaken = chrono::duration_cast<chrono::nanoseconds>(after - before);

    // finding the FG3_PCT_home values of all records where FG_PCT_home = 0.5
    unordered_set<size_t> answer;
    float totalFG3 = 0;
    for (Record *r : *retrieved)
    {
        answer.insert(disk->getBlockId(r));
        totalFG3 += r->FG3_PCT_home;
    }

    // brute force linear scan of disk, block by block, record by record
    int bruteForceBlocks = 0;
    Record *r;
    before = chrono::steady_clock::now();
    for (int i = 0; i < disk->getNumOfBlocksUsed(); i++)
    {
        bruteForceBlocks++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++)
        {
            r = disk->getRecord(i, j);
            if (r->FG_PCT_home == 0.5)
            {
                continue;
            }
        }
    }
    after = chrono::steady_clock::now();
    auto bruteForceTime = chrono::duration_cast<chrono::nanoseconds>(after - before);

    // experiment 3 log
    cout << "Experiment 3:" << endl;
    cout << "Number of index nodes processed: " << tree->getNumOfNodesSearched() << endl;
    cout << "Number of data blocks processed: " << answer.size() << endl;
    cout << "Average of FG3_PCT_home of all the records returned: " << totalFG3 / retrieved->size() << endl;
    cout << "Running time of retrieval process (in nanoseconds): " << timeTaken.count() << "ns" << endl;
    cout << "Number of data blocks accessed by brute-force linear scan method: " << bruteForceBlocks << endl;
    cout << "Running time of brute-force linear scan method (in nanoseconds): " << bruteForceTime.count() << "ns" << endl;
    cout << endl;
}

/*
=============================================================
Experiment 4: retrieve those records with the attribute “FG_PCT_home” from 0.6 to 1, both inclusively and report the following statistics:
• the number of index nodes the process accesses;
• the number of data blocks the process accesses;
• the average of “FG3_PCT_home” of the records that are returned;
• the running time of the retrieval process;
• the number of data blocks that would be accessed by a brute-force
linear scan method (i.e., it scans the data blocks one by one) and its
running time (for comparison)
=============================================================
*/
void experiment4(Disk *disk, BPlusTree *tree)
{
    vector<Record *> result;
    vector<float> keys;
    float lower = 0.6;
    float upper = 1.0;
    int lowerIdx, upperIdx, leafNodesSearched = 0;
    bool searching = true;
    tree->setNumOfNodesSearched(0);

    auto before = chrono::steady_clock::now();
    Node *resultNode = tree->searchNode(lower);

    // locating of first record where key >= lower
    while (searching)
    {
        keys = resultNode->getKeys();
        lowerIdx = lower_bound(keys.begin(), keys.end(), lower) - keys.begin();
        upperIdx = lower_bound(keys.begin(), keys.end(), upper) - keys.begin();

        for (int i = lowerIdx; i < upperIdx; i++)
        {
            for (Record *r : resultNode->getRecords(i))
            {
                result.push_back(r);
            }
        }

        upperIdx = upperIdx == keys.size() ? upperIdx - 1 : upperIdx;

        if (keys.at(upperIdx) >= upper || keys.at(upperIdx) >= tree->findLargestKey())
        {
            searching = false;
        }
        else
        {
            resultNode = resultNode->getNextLeaf();
            leafNodesSearched++;
        }
    }

    auto after = chrono::steady_clock::now();
    auto timeTaken = chrono::duration_cast<chrono::nanoseconds>(after - before);

    // finding the FG3_PCT_home values of all records where 0.6 <= FG_PCT_home <= 1.0
    unordered_set<size_t> resultSet;
    float totalFG3 = 0;
    for (Record *r : result)
    {
        resultSet.insert(disk->getBlockId(r));
        totalFG3 += r->FG3_PCT_home;
    }

    // brute force linear scan of disk, block by block, record by record
    int bruteForceBlocks = 0;
    Record *r;
    before = chrono::steady_clock::now();
    for (int i = 0; i < disk->getNumOfBlocksUsed(); i++)
    {
        bruteForceBlocks++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++)
        {
            r = disk->getRecord(i, j);
            if (r->FG_PCT_home >= lower && r->FG_PCT_home <= upper)
            {
                continue;
            }
        }
    }
    after = chrono::steady_clock::now();
    auto bruteTimeTaken = chrono::duration_cast<chrono::nanoseconds>(after - before);

    // experiment 4 log
    cout << "Experiment 4:" << endl;
    cout << "Number of index nodes accessed: " << tree->getNumOfNodesSearched() + leafNodesSearched << endl;
    cout << "Number of data blocks accessed: " << resultSet.size() << endl;
    cout << "Average of FG3_PCT_home of all the records returned: " << totalFG3 / result.size() << endl;
    cout << "Running time for retrieval process (in nanoseconds): " << timeTaken.count() << "ns" << endl;
    cout << "Number of data blocks accessed by brute force method: " << bruteForceBlocks << endl;
    cout << "Running time for retrival by brute force method (in nanoseconds): " << bruteTimeTaken.count() << "ns" << endl;
    cout << endl;
}

/*
=============================================================
Experiment 5: delete those movies with the attribute “FG_PCT_home” below 0.35 inclusively, update the B+ tree accordingly, and report the following statistics:
• the number nodes of the updated B+ tree;
• the number of levels of the updated B+ tree;
CE4031-CZ4031
• the content of the root node of the updated B+ tree(only the keys);
• the running time of the process;
• the number of data blocks that would be accessed by a brute-force
linear scan method (i.e., it scans the data blocks one by one) and its running time (for comparison)
=============================================================
*/
void experiment5(Disk *disk, BPlusTree *tree)
{
    // deletion of all records with key <= 0.35
    float keysToDeleteBelow = 0.35;
    auto before = chrono::steady_clock::now();
    tree->deleteKeyBelow(keysToDeleteBelow);
    auto after = chrono::steady_clock::now();
    auto timeTaken = chrono::duration_cast<chrono::nanoseconds>(after - before);

    int numOfBlocksAcc = 0;
    Record *r;
    before = chrono::steady_clock::now();

    // brute force linear scan of disk, block by block, record by record
    vector<float> selectedKeys;
    for (int i = 0; i < disk->getNumOfBlocksUsed(); i++)
    {
        numOfBlocksAcc++;
        for (int j = 0; j < disk->getRecordsPerBlock(); j++)
        {
            r = disk->getRecord(i, j);
            if (r->FG_PCT_home <= keysToDeleteBelow)
            {
                continue;
            }
            selectedKeys.push_back(r->FG_PCT_home);
        }
    }
    after = chrono::steady_clock::now();
    auto bruteTimeTaken = chrono::duration_cast<chrono::nanoseconds>(after - before);

    // experiment 5 log
    cout << "Experiment 5:" << endl;
    cout << "Number of nodes of the updated B+ Tree: " << tree->getTotalNumOfNodes() << endl;
    cout << "Number of levels of the updated B+ Tree: " << tree->getNumOfLevels() << endl;
    cout << "Content of Root Node of updated B+ Tree: ";
    tree->printKeys(tree->getRoot());
    cout << "Running time for deletion process (in nanoseconds): " << timeTaken.count() << "ns" << endl;
    cout << "Number of data blocks accessed by brute force method: " << numOfBlocksAcc << endl;
    cout << "Running time for deletion by brute force method (in nanoseconds): " << bruteTimeTaken.count() << "ns" << endl;
    cout << endl;
}

int main()
{
    // Set block size to 400B and disk size to 100MB
    int BLOCKSIZE = 400;
    int DISKSIZE = 100 * pow(2, 20);
    Disk disk(DISKSIZE, BLOCKSIZE);
    BPlusTree tree(BLOCKSIZE);

    experiment1(&disk, &tree);

    experiment2(&tree);

    experiment3(&disk, &tree);

    experiment4(&disk, &tree);

    experiment5(&disk, &tree);
}
