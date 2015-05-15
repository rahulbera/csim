#ifndef LIPCACHE_H
#define	LIPCACHE_H

#include "BaseCache.h"

class LIPCache : public BaseCache
{
    public:
        bool *setIsFull;
        
    public:
        LIPCache(uint,uint,uint);
        ~LIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

class LIPCachev2 : public BaseCache
{
    public:
        
    public:
        LIPCachev2(uint,uint,uint);
        ~LIPCachev2();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

#endif	/* LIPCACHE_H */

