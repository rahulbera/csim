#ifndef LRUCACHE_H
#define	LRUCACHE_H

#include "BaseCache.h"

class LRUCache : public BaseCache
{
    public:
        
        
    public:
        LRUCache(uint,uint,uint);
        ~LRUCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint);
        void update(uint);

};

#endif	/* LRUCACHE_H */

