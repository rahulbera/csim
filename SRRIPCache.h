#ifndef SRRIPCACHE_H
#define	SRRIPCACHE_H

#include "BaseCache.h"

class SRRIPCache : public BaseCache
{
    public:
        uint **rrpv;
        uint maxRRPV;
        
    public:
        SRRIPCache(uint,uint,uint,uint);
        ~SRRIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

class SRRIPCachev2 : public BaseCache
{
    public:
        uint **rrpv;
        uint maxRRPV;
        
    public:
        SRRIPCachev2(uint,uint,uint,uint);
        ~SRRIPCachev2();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

class SRRIPCachev3 : public BaseCache
{
    public:
        uint **rrpv;
        uint maxRRPV;
        
        
    public:
        SRRIPCachev3(uint,uint,uint,uint);
        ~SRRIPCachev3();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

#endif	/* SRRIPCACHE_H */

