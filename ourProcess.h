//
// Created by Danny Aguilar on 2/28/24.
//
#ifndef ASSIGNMENT2_OURPROCESS_H
#define ASSIGNMENT2_OURPROCESS_H
#include <vector>

class ourProcess {
public:
    //defining all the atrributes needed for each process
    std::vector<int> process;
    int processID;
    int cpuBurst;
    int IOBurst;
    int waitTime;
    int personalCompletionTime;

    //this the default constructor that sets the default values of all attributes of each process
    //besides processID, which is calculated depending on how many process have been created, calculated in main
    ourProcess(){
        cpuBurst = 0;
        IOBurst = 0;
        waitTime = 0;
        personalCompletionTime = 0;
    }
    //this is our comparator struct that is used in the priority queue to manipulate the elements accordingly
    //we compare the first burst in each process and sort it by the lesser burst
    struct correctOrder
    {
        bool operator()(ourProcess first, ourProcess second)
        {
            return first.process[0] > second.process[0];
        }
    };
};


#endif //ASSIGNMENT2_OURPROCESS_H
