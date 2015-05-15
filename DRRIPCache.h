#ifndef DRRIPCACHE_H
#define	DRRIPCACHE_H

#include "BaseCache.h"
#include "Helper.h"

class DRRIPCache : public BaseCache
{
    public:
        uint **rrpv;
        uint maxRRPV;
        uint throttleCounter;
        uint throttleParam;
        Monitor psel;
        
    public:
        DRRIPCache(uint,uint,uint,uint,uint,uint);
        ~DRRIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint,bool);
        void update(uint);

};

#endif	/* DRRIPCACHE_H */

