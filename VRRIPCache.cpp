#include "VRRIPCache.h"

VRRIPCache::VRRIPCache(uint a, uint b, uint c, uint n, uint m) : BaseCache(a,b,c)
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
    sel = Monitor();
    sel.set(m);
}

VRRIPCache::~VRRIPCache()
{
    free(rrpv);
}

int VRRIPCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void VRRIPCache::promotion(uint setIndex, uint wayIndex)
{
    sets[setIndex][wayIndex].reuseCount++;
    /*if(b)
        rrpv[setIndex][wayIndex] = maxRRPV;
    else*/
        rrpv[setIndex][wayIndex] = 0;
}

uint VRRIPCache::victimize(uint setIndex)
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

void VRRIPCache::eviction(uint setIndex, uint wayIndex)
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

void VRRIPCache::insertion(uint setIndex, uint wayIndex, uint tag, bool b)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].valid = true;
    if(b)
        rrpv[setIndex][wayIndex] = maxRRPV;
    else
        rrpv[setIndex][wayIndex] = (maxRRPV-1);
}

void VRRIPCache::update(uint addr)
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
        type = 1; //Sets where value promotes to maxRRPV
    else if((first+second)==((1<<half)-1))
        type = 2; //Sets where value promotes to 0
    else
        type = 0; //A follower set
    
    int wayIndex = find(setIndex,tag);
    if(wayIndex != -1)
    {
        hit++;
        /*bool b;
        if(type == 1)
            b = true;
        else if(type == 2)
            b = false;
        else
        {
            if(sel.value()>=(sel.max()/2))
                b = false;
            else 
                b = true;
        }*/
        promotion(setIndex,wayIndex);
    }        
    else
    {
        miss++;
        
        if(type == 1)
            sel.inc();
        else if(type == 2)
            sel.dec();
        
        uint victimWayIndex = victimize(setIndex);
        if(sets[setIndex][victimWayIndex].valid)
            eviction(setIndex,victimWayIndex);
        
        bool b;
        if(type == 1)
            b = true;
        else if(type == 2)
            b = false;
        else
        {
            if(sel.value()>=(sel.max()/2))
                b = false;
            else 
                b = true;
        }
        
        insertion(setIndex,victimWayIndex,tag,b);
    }
    totalAccess++;
}
