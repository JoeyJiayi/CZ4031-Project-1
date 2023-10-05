#ifndef DISK_H
#define DISK_H

#include <cstddef>
#include <string>
#include <iostream>
#include <cmath>
#include <cstring>

// from the games.txt file
struct Record
{
    char GAME_DATE_EST[11];
    char TEAM_ID_HOME[11];
    int PTS_home;
    float FG_PCT_home;
    float FT_PCT_home;
    float FG3_PCT_home;
    int AST_home;
    int REB_home;
    int HOME_TEAM_WINS;
};

//memory allocation. methods and variables to process a block of records, as well as each individual record
class Disk{
    private:
        size_t blockSize; //memory occupied by a block
        size_t diskSize;  //memory allocated by user for the entire memory space
        size_t blockIdx;  //pointer to indicate how many blocks are already present
        size_t recordIdx; //pointer to indicate how many records are already present
        unsigned char *pointerToDiskAddress; //pointer to access memory space

        size_t maxRecordsPerBlock; //maximum number of records that can be contained within a block
        size_t maxBlocksPerDisk;   //maximum number of blocks that can be contained within the memory space
        size_t totalMemoryUsed;    //how much memory is currently used to store all existing records

    public:
        //constructor
        Disk(size_t aDiskSize, size_t aBlockSize);

        //methods for records
        Record *getRecord(size_t aBlockIdx, size_t aRecordIdx);

        Record *insertRecord(const std::string &GAME_DATE_EST, std::string TEAM_ID_HOME, int PTS_home, float FG_PCT_home, float FT_PCT_home, float FG3_PCT_home, int AST_home, int REB_home, int HOME_TEAM_WINS);

        void printRecord(Record *record);

        // methods for blocks
        size_t getBlockId(Record *record);

        void printBlock(size_t aBlockIdx);

        size_t getNumOfBlocksUsed();

        size_t getRecordsPerBlock();
    };

#endif
