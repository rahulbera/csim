#include "TrialCache.h"

TrialCache::TrialCache(uint a, uint b, uint c) : BaseCache(a,b,c)
{
    hint = (bool**)malloc(noOfSets*sizeof(bool*));
    for(int i=0;i<noOfSets;++i)
        hint[i] = (bool*)malloc(associativity*sizeof(bool));
    for(int i=0;i<noOfSets;++i)
    {
        for(int j=0;j<associativity;++j)
            hint[i][j] = false;
    }
}

TrialCache::~TrialCache() 
{
    free(hint);
}

int TrialCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void TrialCache::promotion(uint setIndex, uint wayIndex, bool isLive)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid && sets[setIndex][i].age<sets[setIndex][wayIndex].age)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    hint[setIndex][wayIndex] = isLive;
    sets[setIndex][wayIndex].reuseCount++;
}

uint TrialCache::victimize(uint setIndex)
{
    uint maxAgeIndex=0, maxAge = 0; 
    
    for(int i=0;i<associativity;++i)
    {
        if(!sets[setIndex][i].valid)
            return i;
        else if(!hint[setIndex][i])
            return i;
        else
        {   
            if(sets[setIndex][i].age>maxAge)
            {
                maxAge = sets[setIndex][i].age;
                maxAgeIndex = i;
            } 
        }
    }
    
    return maxAgeIndex;
}

void TrialCache::eviction(uint setIndex, uint wayIndex)
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

void TrialCache::insertion(uint setIndex, uint wayIndex, uint tag,bool isLive)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    hint[setIndex][wayIndex] = isLive;
}

void TrialCache::update(uint addr, bool isLive)
{
    uint temp = addr >> offset;
    uint setIndex = temp & ((1<<setsInPowerOfTwo)-1);
    uint tag = temp >> setsInPowerOfTwo;
    
    int wayIndex = find(setIndex,tag);
    if(wayIndex != -1)
    {
        hit++;
        promotion(setIndex,wayIndex,isLive);
    }        
    else
    {
        miss++;
        uint victimWayIndex = victimize(setIndex);
        if(sets[setIndex][victimWayIndex].valid)
            eviction(setIndex,victimWayIndex);
        insertion(setIndex,victimWayIndex,tag,isLive);
    }
    totalAccess++;
}


