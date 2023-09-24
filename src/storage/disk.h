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

class Disk{
    private:
        size_t blockSize;
        size_t diskSize;
        size_t blockIdx;
        size_t recordIdx;
        unsigned char *pointerToDiskAddress;

        size_t maxRecordsPerBlock;
        size_t maxBlocksPerDisk;
        size_t totalMemoryUsed;

    public:
        Disk(size_t aDiskSize, size_t aBlockSize);

        Record *getRecord(size_t aBlockIdx, size_t aRecordIdx);

        Record *insertRecord(const std::string &GAME_DATE_EST, std::string TEAM_ID_HOME, int PTS_home, float FG_PCT_home, float FT_PCT_home, float FG3_PCT_home, int AST_home, int REB_home, int HOME_TEAM_WINS);

        void printRecord(Record *record);

        size_t getBlockId(Record *record);

        void printBlock(size_t aBlockIdx);

        size_t getNumOfBlocksUsed();

        size_t getRecordsPerBlock();

        size_t getTotalMemoryUsed();
    };

#endif