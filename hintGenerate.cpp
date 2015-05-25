#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
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

typedef struct node
{
    bool hint;
    struct node *next;
}node;

typedef struct buff
{
    bool valid;
    uint blockId;
    uint age;
    node *ref;
}bufferNode;

node* createNode(bool hint)
{
    node *temp = (node*)malloc(sizeof(node));
    assert(temp!=NULL);
    temp->hint = hint;
    temp->next = NULL;
    return temp;
}

void dumpLL(node *head,FILE *fp)
{
    while(head)
    {
        fwrite((void*)(&(head->hint)),sizeof(bool),1,fp);
        head = head->next;
    }
}




int main(int argc, char *argv[])
{
    if(argc!=4 || atoi(argv[2])==0 || atoi(argv[3])==0)
    {
    	cout<<"Usage: "<<argv[0]<<" <benchmark_name> <mode> <lookahead_scale>"<<endl;
    exit(1);
    }

    uint mode = atoi(argv[2]);
    string benchmark = argv[1];
    uint lookaheadScale = atoi(argv[3]); 


    uint associativity = 16;
    uint blockSize = 64*B;
    uint cacheSize = CACHE_SIZE_SCALE*mode;
    uint noOfSets = cacheSize/(blockSize*associativity);
    uint setsInPowerOfTwo = log2(noOfSets); 
    uint offset = log2(blockSize);
    uint bufferLength = associativity * lookaheadScale;

    vector<uint> *setAccessTrace = (vector<uint>*)malloc(sizeof(vector<uint>)*noOfSets);
    assert(setAccessTrace!=NULL);
    bufferNode **buffer = (bufferNode**)malloc(sizeof(bufferNode*)*noOfSets);
    assert(buffer!=NULL);
    for(int i=0;i<noOfSets;++i)
        buffer[i] = (bufferNode*)malloc(sizeof(bufferNode)*bufferLength);
    for(int i=0;i<noOfSets;++i)
    {
        for(int j=0;j<bufferLength;++j)
            buffer[i][j].valid = false;
    }
    node *list = NULL, *ptr;


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

        node *temp = createNode(false);
        if(list == NULL)
        {
            list = temp;
            ptr = list;
        }
        else
        {
            ptr->next = temp;
            ptr = ptr->next;
        }
            

        bool found = false;
        int i=0;
        for(i=0;i<bufferLength;++i)
        {
            if(buffer[setIndex][i].valid && buffer[setIndex][i].blockId == blockId)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            buffer[setIndex][i].ref->hint = true;
            buffer[setIndex][i].ref = temp;
            buffer[setIndex][i].age = 0;
            for(int k=0;k<bufferLength&&k!=i;++k) buffer[setIndex][i].age++;
        }
        else
        {
            int victimIdx;
            for(int i=0;i<bufferLength;++i)
            {
                if(!buffer[setIndex][i].valid)
                {
                    victimIdx = i;
                    buffer[setIndex][i].age = 0;
                    buffer[setIndex][i].blockId = blockId;
                    buffer[setIndex][i].ref = temp;
                    buffer[setIndex][i].valid = true;
                }
            }
            for(int i=0;i<bufferLength&&i!=victimIdx;++i)
            {
                if(buffer[setIndex][i].valid)
                {
                    buffer[setIndex][i].age++;
                    if(buffer[setIndex][i].age >= bufferLength)
                       buffer[setIndex][i].valid = false;
                }               
            }
        }

    	if(count1 == 1000000)
    	{
            count2++;
            cout<<"Processed: "<<count2*1000000<<endl;
            count1 = 0;
    	}
    }
    
    cout<<"Total processed: "<<(count2*1000000+count1)<<endl;


    string dumpFileName = string("dump/").append(benchmark).append(".").append(toString(mode*512)).append("KB.dump");
    ofstream df;
    df.open(dumpFileName.c_str());
    vector<uint>::iterator it;
    uint c;
    
    string hintFileName = string("hints/").append(benchmark).append("_hint.binary");
    FILE *out;
    out = fopen(hintFileName.c_str(),"wb");
    assert(out!=NULL);

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
    
    dumpLL(list,out);

    fclose(out);
    fclose(fp);
    df.flush();
    df.close();

    return 0;
}