#include "LRUCache.h"

LRUCache::LRUCache(uint a, uint b, uint c):BaseCache(a,b,c)
{
    
}

LRUCache::~LRUCache()
{
    
}

int LRUCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void LRUCache::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid && sets[setIndex][i].age<sets[setIndex][wayIndex].age)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint LRUCache::victimize(uint setIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].age == (associativity-1) || !sets[setIndex][i].valid)
            return i;
    }
}

void LRUCache::eviction(uint setIndex, uint wayIndex)
{
    if(sets[setIndex][wayIndex].reuseCount>=15)
        reuseCountBucket[15]++;
    else
        reuseCountBucket[sets[setIndex][wayIndex].reuseCount]++;
    
    sets[setIndex][wayIndex].reuseCount = 0;
    sets[setIndex][wayIndex].valid = false;
}

void LRUCache::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].valid = true;
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid)
            sets[setIndex][i].age++;
    }
}

void LRUCache::update(uint addr)
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


