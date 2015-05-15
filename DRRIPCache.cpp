#include "DRRIPCache.h"

DRRIPCache::DRRIPCache(uint a, uint b, uint c, uint n, uint p, uint m) : BaseCache(a,b,c)
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
    
    throttleCounter = 0;
    throttleParam = p;
    psel = Monitor();
    psel.set(m);    
}

DRRIPCache::~DRRIPCache()
{
    free(rrpv);
}

int DRRIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void DRRIPCache::promotion(uint setIndex, uint wayIndex)
{
    rrpv[setIndex][wayIndex] = 0;
    sets[setIndex][wayIndex].reuseCount++;
}

uint DRRIPCache::victimize(uint setIndex)
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

void DRRIPCache::eviction(uint setIndex, uint wayIndex)
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

void DRRIPCache::insertion(uint setIndex, uint wayIndex, uint tag,bool isSRRIP)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(throttleCounter==throttleParam || isSRRIP)
    {
        rrpv[setIndex][wayIndex] = (maxRRPV - 1);
        if(throttleCounter == throttleParam)
            throttleCounter = 0;
        else
            throttleCounter++;
    }
    else
    {
        rrpv[setIndex][wayIndex] = maxRRPV;
        throttleCounter++;
    }
}

void DRRIPCache::update(uint addr)
{
    uint temp = addr >> offset;
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
        type = 1; //A Dedicated SRRIP set
    else if((first+second)==((1<<half)-1))
        type = 2; //A dedicated BRRIP set
    else
        type = 0; //A follower set
    
    
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
        
        if(type == 1)
            psel.inc();
        else if(type == 2)
            psel.dec();
        
        bool isSRRIP;
        if(type == 1)
            isSRRIP = true;
        else if(type == 2)
            isSRRIP = false;
        else
        {
            if (psel.value()>=(psel.max()/2)) //psel value crosses mid val = LRU causes more miss, ie choose BIP
                isSRRIP = false;
            else
                isSRRIP = true;
        }
        
        insertion(setIndex,victimWayIndex,tag,isSRRIP);
    }
    totalAccess++;
}

