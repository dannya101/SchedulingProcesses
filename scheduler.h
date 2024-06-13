//
// Created by Danny Aguilar on 2/28/24.
//

#ifndef ASSIGNMENT2_SCHEDULER_H
#define ASSIGNMENT2_SCHEDULER_H

#include "ourProcess.h"
#include <vector>
#include <queue>

using namespace std;

class scheduler{
public:
    //defining each of the schedule strategies available for each processes
    //to be used implemented in the .cpp file
    void rr(queue<ourProcess> readyQueue, priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> blockedQueue, int timeq);
    void fcfs(queue<ourProcess> readyQueue,priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> blockedQueue);

};


#endif //ASSIGNMENT2_SCHEDULER_H
