#ifndef OPTCACHE_H
#define	OPTCACHE_H

#include "BaseCache.h"
#include <map>
#include <vector>
#include <cmath>

class OptCache : public BaseCache
{
    public:
        std::map<uint,std::vector<uint> > reuseCountStat;
        std::map<uint,std::vector<uint> > accessIntervalStat;
        uint *setAccessCounter;
        uint **eventCounter;
        
        
    public:
        OptCache(uint,uint,uint);
        ~OptCache();
        int find(uint,uint);
        void promotion(uint,uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint,uint);
        void update(uint,uint);
        void dumpReuseStat(std::ostream*,int);
        void dumpAccessIntervalStat(std::ostream*,int);

};

#endif	/* OPTCACHE_H */

