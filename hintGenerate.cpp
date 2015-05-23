#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>
#define B 1
#define KB 1024*B
#define MB 1024*KB
#define CACHE_SIZE_SCALE 512*KB
using namespace std;

typedef unsigned int uint;

uint log2(uint n)
{
    if(n==1) return 0;
    else return 1+log2(n/2);
}

string toString(uint n)
{
	ostringstream ss;
	ss<<n;
	return ss.str();
}



int main(int argc, char *argv[])
{
	if(argc!=3 || atoi(argv[2])==0)
	{
		cout<<"Usage: "<<argv[0]<<" <benchmark_name> <mode>"<<endl;
		exit(1);
	}

	int mode = atoi(argv[2]);
	string benchmark = argv[1];

    uint associativity = 16;
    uint blockSize = 64*B;
    uint cacheSize = CACHE_SIZE_SCALE*mode;
    uint noOfSets = cacheSize/(blockSize*associativity);
    uint setsInPowerOfTwo = log2(noOfSets); 
    uint offset = log2(blockSize);

    vector<uint> *setAccessTrace = (vector<uint>*)malloc(sizeof(vector<uint>)*noOfSets);
    assert(setAccessTrace!=NULL);


    string traceFileName = "binaries/";
    traceFileName += benchmark;
    traceFileName += "_trace.binary";

    uint addr,setIndex,blockId;
    uint count1 = 0, count2 = 0;
    FILE *fp = fopen(traceFileName.c_str(),"rb");
    assert(fp!=NULL);

    while(fread(&addr,sizeof(uint),1,fp) && !feof(fp) && !ferror(fp))
    {
    	count1++;
    	blockId = addr >> offset;
    	setIndex = blockId & ((1<<setsInPowerOfTwo)-1);
    	setAccessTrace[setIndex].push_back(blockId);
    	if(count1 == 1000000)
    	{
    		count2++;
    		cout<<"Processed: "<<count2*1000000<<endl;
    		count1 = 0;
    	}
    }


    string dumpFileName = string("dump/").append(benchmark).append(".").append(toString(mode*512)).append("KB.dump");
    ofstream df;
    df.open(dumpFileName.c_str());
    vector<uint>::iterator it;
    uint c;

    for(int i=0;i<noOfSets;++i)
    {
    	c = 0;
    	df<<"================   SET "<<i<<"   ================\n";
    	for(it=setAccessTrace[i].begin();it!=setAccessTrace[i].end();++it)
    	{
    		df<<setw(10)<<(*it);
    		c++;
    		if(c==4)
    		{
    			df<<endl;
    			c=0;
    		}
    	}
    	df<<endl<<endl;
    }

    fclose(fp);
    df.flush();
    df.close();

    return 0;
}