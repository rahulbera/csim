#include "NRUCache.h"

NRUCache::NRUCache(uint a, uint b, uint c) : BaseCache(a,b,c)
{
    nruBit = (bool**)malloc(noOfSets*sizeof(bool*));
    for(uint i=0;i<noOfSets;++i)
        nruBit[i] = (bool*)malloc(associativity*sizeof(bool));
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            nruBit[i][j] = true;
    }
}

NRUCache::~NRUCache()
{
    free(nruBit);
}

int NRUCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void NRUCache::promotion(uint setIndex, uint wayIndex)
{
    nruBit[setIndex][wayIndex] = false;
    sets[setIndex][wayIndex].reuseCount++;
}

uint NRUCache::victimize(uint setIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(!sets[setIndex][i].valid) return i;
    }
    for(uint i=0;i<associativity;++i)
    {
        if(nruBit[setIndex][i])
            return i;
    }
    for(uint i=0;i<associativity;++i)
        nruBit[setIndex][i] = true;
    return 0;
    
}

void NRUCache::eviction(uint setIndex, uint wayIndex)
{
    if(sets[setIndex][wayIndex].valid)
    {
        if(sets[setIndex][wayIndex].reuseCount>=15)
            reuseCountBucket[15]++;
        else
            reuseCountBucket[sets[setIndex][wayIndex].reuseCount]++;
    }    
    
    sets[setIndex][wayIndex].reuseCount = 0;
    sets[setIndex][wayIndex].valid = false;
}

void NRUCache::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    nruBit[setIndex][wayIndex] = false;
}

void NRUCache::update(uint addr)
{
    uint temp = addr >> offset;
    uint setIndex = temp & ((1<<setsInPowerOfTwo)-1);
    uint tag = temp >> setsInPowerOfTwo;
    
    int wayIndex = find(setIndex,tag);
    if(wayIndex != -1)
    {
        hit++;
        promotion(setIndex,wayIndex);
    }        
    else
    {
        miss++;
        uint victimWayIndex = victimize(setIndex);
        if(sets[setIndex][victimWayIndex].valid)
            eviction(setIndex,victimWayIndex);
        insertion(setIndex,victimWayIndex,tag);
    }
    totalAccess++;
}



