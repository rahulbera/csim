#include "LIPCache.h"

LIPCache::LIPCache(uint a, uint b, uint c):BaseCache(a,b,c)
{
    setIsFull=(bool*)malloc(sizeof(bool)*noOfSets);
    for(uint i=0;i<noOfSets;++i) setIsFull[i]=false;
}

LIPCache::~LIPCache()
{
    free(setIsFull);
}

int LIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void LIPCache::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid && sets[setIndex][i].age<sets[setIndex][wayIndex].age)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint LIPCache::victimize(uint setIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].age == (associativity-1) || !sets[setIndex][i].valid)
            return i;
    }
}

void LIPCache::eviction(uint setIndex, uint wayIndex)
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

void LIPCache::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(!setIsFull[setIndex])
    {
        sets[setIndex][wayIndex].age = 0;   
        for(uint i=0;i<associativity;++i)
        {
            if(i!=wayIndex && sets[setIndex][i].valid)
                sets[setIndex][i].age++;
        }
        bool flag = true;
        for(uint i=0;i<associativity;++i) flag &= sets[setIndex][i].valid;
        setIsFull[setIndex] = flag;
    } 
    else
        sets[setIndex][wayIndex].age = (associativity-1);
}

void LIPCache::update(uint addr)
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


LIPCachev2::LIPCachev2(uint a, uint b, uint c) : BaseCache(a,b,c)
{
    
}

LIPCachev2::~LIPCachev2()
{
    
}

int LIPCachev2::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void LIPCachev2::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint LIPCachev2::victimize(uint setIndex)
{
    bool foundInvalid = false;
    uint invalidIdx = 0;
    uint maxAge = 0;
    uint maxAgeIdx = 0;
    for(uint i=0;i<associativity;++i)
    {
        if(!sets[setIndex][i].valid && !foundInvalid)
        {
            foundInvalid = true;
            invalidIdx = i;
        }
        else if(sets[setIndex][i].age > maxAge)
        {
            maxAge = sets[setIndex][i].age;
            maxAgeIdx = i;
        }
    }
    if(foundInvalid)
        return invalidIdx;
    else
        return maxAgeIdx;
}

void LIPCachev2::eviction(uint setIndex, uint wayIndex)
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

void LIPCachev2::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    sets[setIndex][wayIndex].age = (associativity-1);
}

void LIPCachev2::update(uint addr)
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