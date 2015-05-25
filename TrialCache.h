#ifndef TRIALCACHE_H
#define	TRIALCACHE_H

#include "BaseCache.h"

class TrialCache : public BaseCache
{
    public:
        bool **hint;
        
    public:
         TrialCache(uint,uint,uint);
        ~TrialCache();
        int find(uint,uint);
        void promotion(uint,uint,bool);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint,bool);
        void update(uint,bool);   
};

#endif	/* TRIALCACHE_H */

