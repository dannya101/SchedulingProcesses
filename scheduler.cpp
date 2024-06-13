//
// Created by Danny Aguilar on 2/28/24.
//
#include <iostream>
#include <fstream>
#include <string>
#include "scheduler.h"
#include "log.h"
#include "ourProcess.h"
#include <queue>

using namespace std;

void scheduler::rr(queue<ourProcess> readyQueue ,priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> blockedQueue, int timeq) {
    ExecutionStopReasonType stop;
    int completionTime = 0;
    priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> temporaryBQ;
    queue<ourProcess> completionQueue;
    int i = 0;
    //this is the while loop which runs keeps checking the status of both the ready queue and the blocked queue where if both were empty,
    //this means all the processes have been completed
    while (!readyQueue.empty() || !blockedQueue.empty()) {
        //store the first element in the ready queue as we can update its values and position accordingly
        ourProcess currentReadyProcess = readyQueue.front();
        int decrementer = 0;
        int pushToBlocked = 0;
        int pushBackRdy = 0;
        if (!readyQueue.empty()) {
            //this is the if statement which would check if the cpu burst has expires the time quantum
            //if it does expire we are updating the current cpu burst, executed cpu burst, and total time elapsed with the specified time quantum
            if (currentReadyProcess.process[i] > timeq) {
                currentReadyProcess.process[i] -= timeq;
                currentReadyProcess.cpuBurst += timeq;
                stop = QUANTUM_EXPIRED;
                completionTime += timeq;
                //this prints our current status of cpu burst execution what process is being executed at this time, the cpu burst, io burst,
                //total elapsed time, and the reason for the stopping this time during this time the quantum hs expired
                log_cpuburst_execution(currentReadyProcess.processID, currentReadyProcess.cpuBurst,
                                       currentReadyProcess.IOBurst, completionTime, stop);
                //we then pop the object from the ready queue as we have this stored in a local variable
                //then we set the values of pushBackToRdy accordingly to specify that this process will need to be added later into the ready queue again
                //we also specify the decrementer as the time quantum in the case we need to adjust certain IO bursts in processes in the blocked queue
                readyQueue.pop();
                pushBackRdy = 1;
                decrementer = timeq;

            } else {
                //else if the cpu burst does expire the time quantum then this means the full cpu burst will be executed and the process will be moving into the IO burst state
                //if it does expire we are updating the current cpu burst, executed cpu burst, and total time elapsed with the specified current cpu burst in the process
                //we are also setting the decrementer to the current cpu burst in the case we have to adjust IO bursts of processes in the blocked queue
                //we then delete the cpu burst from the process and move into the IO state
                currentReadyProcess.cpuBurst += currentReadyProcess.process[0];
                stop = ENTER_IO;
                completionTime += currentReadyProcess.process[0];
                decrementer = currentReadyProcess.process[0];
                currentReadyProcess.process.erase(currentReadyProcess.process.begin());
                //though after this deletion of the cpu burst and the process is empty we know we have completed the process
                //in this case we update the cpu burst of the first process in the ready queue as well as the total elapsed time it was being processed for
                //we then push into a new queue which would show the order of when each process was able to finish
                //lastly we pop the process from the ready queue
                if (currentReadyProcess.process.empty()) {
                    stop = COMPLETED;
                    readyQueue.front().cpuBurst += readyQueue.front().process[0];
                    readyQueue.front().personalCompletionTime += completionTime;
                    log_cpuburst_execution(readyQueue.front().processID, readyQueue.front().cpuBurst,
                                           readyQueue.front().IOBurst, completionTime, stop);
                    completionQueue.push(readyQueue.front());
                    readyQueue.pop();
                }
                //if the process does not finish then we mark the process to be moved to the blocked queue later in the algorithm
                // after we finish adjusting the current IO bursts
                //we pop the process from the ready queue to remove it from the ready queue
                else {
                    log_cpuburst_execution(currentReadyProcess.processID, currentReadyProcess.cpuBurst,
                                           currentReadyProcess.IOBurst, completionTime, stop);
                    pushToBlocked = 1;
                    readyQueue.pop();
                }
            }
            //we check now if we currently have processes in the blocked queue to update them accordingly
            //we are first going to create an instance variable to retrieve the top process of the blocked queue to adjust the attributes
            // we pop the value from the queue then take the IO burst of the process and subtract with the decrementer we saved before
            //according to what the result is of that operation we adjust its executed IO burst and update its current IO bursts
            if (!blockedQueue.empty()) {
                int bQueSize = blockedQueue.size();
                int flag = 0;
                for (int iteration = 0; iteration < bQueSize; iteration++) {
                    ourProcess topOfBlocked = blockedQueue.top();
                    blockedQueue.pop();
                    int manipulatedBurst = topOfBlocked.process[i] - decrementer;
                    if (manipulatedBurst < 0) {
                        //if the manipulated burst is negative this means the decrementer was more than the current IO Burst
                        //because of this we instead add whatever the value was in the current IO burst instead as the other operation would result in a negative number which is not a valid burst
                        //we then erase the burst from the process as we know we have completed the IO burst
                        //after we move it back to the readyQueue
                        topOfBlocked.IOBurst += topOfBlocked.process[0];
                        topOfBlocked.process.erase(topOfBlocked.process.begin());
                        readyQueue.push(topOfBlocked);
                        flag = 1;
                    } else if (manipulatedBurst > 0) {
                        //else if the manipulated burst is greater than 0 this means the cpu burst has not completed the IO burst of a proess in the blocked queue
                        //add how much we decremented the IO burst to the executed IO burst and set new value  manipulated as the IO burst in the process
                        //lastly we push this object with new attributes to a temporary blocked queue to hold our updated values
                        topOfBlocked.IOBurst += decrementer;
                        topOfBlocked.process[0] = manipulatedBurst;
                        temporaryBQ.push(topOfBlocked);
                        flag = 0;
                    } else {
                        //this case is to check if the manipulated burst is 0
                        //if it is, perform the same operations as if the IO burst would be negative
                        topOfBlocked.IOBurst += topOfBlocked.process[i];
                        topOfBlocked.process.erase(topOfBlocked.process.begin());
                        readyQueue.push(topOfBlocked);
                        flag = 1;
                    }
                }
                //we do this to have a correctly sorted blocked queue
                //if we put the updated values at the end of the blocked queue then they would jump back to the front of the queue
                //so we swap the temporary blocked queue with our original blocked queue and delete all elements from temporary queue
                if (flag == 0) {
                    temporaryBQ.swap(blockedQueue);
                    if (!temporaryBQ.empty()) {
                        while (!temporaryBQ.empty()) {
                            temporaryBQ.pop();
                        }
                    }
                }
            }
            //this would be executed when the current cpu burst has been completed and it needs to be added to the end of the blocked queue
            //this occurs after all adjustments to current blocked queue
            if(pushToBlocked == 1){
                blockedQueue.push(currentReadyProcess);
            }
            //this would also be executed after all the adjustments of the blocked queue
            //but this executes if the time quantum expires and the process goes to the end of the ready queue
            if(pushBackRdy == 1){
                readyQueue.push(currentReadyProcess);
            }
        }
        //this would execute when all the processes are in the blocked queue
        //we first the top process in the blocked queue and run its IO burst while incrementing the total elapsed time
        //then iterate through the rest of the blocked queue adjusting each IO burst in each process with the first IO burst executed
        if (readyQueue.empty() && !blockedQueue.empty()) {
            ourProcess getDecrement = blockedQueue.top();
            decrementer = getDecrement.process[0];
            completionTime += decrementer;
            int bQueSize = blockedQueue.size();
            for (int iteration = 0; iteration < bQueSize; iteration++) {
                ourProcess topOfBlocked = blockedQueue.top();
                blockedQueue.pop();
                topOfBlocked.IOBurst += decrementer;
                topOfBlocked.process[0] -= decrementer;

                //if the IO burst becomes negative or zero then the IO burst has finished and move it back to the ready queue
                if(topOfBlocked.process[0] <= 0){
                    topOfBlocked.process.erase(topOfBlocked.process.begin());
                    readyQueue.push(topOfBlocked);
                }
                //if it still has more IO burst to run then push the process to our temporary queue to keep the order
                else{
                    temporaryBQ.push(topOfBlocked);
                }
            }
            //again we do this switching to preserve the order of each process
            temporaryBQ.swap(blockedQueue);
            if (!temporaryBQ.empty()) {
                while (!temporaryBQ.empty()) {
                    temporaryBQ.pop();
                }
            }
        }
    }
    //finally after all the process are eliminated from both queues
    //we print each processes requirements specified by the log process completion file
    //we all calculate the wait time of each process from how long they spent in the ready queue by
    //manipulating the total elapsed time equation
    while(!completionQueue.empty()) {
        ourProcess temporaryVec = completionQueue.front();
        temporaryVec.waitTime = temporaryVec.personalCompletionTime - temporaryVec.cpuBurst - temporaryVec.IOBurst;
        log_process_completion(temporaryVec.processID, temporaryVec.personalCompletionTime, temporaryVec.waitTime);
        completionQueue.pop();
    }
}
void scheduler::fcfs(queue<ourProcess> readyQueue, priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> blockedQueue) {
    ExecutionStopReasonType stop;
    int completionTime = 0;
    priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> temporaryBQ;
    queue<ourProcess> completionQueue;
    int i = 0;
    //we loop until there are no processes in both queues
    while (!readyQueue.empty() || !blockedQueue.empty()) {
        //this instance of ourProcess represents the head process in the ready queue
        //that is about to be executed
        ourProcess currentReadyProcess = readyQueue.front();
        int decrementer = 0;
        int pushToBlocked = 0;
        //if there is a process in the ready queue
        //we do not need to check for any time quantum so completely run the current CPU burst
        //then update the specified total time, the decrementer which would be the whole CPU burst, and erase the first CPU burst from the vector
        //as we are representing the entering of the IO burst of this current process
        if (!readyQueue.empty()) {
            currentReadyProcess.cpuBurst += currentReadyProcess.process[0];
            stop = ENTER_IO;
            completionTime += currentReadyProcess.process[0];
            decrementer = currentReadyProcess.process[0];
            currentReadyProcess.process.erase(currentReadyProcess.process.begin());
            //though if this causes the process to be empty we know that this process has been completed
            //we then update the current cpu burst time and completion time according to the previous cpu burst deleted
            //then we preserve the order of completion by pushing the process into a comletion queue
            if (currentReadyProcess.process.empty()) {
                stop = COMPLETED;
                readyQueue.front().cpuBurst += readyQueue.front().process[0];
                readyQueue.front().personalCompletionTime += completionTime;
                log_cpuburst_execution(readyQueue.front().processID, readyQueue.front().cpuBurst,readyQueue.front().IOBurst, completionTime, stop);
                completionQueue.push(readyQueue.front());
                readyQueue.pop();
            //if the process has not been completed we mark as being pushed to the blocked queue
            //after the operations performed on the blocked queue currently
            } else {
                log_cpuburst_execution(currentReadyProcess.processID, currentReadyProcess.cpuBurst,
                                       currentReadyProcess.IOBurst, completionTime, stop);
                pushToBlocked = 1;
                readyQueue.pop();
            }
        }
        //if there is something in the blocked queue we need to perform specified cpu burst operations to each IO burst
        if (!blockedQueue.empty()) {
            int bQueSize = blockedQueue.size();
            int flag = 0;
            //this is where we iterate through each of the processes in the blocked queue
            //we then update each process' executed IO bursts and current IO burst according to
            //how much time the current IO burst contains
            for (int iteration = 0; iteration < bQueSize; iteration++) {
                ourProcess topOfBlocked = blockedQueue.top();
                blockedQueue.pop();
                int holdManipElement = topOfBlocked.process[i] - decrementer;
                if (holdManipElement < 0) {
                    //executed if there is less IO burst than decrementer from the cpu burst
                    //this represents the process has finished its IO burst and needs to be moved to the ready queue
                    topOfBlocked.IOBurst += topOfBlocked.process[0];
                    topOfBlocked.process.erase(topOfBlocked.process.begin());
                    readyQueue.push(topOfBlocked);
                    flag = 1;
                } else if (holdManipElement > 0) {
                    //executed if there is more IO burst than the decrementer from the cpu burst
                    //this represents the process has not finished yet so update remaining value
                    //we push this process to our temporary blocked queue to not adjust the current blocked queue we are adjusting
                    topOfBlocked.IOBurst += decrementer;
                    topOfBlocked.process[0] = holdManipElement;
                    temporaryBQ.push(topOfBlocked);
                    flag = 0;
                } else {
                    //this is executed if the IO burst is equal to the decrementer
                    //this also represents that the IO burst of the process has finished and needs to be placed at the end of the ready queue
                    topOfBlocked.IOBurst += topOfBlocked.process[i];
                    topOfBlocked.process.erase(topOfBlocked.process.begin());
                    readyQueue.push(topOfBlocked);
                    flag = 1;
                }
            }
            //we then execute this if there has been an input to this temporary blocked queue
            //we swap the values of the temporary queue to our original blocked queue
            if (flag == 0) {
                temporaryBQ.swap(blockedQueue);
                if (!temporaryBQ.empty()) {
                    while (!temporaryBQ.empty()) {
                        temporaryBQ.pop();
                    }
                }
            }
        }
        //this is executed if the cpu burst just executed of the process has finished its cpu burst
        if(pushToBlocked == 1){
            blockedQueue.push(currentReadyProcess);
        }
        //we execute this statement if all the processes are in the blocked queue
        //we create the decrementer in this case with the top process' IO burst of the blocked queue
        //we then increment the total time with the IO burst
        if (readyQueue.empty() && !blockedQueue.empty()) {
            ourProcess getDecrement = blockedQueue.top();
            decrementer = getDecrement.process[0];
            completionTime += decrementer;
            int bQueSize = blockedQueue.size();
            //we then iterate through each of the processes and update their IO burst accordingly from the first IO burst
            for (int iteration = 0; iteration < bQueSize; iteration++) {
                ourProcess topOfBlocked = blockedQueue.top();
                blockedQueue.pop();
                topOfBlocked.IOBurst += decrementer;
                topOfBlocked.process[0] -= decrementer;
                //this if statement represents that the IO burst has been completed and needs to be moved to the ready queue
                if(topOfBlocked.process[0] <= 0){
                    topOfBlocked.process.erase(topOfBlocked.process.begin());
                    readyQueue.push(topOfBlocked);
                }
                //if it has not completed its IO burst push to the temporary queue to preserve order of original blocked queue
                else{
                    temporaryBQ.push(topOfBlocked);
                }
            }
            //we then swap the temporary queue with the original blocked queue
            //with the preserved order
            temporaryBQ.swap(blockedQueue);
            if (!temporaryBQ.empty()) {
                while (!temporaryBQ.empty()) {
                    temporaryBQ.pop();
                }
            }
        }
    }
    //after all the processes have finished in the ready queue and there are no processes being blocked
    //we print each processes completion time, wait time, and attributes in order of completion
    //the wait time is calculated from the equation of how to calculate completion time
    while(!completionQueue.empty()) {
        ourProcess temporaryVec = completionQueue.front();
        temporaryVec.waitTime = temporaryVec.personalCompletionTime - temporaryVec.cpuBurst - temporaryVec.IOBurst;
        log_process_completion(temporaryVec.processID, temporaryVec.personalCompletionTime, temporaryVec.waitTime);
        completionQueue.pop();
    }
}