// Author: Peiyao Li
// Date:   Jan 8 2023
#pragma once
#include <chrono>
#include <string>


// ¼ÆÊ±Æ÷
class Timer {
    
public:
    void start();
    void end();
    void printTimeCost(std::string task);
private:
    clock_t startTime;
    clock_t endTime;

};