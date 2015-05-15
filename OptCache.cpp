#include <vector>

#include "OptCache.h"

OptCache::OptCache(uint a, uint b, uint c) : BaseCache(a,b,c)
{
    setAccessCounter = (uint*)malloc(sizeof(uint)*noOfSets);
    for(uint i=0;i<noOfSets;++i)
        setAccessCounter[i] = 0;
    eventCounter = (uint**)malloc(sizeof(uint*)*noOfSets);
    for(uint i=0;i<noOfSets;++i)
        eventCounter[i] = (uint*)malloc(sizeof(uint)*associativity);
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            eventCounter[i][j]=0;
    }
}

OptCache::~OptCache()
{
    free(setAccessCounter);
    free(eventCounter);
}

int OptCache::find(uint setIndex, uint tag)
{
    for(uint i=0;i<associativity;++i)
    {
        if(sets[setIndex][i].tag==tag && sets[setIndex][i].valid)
            return i;
    }
    return -1;
}

void OptCache::promotion(uint setIndex, uint wayIndex,uint age)
{
    sets[setIndex][wayIndex].age = age;
    sets[setIndex][wayIndex].reuseCount++;
    
    uint blockIdx = (sets[setIndex][wayIndex].tag<<setsInPowerOfTwo) | setIndex;
    std::map<uint,std::vector<uint> >::iterator it = accessIntervalStat.find(blockIdx);
    if(it!=accessIntervalStat.end())
        (it->second).push_back(setAccessCounter[setIndex]-eventCounter[setIndex][wayIndex]);
    else
    {
        std::vector<uint> list;
        list.push_back(setAccessCounter[setIndex]-eventCounter[setIndex][wayIndex]);
        accessIntervalStat.insert(std::pair<uint,std::vector<uint> >(blockIdx,list));
    }
    eventCounter[setIndex][wayIndex] = setAccessCounter[setIndex];
    setAccessCounter[setIndex]++;
}

uint OptCache::victimize(uint setIndex)
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

void OptCache::eviction(uint setIndex, uint wayIndex)
{
    if(sets[setIndex][wayIndex].reuseCount>=15)
        reuseCountBucket[15]++;
    else
        reuseCountBucket[sets[setIndex][wayIndex].reuseCount]++;
    
    uint blockIdx = (sets[setIndex][wayIndex].tag<<setsInPowerOfTwo) | setIndex;
    std::map<uint,std::vector<uint> >::iterator it = reuseCountStat.find(blockIdx);
    if(it!=reuseCountStat.end())
    {
        (it->second).push_back(sets[setIndex][wayIndex].reuseCount);
    }
    else
    {
        std::vector<uint> list;
        list.push_back(sets[setIndex][wayIndex].reuseCount);
        reuseCountStat.insert(std::pair<uint,std::vector<uint> >(blockIdx,list));
    }
    
    it = accessIntervalStat.find(blockIdx);
    if(it!=accessIntervalStat.end())
        (it->second).push_back(setAccessCounter[setIndex]-eventCounter[setIndex][wayIndex]);
    else
    {
        std::vector<uint> list;
        list.push_back(setAccessCounter[setIndex]-eventCounter[setIndex][wayIndex]);
        accessIntervalStat.insert(std::pair<uint,std::vector<uint> >(blockIdx,list));
    }
    eventCounter[setIndex][wayIndex] = 0;
    
    sets[setIndex][wayIndex].reuseCount = 0;
    sets[setIndex][wayIndex].valid = false;
}

void OptCache::insertion(uint setIndex, uint wayIndex, uint tag, uint age)
{
    sets[setIndex][wayIndex].tag = tag;
    sets[setIndex][wayIndex].age = age;
    sets[setIndex][wayIndex].valid = true;
    
    eventCounter[setIndex][wayIndex] = setAccessCounter[setIndex];
    setAccessCounter[setIndex]++;
}

void OptCache::update(uint addr,uint age)
{
    uint temp = addr >> offset;
    uint setIndex = temp & ((1<<setsInPowerOfTwo)-1);
    uint tag = temp >> setsInPowerOfTwo;
    
    int wayIndex = find(setIndex,tag);
    if(wayIndex != -1)
    {
        hit++;
        promotion(setIndex,wayIndex,age);
    }        
    else
    {
        miss++;
        uint victimWayIndex = victimize(setIndex);
        if(sets[setIndex][victimWayIndex].valid)
            eviction(setIndex,victimWayIndex);
        insertion(setIndex,victimWayIndex,tag,age);
    }
    totalAccess++;
}

void OptCache::dumpReuseStat(std::ostream* out,int verbose)
{
    uint total=0,group1=0, group2=0, group3=0, group4=0;
    std::map<uint,std::vector<uint> >::iterator it;
    for(it=reuseCountStat.begin();it!=reuseCountStat.end();++it)
    {
        double mean=0.0, sd=0.0, var=0.0;
        for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            mean += (double)(*i);
        mean = mean/((it->second).size());
        for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            var += (double)((*i)-mean)*((*i)-mean);
        var = var/((it->second).size());
        sd = sqrt(var);
        
        total++;
        if(mean==0&&sd==0)
            group1++;
        else if(mean!=0&&sd==0)
            group2++;
        else if(mean!=0&&(mean-sd)>=0)
            group3++;
        else if(mean!=0&&(mean-sd)<0)
            group4++;        
        
        if(verbose>=1)
            (*out)<<it->first<<" : ["<<(it->second).size()<<"], ["<<mean<<"], ["<<sd<<"]\n";
        
        if(verbose>=2)
        {
            (*out)<<"{\n";
            for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            {
                (*out)<<"\t"<<(*i)<<"\n";
            }
            (*out)<<"}\n\n";
        }
        
    }
    
    (*out)<<"Total distinct block accessed: "<<total<<"\n";
    (*out)<<"Group 1: "<<group1<<" ["<<(double)group1/total*100<<"]\n";
    (*out)<<"Group 2: "<<group2<<" ["<<(double)group2/total*100<<"]\n";
    (*out)<<"Group 3: "<<group3<<" ["<<(double)group3/total*100<<"]\n";
    (*out)<<"Group 4: "<<group4<<" ["<<(double)group4/total*100<<"]\n";
    if (total==(group1+group2+group3+group4))
        (*out)<<"Count matched\n";
    else
        (*out)<<"Count mismatched\n";
    
}

void OptCache::dumpAccessIntervalStat(std::ostream* out,int verbose)
{
    std::map<uint,std::vector<uint> >::iterator it;
    for(it=accessIntervalStat.begin();it!=accessIntervalStat.end();++it)
    {
        double mean=0.0, sd=0.0, var=0.0;
        for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            mean += (double)(*i);
        mean = mean/((it->second).size());
        for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            var += (double)((*i)-mean)*((*i)-mean);
        var = var/((it->second).size());
        sd = sqrt(var);
        
        (*out)<<it->first<<" : ["<<(it->second).size()<<"], ["<<mean<<"], ["<<sd<<"]\n";
        
        if(verbose)
        {
            (*out)<<"{\n";
            for(std::vector<uint>::iterator i=(it->second).begin();i!=(it->second).end();++i)
            {
                (*out)<<"\t"<<(*i)<<"\n";
            }
            (*out)<<"}\n\n";
        }
        
    }
}

