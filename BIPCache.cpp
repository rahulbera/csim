#include "BIPCache.h"

BIPCache::BIPCache(uint a, uint b, uint c,uint p):BaseCache(a,b,c) 
{
    setIsFull=(bool*)malloc(sizeof(bool)*noOfSets);
    throttleCounter = (uint*)malloc(sizeof(uint)*noOfSets);
    for(uint i=0;i<noOfSets;++i)
    {
        setIsFull[i]=false;
        throttleCounter[i]=0;
    }
    throttleParam = p;
}

BIPCache::~BIPCache()
{
    free(setIsFull);
}

int BIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void BIPCache::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid && sets[setIndex][i].age<sets[setIndex][wayIndex].age)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint BIPCache::victimize(uint setIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].age == (associativity-1) || !sets[setIndex][i].valid)
            return i;
    }
}

void BIPCache::eviction(uint setIndex, uint wayIndex)
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

void BIPCache::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(!setIsFull[setIndex] || throttleCounter[setIndex]==throttleParam)
    {
        sets[setIndex][wayIndex].age = 0;   
        for(uint i=0;i<associativity;++i)
        {
            if(i!=wayIndex && sets[setIndex][i].valid)
                sets[setIndex][i].age++;
        }
        if(throttleCounter[setIndex]==throttleParam)
            throttleCounter[setIndex]=0;
        else
        {
            bool flag = true;
            for(uint i=0;i<associativity;++i) flag &= sets[setIndex][i].valid;
            setIsFull[setIndex] = flag;
        }
    } 
    else
        sets[setIndex][wayIndex].age = (associativity-1);
    
    throttleCounter[setIndex]++;
}

void BIPCache::update(uint addr)
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

BIPCachev2::BIPCachev2(uint a, uint b, uint c, uint p) : BaseCache(a,b,c)
{  
    throttleCounter = 0;
    throttleParam = p;
}

BIPCachev2::~BIPCachev2()
{
    
}

int BIPCachev2::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void BIPCachev2::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint BIPCachev2::victimize(uint setIndex)
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

void BIPCachev2::eviction(uint setIndex, uint wayIndex)
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

void BIPCachev2::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(throttleCounter==throttleParam)
    {
        sets[setIndex][wayIndex].age = 0;   
        for(uint i=0;i<associativity;++i)
        {
            if(i!=wayIndex && sets[setIndex][i].valid)
                sets[setIndex][i].age++;
        }
        throttleCounter = 0;
    } 
    else
    {
        sets[setIndex][wayIndex].age = (associativity-1);
        throttleCounter++;
    }    
}

void BIPCachev2::update(uint addr)
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
