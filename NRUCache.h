#ifndef NRUCACHE_H
#define	NRUCACHE_H

#include "BaseCache.h"

class NRUCache : public BaseCache
{
    public:
        bool **nruBit;
        
    public:
        NRUCache(uint,uint,uint);
        ~NRUCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

#endif	/* NRUCACHE_H */

