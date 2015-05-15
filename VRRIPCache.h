#ifndef VRRIPCACHE_H
#define	VRRIPCACHE_H

#include "BaseCache.h"
#include "Helper.h"

class VRRIPCache : public BaseCache
{
    public:
        uint **rrpv;
        uint maxRRPV;
        Monitor sel;
        
    public:
        VRRIPCache(uint,uint,uint,uint,uint);
        ~VRRIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint,bool);
        void update(uint);

};

#endif	/* VRRIPCACHE_H */

