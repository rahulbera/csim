#ifndef BIPCACHE_H
#define	BIPCACHE_H

#include "BaseCache.h"

class BIPCache : public BaseCache
{
    public:
        bool *setIsFull;
        uint *throttleCounter;
        uint throttleParam;
        
    public:
        BIPCache(uint,uint,uint,uint);
        ~BIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

class BIPCachev2 : public BaseCache
{
    public:
        uint throttleCounter;
        uint throttleParam;
        
    public:
        BIPCachev2(uint,uint,uint,uint);
        ~BIPCachev2();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

#endif	/* BIPCACHE_H */

