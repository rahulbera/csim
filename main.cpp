#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <fstream>
#include <cassert>
#include "LRUCache.h"
#include "LIPCache.h"
#include "BIPCache.h"
#include "DIPCache.h"
#include "NRUCache.h"
#include "SRRIPCache.h"
#include "VRRIPCache.h"
#include "OptCache.h"
#include "DRRIPCache.h"
#include "TrialCache.h"
#define B 1
#define KB 1024*B
#define MB 1024*KB
#define VERBOSE 0
#define OPT_VERBOSE 0
#define CACHE_SIZE_SCALE 512*KB
#define SEPARATE(out,s) (out<<"*************   "<<s<<"   **************\n") 

using namespace std;

void testCache();
string toString(uint);

int main(int argc, char** argv) 
{
    if(argc!=3 || atoi(argv[2])==0)
    {
        cout<<"Usage: "<<argv[0]<<" <benchmark_name> <mode>\n";
        exit(0);
    }
    int mode = atoi(argv[2]);
    string benchmark = argv[1];
    
    string binFilePath = "binaries/";
    binFilePath += benchmark;
    binFilePath += "_trace.binary";
    
    string preprocessFilePath = "preprocess/";
    preprocessFilePath += benchmark;
    preprocessFilePath += "_opt.binary";
    
    string hintFilePath = "hints/";
    hintFilePath += benchmark;
    hintFilePath += "_hint.binary";
        
    string reuseStatFilePath = "result/";
    reuseStatFilePath += benchmark;
    reuseStatFilePath += "_reuseStat.txt";
    
    
    LRUCache lruCache                 = LRUCache(16,64*B,CACHE_SIZE_SCALE*mode);
    //LIPCachev2 lipCache             = LIPCachev2(16,64*B,CACHE_SIZE_SCALE*mode);
    //BIPCachev2 bipCache16           = BIPCachev2(16,64*B,CACHE_SIZE_SCALE*mode,16);
    //BIPCachev2 bipCache32           = BIPCachev2(16,64*B,CACHE_SIZE_SCALE*mode,32);
    //BIPCachev2 bipCache64           = BIPCachev2(16,64*B,CACHE_SIZE_SCALE*mode,64);
    //DIPCache dipCache8            = DIPCache(8,64*B,CACHE_SIZE_SCALE*mode,32,8);
    //DIPCache dipCache10             = DIPCache(16,64*B,CACHE_SIZE_SCALE*mode,32,10);
    //DIPCache dipCache12           = DIPCache(8,64*B,CACHE_SIZE_SCALE*mode,32,12);
    //NRUCache nruCache               = NRUCache(16,64*B,CACHE_SIZE_SCALE*mode);
    //SRRIPCache srripCache2          = SRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,2);
    //SRRIPCache srripCache3          = SRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,3);
    //SRRIPCache srripCache4          = SRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,4);
    //SRRIPCache srripCache5          = SRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,5);
    //DRRIPCache drripCache2          = DRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,2,32,10);
    //DRRIPCache drripCache3          = DRRIPCache(16,64*B,CACHE_SIZE_SCALE*mode,3,32,10);
    //SRRIPCachev2 srripCachev2_2   = SRRIPCachev2(8,64*B,CACHE_SIZE_SCALE*mode,2);
    //SRRIPCachev2 srripCachev2_3   = SRRIPCachev2(8,64*B,CACHE_SIZE_SCALE*mode,3);
    //SRRIPCachev3 srripCachev3_3   = SRRIPCachev3(8,64*B,CACHE_SIZE_SCALE*mode,3);
    //VRRIPCache vrripCache8        = VRRIPCache(8,64*B,CACHE_SIZE_SCALE*mode,3,8);
    //VRRIPCache vrripCache10       = VRRIPCache(8,64*B,CACHE_SIZE_SCALE*mode,3,10);
    //VRRIPCache vrripCache12       = VRRIPCache(8,64*B,CACHE_SIZE_SCALE*mode,3,12);
    //SRRIPCache srripCache4        = SRRIPCache(8,64*B,CACHE_SIZE_SCALE*mode,4);
    //OptCache optCache               = OptCache(16,64*B,CACHE_SIZE_SCALE*mode);
    TrialCache trialCache             = TrialCache(16,64*B,CACHE_SIZE_SCALE*mode);
    
    
    FILE *fp,*opt,*hint;    
    uint addr,age;
    bool isLive;
    ulong count1 = 0, count2 = 0;
    
    fp = fopen(binFilePath.c_str(),"rb");
    assert(fp!=NULL);
    //opt = fopen(preprocessFilePath.c_str(),"rb");
    //assert(opt!=NULL);
    hint = fopen(hintFilePath.c_str(),"rb");
    assert(hint!=NULL);
    
    
    
    while(  fread(&addr,sizeof(uint),1,fp) && !feof(fp) && !ferror(fp) /*&&
            fread(&age,sizeof(uint),1,opt) && !feof(opt) && !ferror(opt)*/ &&
            fread(&isLive,sizeof(bool),1,hint) && !feof(hint) && !ferror(hint))
    {
        count1++;
        lruCache.update(addr);
        //lipCache.update(addr);
        //bipCache16.update(addr);
        //bipCache32.update(addr);
        //bipCache64.update(addr);
        //dipCache8.update(addr);
        //dipCache10.update(addr);
        //dipCache12.update(addr);
        //nruCache.update(addr);
        //srripCache2.update(addr);
        //srripCache3.update(addr);
        //srripCache4.update(addr);
        //srripCache5.update(addr);
        //drripCache2.update(addr);
        //drripCache3.update(addr);
        //srripCachev2_2.update(addr);
        //srripCachev2_3.update(addr);
        //srripCachev3_3.update(addr);
        //vrripCache8.update(addr);
        //vrripCache10.update(addr);
        //vrripCache12.update(addr);
        //srripCache4.update(addr);
        //optCache.update(addr,age);
        trialCache.update(addr,isLive);
        
        if(count1==1000000)
        {
            cout<<"Processed: "<<(count2+1)*1000000<<endl;
            count2++;
            count1=0;
        }
    }
    
    
    ofstream df;
    
    df.open(string("dump/").append(benchmark).append(".").append(toString(512*mode)).append("KB.dump").c_str());
    assert(df!=NULL);
    
    SEPARATE(df,"LRU CACHE");
    lruCache.dump(&df,VERBOSE);
    df<<endl<<endl;
    //SEPARATE(df,"LIP CACHE");
    //lipCache.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"BIP_16 CACHE");
    //bipCache16.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"BIP_32 CACHE");
    //bipCache32.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"BIP_64 CACHE");
    //bipCache64.dump(&df,VERBOSE);
    //df<<endl<<endl;*/
    //SEPARATE(df,"DIP_32_8 CACHE");
    //dipCache8.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"DIP_32_10 CACHE");
    //dipCache10.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"DIP_32_12 CACHE");
    //dipCache12.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"NRU CACHE");
    //nruCache.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIP_2bit CACHE");
    //srripCache2.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIP_3bit CACHE");
    //srripCache3.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIP_4bit CACHE");
    //srripCache4.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIP_5bit CACHE");
    //srripCache5.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"DRRIP_2bit CACHE");
    //drripCache2.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"DRRIP_3bit CACHE");
    //drripCache3.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIPv2_2bit CACHE");
    //srripCachev2_2.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIPv2_3bit CACHE");
    //srripCachev2_3.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIPv3_3bit CACHE");
    //srripCachev3_3.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"VRRIP_3bit CACHE");
    //vrripCache8.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"VRRIP_3bit CACHE");
    //vrripCache10.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"VRRIP_3bit CACHE");
    //vrripCache12.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"SRRIP_4bit CACHE");
    //srripCache4.dump(&df,VERBOSE);
    //df<<endl<<endl;
    //SEPARATE(df,"OPT CACHE");
    //optCache.dump(&df,VERBOSE);
    //df<<endl<<endl;
    SEPARATE(df,"TRIAL CACHE");
    trialCache.dump(&df,VERBOSE);
    df<<endl<<endl;
    
    
    
    df.flush();    
    df.close();    
    
    fclose(fp);
    //fclose(opt);
    fclose(hint);
    
    //testCache();
    
    return 0;
}

void testCache()
{
    LIPCache L2_Cache = LIPCache(4,16*B,1*KB);
    uint addr1 = 0xa1ff;
    uint addr2 = 0xa2ff;
    uint addr3 = 0xa3ff;
    uint addr4 = 0xa4ff;
    uint addr5 = 0xa5ff;
    uint addr6 = 0xa6ff;
    uint addr7 = 0xa7ff;
    uint addr8 = 0xa8ff;
    
    L2_Cache.update(addr1);
    L2_Cache.update(addr2);
    L2_Cache.update(addr3);
    L2_Cache.update(addr4);
    L2_Cache.update(addr5);
    L2_Cache.update(addr1);
    L2_Cache.update(addr2);
    
    ofstream df;
    df.open("test.dump");
    
    L2_Cache.dump(&df,1);
    
    df.flush();
    
    df.close();
    return;
}

string toString(uint n)
{
    ostringstream ss;
    ss<<n;
    return ss.str();
}

