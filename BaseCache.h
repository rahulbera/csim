#ifndef BASECACHE_H
#define	BASECACHE_H

#include <cstdlib>
#include <iomanip>
#include <iostream>

typedef unsigned int uint;
typedef unsigned long int counter;


class CacheLine
{
    public:
        uint    size;
        uint    tag;
        bool    valid;
        uint    age;
        uint    reuseCount;
		
    public:
	CacheLine(uint);
};


class BaseCache
{
    public:
        uint associativity;
        uint noOfSets;
        uint setsInPowerOfTwo;
        uint offset;
        CacheLine **sets;
        
        uint reuseCountBucket[16];
        counter totalAccess;
        counter hit,miss;
    
    public:
        BaseCache(uint,uint,uint);
        ~BaseCache();
        virtual int find(uint,uint){}
        virtual void promotion(uint,uint){}
        virtual uint victimize(uint){}
        virtual void eviction(uint,uint){}
        virtual void insertion(uint,uint,uint){}
        virtual void update(uint){}
        
        void dump(std::ostream*,int);
        
    private:
        uint log2(uint);
};

#endif	/* BASECACHE_H */

