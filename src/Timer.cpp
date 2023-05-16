// Author: Peiyao Li
// Date:   Jan 8 2023
#include<iostream>
#include "Timer.h"

void Timer::start() { 
    this->startTime = clock();
}

void Timer::end() { 
    this->endTime = clock();
}

void Timer::printTimeCost(std::string task)
{
    double timecost = endTime - startTime; 
    std::cout<<task<<" time cost: " << timecost << std::endl;
}