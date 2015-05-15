#include "Helper.h"

Monitor::Monitor() 
{
    
}

Monitor::~Monitor()
{
    
}

void Monitor::set(unsigned int n)
{
    max_count = (1<<n)-1;
    c = 1<<(n-1);
}

void Monitor::inc()
{
    if(c<max_count)
        c++;
}

void Monitor::dec()
{
    if(c>0)
        c--;
}

unsigned int Monitor::value()
{
    return c;
}

unsigned int Monitor::max()
{
    return max_count;
}

