#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <assert.h>
#include <limits.h>
#define B 1
#define KB 1024*B
#define MB 1024*KB
#define BLOCK_SIZE 64*B
using namespace std;

typedef unsigned int uint;

uint log2(uint l)
{
    if(l==1) return 0;
    else return 1+log2(l/2);
}

typedef struct node
{
    uint blockNo;
    uint age;
    node *next;
}node;


node* createNode(int a)
{
    node *temp = (node*)malloc(sizeof(node));
    assert(temp!=NULL);
    temp->blockNo = a;
    temp->age = UINT_MAX;
    temp->next = NULL;
    return temp;
}

void print(node *head,FILE *out)
{
    node *ptr = head;
    while(ptr)
    {
        fwrite((void*)(&(ptr->age)),sizeof(uint),1,out);
        ptr=ptr->next;
    }
}





int main(int argc,char *argv[])
{
    if(argc!=2)
    {
        printf("Usage: %s <benchmark_name>",argv[0]);
        exit(0);
    }
    string fileName = argv[1];
    FILE *fp,*out;
    fp = fopen(string("binaries/").append(fileName).append("_trace.binary").c_str(),"rb");
    assert(fp!=NULL);
    out = fopen(string("preprocess/").append(fileName).append("_opt.binary").c_str(),"wb");
    assert(out!=NULL);

    uint offset = log2(BLOCK_SIZE);

    uint counter=0;
    uint count1=0,count2=0;
    map<uint,node*>ageMap;
    map<uint,node*>::iterator it;
    uint elem,block;
    node *list,*ptr;
    
    while(fread(&elem,sizeof(uint),1,fp) && !feof(fp) && !ferror(fp))
    {
        block = elem >> offset;
        node *temp = createNode(block);
        if(counter==0) 
            list = ptr = temp;
        else
        {
            ptr->next = temp;
            ptr = ptr->next;
	    }
        if((it=ageMap.find(block)) != ageMap.end())
        {
            it->second->age = counter;
            it->second = temp;
        }
        else
        {
            ageMap.insert(std::pair<uint,node*>(block,temp));
        }
        
        counter++;
        count1++;
        if(count1%1000000 == 0)
        {
            printf("Processed: %d\n",(count2+1)*1000000);
            count2++;
            count1=0;
        }
    }
    
    print(list,out);
    
    fclose(fp);
    fclose(out);

    return 0;
}

