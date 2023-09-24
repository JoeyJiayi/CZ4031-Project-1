#include "disk.h"

using namespace std;

Disk::Disk(size_t diskSize, size_t blockSize)
{

    this->diskSize = diskSize;
    this->blockSize = blockSize;
    this->totalMemoryUsed = 0;

    pointerToDiskAddress = new unsigned char[diskSize]();

    maxRecordsPerBlock = floor(blockSize / sizeof(Record));
    maxBlocksPerDisk = floor(diskSize / blockSize);

    blockIdx = 0;
    recordIdx = 0;
    cout << "Disk Capacity Chosen: " << diskSize / pow(2, 20) << " MB" << endl;
    cout << "Block Size: " << blockSize << " B" << endl;
}

Record *Disk::insertRecord(const string &GAME_DATE_EST, const string TEAM_ID_HOME, int PTS_home, float FG_PCT_home, float FT_PCT_home, float FG3_PCT_home, int AST_home, int REB_home, int HOME_TEAM_WINS)
{
    // only insert if don't exit max blocks per disk
    if (blockIdx >= maxBlocksPerDisk)
    {
        std::cout << "Error: No memory left to allocate new block";
        return nullptr;
    }

    // point to new record
    Record *newRecord = getRecord(blockIdx, recordIdx);

    // set values for new record 
    strncpy(newRecord->GAME_DATE_EST, GAME_DATE_EST.c_str(), sizeof(newRecord->GAME_DATE_EST));
    strncpy(newRecord->TEAM_ID_HOME, TEAM_ID_HOME.c_str(), sizeof(newRecord->TEAM_ID_HOME));
    newRecord->PTS_home = PTS_home;
    newRecord->FG_PCT_home = FG_PCT_home;
    newRecord->FT_PCT_home = FT_PCT_home;
    newRecord->FG3_PCT_home = FG3_PCT_home;
    newRecord->AST_home = AST_home;
    newRecord->REB_home = REB_home;
    newRecord->HOME_TEAM_WINS = HOME_TEAM_WINS;

    // first block and first record
    if (blockIdx == 0 && recordIdx == 0)
    {
        totalMemoryUsed = totalMemoryUsed + blockSize;
    }

    recordIdx++;

    // last record allowed for block 
    if (recordIdx == maxRecordsPerBlock)
    {
        blockIdx++;
        totalMemoryUsed = totalMemoryUsed + blockSize;
        recordIdx = 0;
    }

    return newRecord;
}

Record *Disk::getRecord(size_t blockIdx, size_t recordIdx)
{
    size_t offset = (blockIdx * blockSize) + (recordIdx * sizeof(Record));
    return reinterpret_cast<Record *>(pointerToDiskAddress + offset);
}

void Disk::printRecord(Record *record)
{
    printf("Game_Date_Est: %s\n Team_ID_home: %s\n PTS_home: %d\n FG_PCT_home: %f\n, FT_PCT_home: %f\n, FG3_PCT_home: %f\n, AST_home: %d\n, REB_home: %d\n, HOME_TEAM_WINS: %d\n,", record->GAME_DATE_EST, record->TEAM_ID_HOME, record->PTS_home, record->FG_PCT_home, record->FT_PCT_home, record->FG3_PCT_home, record->AST_home, record->REB_home, record->HOME_TEAM_WINS);
}

size_t Disk::getBlockId(Record *record)
{
    return (reinterpret_cast<unsigned char *>(record) - pointerToDiskAddress) / blockSize;
}

void Disk::printBlock(size_t aBlockIdx)
{
    cout << "Contents of Block (blockIdx=" << aBlockIdx << "):" << endl;

    for (int i = 0; i < maxRecordsPerBlock; i++)
    {
        cout << getRecord(aBlockIdx, i)->GAME_DATE_EST << " ";
    }
    cout << endl;
}

size_t Disk::getNumOfBlocksUsed()
{
    return blockIdx + 1;
}

size_t Disk::getRecordsPerBlock()
{
    return maxRecordsPerBlock;
}

size_t Disk::getTotalMemoryUsed()
{
    return totalMemoryUsed;
}