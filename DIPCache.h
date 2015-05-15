#ifndef DIPCACHE_H
#define	DIPCACHE_H

#include "BaseCache.h"
#include "Helper.h"

class DIPCache : public BaseCache
{
    public:
        uint throttleCounter;
        uint throttleParam;
        Monitor psel;
        
    public:
        DIPCache(uint,uint,uint,uint,uint);
        ~DIPCache();
        int find(uint,uint);
        void promotion(uint,uint);
        uint victimize(uint);
        void eviction(uint,uint);
        void insertion(uint,uint,uint,bool);
        void update(uint);

};

#endif	/* DIPCACHE_H */

