#ifndef HELPER_H
#define	HELPER_H

class Monitor
{
    unsigned int c;
    unsigned int max_count;
    
    public:
        Monitor();
        ~Monitor();
        void set(unsigned int);
        void inc();
        void dec();
        unsigned int value();
        unsigned int max();
};

#endif	/* HELPER_H */

