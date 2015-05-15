#include "BaseCache.h"


CacheLine::CacheLine(uint b)
{
	size = b;
	tag=0;
	age=0;
	valid=false;
	reuseCount = 0;
}

uint BaseCache::log2(uint n)
{
    if(n==1) return 0;
    else return (1+log2(n/2));
}

BaseCache::BaseCache(uint a,uint b,uint c)
{
    associativity=a;
    noOfSets = c/(a*b);
    setsInPowerOfTwo = log2(noOfSets);
    offset = log2(b);	
    sets = (CacheLine **)malloc(noOfSets*sizeof(CacheLine*));
    for(uint i=0;i<noOfSets;++i)
        sets[i] = (CacheLine*)malloc(associativity*sizeof(CacheLine));
    for(uint i=0;i<noOfSets;++i)
    {
        for(uint j=0;j<associativity;++j)
            sets[i][j] = CacheLine(b);
    }
    
    totalAccess = hit = miss = 0;
    for(uint i=0;i<16;++i) reuseCountBucket[i] = 0;
}

BaseCache::~BaseCache()
{
    free(sets);
}

void BaseCache::dump(std::ostream *out,int verbose)
{
    *out<<"Total accesses: "<<totalAccess<<"\n";
    *out<<"Hit: "<<hit<<" ["<<std::setprecision(5)<<(float)hit/totalAccess*100<<"]\n";
    *out<<"Miss: "<<miss<<" ["<<std::setprecision(5)<<(float)miss/totalAccess*100<<"]\n\n";
    *out<<">>Reuse count stats:\n";
    uint total = 0;
    for(uint i=0;i<16;++i) total+=reuseCountBucket[i];
    for(uint i=0;i<16;++i)
        *out<<std::setw(2)<<i<<": "<<std::setprecision(5)<<(float)reuseCountBucket[i]/total*100<<"\n";
    
    if(verbose==1)
    {
        for(uint i=0;i<noOfSets;++i)
        {
            *out<<"::::::::::::::: SET "<<std::setbase(10)<<i<<" ::::::::::::::::\n";
            for(uint j=0;j<associativity;++j)
            {             
                *out<<std::setw(15)<<std::setbase(16)<<sets[i][j].tag
                        <<std::setw(5)<<sets[i][j].valid
                        <<std::setw(10)<<sets[i][j].age<<"\n";
            }
            *out<<"\n";
        }
    }
}



