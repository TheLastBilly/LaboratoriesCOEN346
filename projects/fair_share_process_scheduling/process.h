#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>

static u_int32_t uniqueId = 0;

class Process
{
private:
    u_int32_t id;
    u_int32_t readyTime;
    u_int32_t serviceTime;

public:
    Process(u_int32_t, u_int32_t); //constructor

    u_int32_t getId() { return id; }
};

#endif