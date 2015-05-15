#include "SRRIPCache.h"

SRRIPCache::SRRIPCache(uint a, uint b, uint c, uint n):BaseCache(a,b,c)
{
    maxRRPV = (1<<n)-1;
    rrpv = (uint**)malloc(noOfSets*sizeof(uint*));
    for(uint i=0;i<noOfSets;++i)
        rrpv[i] = (uint*)malloc(associativity*sizeof(uint));
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            rrpv[i][j] = maxRRPV;
    }
}

SRRIPCache::~SRRIPCache()
{
    free(rrpv);
}

int SRRIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void SRRIPCache::promotion(uint setIndex, uint wayIndex)
{
    rrpv[setIndex][wayIndex] = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint SRRIPCache::victimize(uint setIndex)
{
    uint idx=0;
    bool foundVictim = false;
    while(!foundVictim)
    {
        for(uint i=0;i<associativity;++i)
        {
            if(!sets[setIndex][i].valid || (rrpv[setIndex][i]==maxRRPV && sets[setIndex][i].valid))
            {
                foundVictim = true;
                idx = i;
                break;
            }
        }
        if(!foundVictim)
        {
            for(uint i=0;i<associativity;++i)
                rrpv[setIndex][i]++;
        }
        else
            break;
    }    
    return idx;
}

void SRRIPCache::eviction(uint setIndex, uint wayIndex)
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

void SRRIPCache::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    rrpv[setIndex][wayIndex] = (maxRRPV-1);
}

void SRRIPCache::update(uint addr)
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




SRRIPCachev2::SRRIPCachev2(uint a, uint b, uint c, uint n):BaseCache(a,b,c)
{
    maxRRPV = (1<<n)-1;
    rrpv = (uint**)malloc(noOfSets*sizeof(uint*));
    for(uint i=0;i<noOfSets;++i)
        rrpv[i] = (uint*)malloc(associativity*sizeof(uint));
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            rrpv[i][j] = maxRRPV;
    }
}

SRRIPCachev2::~SRRIPCachev2()
{
    free(rrpv);
}

int SRRIPCachev2::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void SRRIPCachev2::promotion(uint setIndex, uint wayIndex)
{
    rrpv[setIndex][wayIndex]--;
    sets[setIndex][wayIndex].reuseCount++;    
}

uint SRRIPCachev2::victimize(uint setIndex)
{
    uint idx=0;
    bool foundVictim = false;
    while(!foundVictim)
    {
        for(uint i=0;i<associativity;++i)
        {
            if(!sets[setIndex][i].valid || (rrpv[setIndex][i]==maxRRPV && sets[setIndex][i].valid))
            {
                foundVictim = true;
                idx = i;
                break;
            }
        }
        if(!foundVictim)
        {
            for(uint i=0;i<associativity;++i)
                rrpv[setIndex][i]++;
        }
        else
            break;
    }    
    return idx;
}

void SRRIPCachev2::eviction(uint setIndex, uint wayIndex)
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

void SRRIPCachev2::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    rrpv[setIndex][wayIndex] = (maxRRPV-1);
    sets[setIndex][wayIndex].valid = true;
    
}

void SRRIPCachev2::update(uint addr)
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



SRRIPCachev3::SRRIPCachev3(uint a, uint b, uint c, uint n):BaseCache(a,b,c)
{
    maxRRPV = (1<<n)-1;
    rrpv = (uint**)malloc(noOfSets*sizeof(uint*));
    for(uint i=0;i<noOfSets;++i)
        rrpv[i] = (uint*)malloc(associativity*sizeof(uint));
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            rrpv[i][j] = maxRRPV;
    }
}

SRRIPCachev3::~SRRIPCachev3()
{
    free(rrpv);
}

int SRRIPCachev3::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void SRRIPCachev3::promotion(uint setIndex, uint wayIndex)
{
    rrpv[setIndex][wayIndex] = maxRRPV;
    sets[setIndex][wayIndex].reuseCount++;
}

uint SRRIPCachev3::victimize(uint setIndex)
{
    uint idx=0;
    bool foundVictim = false;
    while(!foundVictim)
    {
        for(uint i=0;i<associativity;++i)
        {
            if(!sets[setIndex][i].valid || (rrpv[setIndex][i]==maxRRPV && sets[setIndex][i].valid))
            {
                foundVictim = true;
                idx = i;
                break;
            }
        }
        if(!foundVictim)
        {
            for(uint i=0;i<associativity;++i)
                rrpv[setIndex][i]++;
        }
        else
            break;
    }    
    return idx;
}

void SRRIPCachev3::eviction(uint setIndex, uint wayIndex)
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

void SRRIPCachev3::insertion(uint setIndex, uint wayIndex, uint tag)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    rrpv[setIndex][wayIndex] = (maxRRPV-1);
}

void SRRIPCachev3::update(uint addr)
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