#include "DIPCache.h"

DIPCache::DIPCache(uint a, uint b, uint c, uint p, uint n) : BaseCache(a,b,c)
{
    throttleCounter = 0;
    throttleParam = p;
    psel = Monitor();
    psel.set(n);
}

DIPCache::~DIPCache()
{
    
}

int DIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void DIPCache::promotion(uint setIndex, uint wayIndex)
{
    for(uint i=0;i<associativity;++i)
    {
        if(i!=wayIndex && sets[setIndex][i].valid)
            sets[setIndex][i].age++;
    }
    sets[setIndex][wayIndex].age = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint DIPCache::victimize(uint setIndex)
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

void DIPCache::eviction(uint setIndex, uint wayIndex)
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

void DIPCache::insertion(uint setIndex, uint wayIndex, uint tag, bool isLRU)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(throttleCounter==throttleParam || isLRU)
    {
        sets[setIndex][wayIndex].age = 0;   
        for(uint i=0;i<associativity;++i)
        {
            if(i!=wayIndex && sets[setIndex][i].valid)
                sets[setIndex][i].age++;
        }
        if(throttleCounter==throttleParam)
            throttleCounter = 0;
        else
            throttleCounter++;
    } 
    else
    {
        sets[setIndex][wayIndex].age = (associativity-1);
        throttleCounter++;
    }    
}

void DIPCache::update(uint addr)
{
    int temp = addr >> offset;
    uint setIndex = temp & ((1<<setsInPowerOfTwo)-1);
    uint tag = temp >> setsInPowerOfTwo;
    
    uint first=0,second=0,half=setsInPowerOfTwo/2;
    uint type;
	
    if(setsInPowerOfTwo%2 != 0)
    {
        first = setIndex & ((1<<half)-1);
	second = setIndex >> (1+half);
    }
    else
    {
    	first = setIndex & ((1<<half)-1);
	second = setIndex >> half;
    }
    if(first == second)
        type = 1; //A Dedicated LRU set
    else if((first+second)==((1<<half)-1))
        type = 2; //A dedicated BIP set
    else
        type = 0; //A follower set
    
    int wayIndex = find(setIndex,tag);
    if(wayIndex != -1)
    {
        hit++;
        promotion(setIndex,wayIndex);
        //if(type == 1) psel.inc();
        //else if(type == 2) psel.dec();
    } 
    else
    {
        miss++;
        uint victimWayIndex = victimize(setIndex);
        if(sets[setIndex][victimWayIndex].valid)
            eviction(setIndex,victimWayIndex);
        
        if(type == 1)
            psel.inc();
        else if(type == 2)
            psel.dec();
        
        bool isLRU;
        if(type == 1)
            isLRU = true;
        else if(type == 2)
            isLRU = false;
        else
        {
            if (psel.value()>=(psel.max()/2)) //psel value crosses mid val = LRU causes more miss, ie choose BIP
                isLRU = false;
            else
                isLRU = true;
        }
        insertion(setIndex,victimWayIndex,tag,isLRU);
    }
    
    totalAccess++;
}