#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <cstring>
#include <unistd.h>
#include "ourProcess.h"
#include "scheduler.h"
#include <cstdlib>


using namespace std;

int main(int argc, char* argv[]) {
    char* file = nullptr; // setting file char pointer to nullptr so we do not generate a garbage value
    char* getStrategy = "fcfs"; //we are specifying the default strategy we are going to use if no strategy is specified
    int timeQuantum = 2; // we are specifying the default quantum value 2
    int Option = 1;

    /*If the option has an argument, optarg is set to point to the
    argument associated with the option, here we are looping to retrieve the optional arguments in the command line.*/
    while ((Option = getopt(argc, argv, "s:q:")) != -1) {
        switch (Option) {
            case 's':
                getStrategy = optarg; //optarg points to whatever follows the char 's'
                break;
            case 'q':
                timeQuantum = atoi(optarg); //optarg points to whatever follows the char 'q'
                break;
            default:
                exit(1);
        }
    }
    // here we are checking and retrieving the one mandatory argument specified in the command line
    int idx = optind;
    if (idx < argc) {
        file = argv[idx];
    }

    //here we are checking to determine if the time quantum is valid in our case, which is anything greater than 0
    if(timeQuantum <= 0){
        cout << "Time quantum must be a number and bigger than 0" << endl;
        exit(1);
    }
    //here we are reading in the file and we are checking if the file is valid
    ifstream istream(file);
    if(!istream.is_open()){
        cout << "Unable to open <<" << file << ">>" << endl;
        exit(1);
    }
    // we are creating a queue to represent our ready queue which holds objects of ourProcess which would hold attributes related to each process
    queue<ourProcess> readyQueue;
    string line;
    int incrementID = 0;
    //the while loops on every line in the file we are reading from and are storing each line to a string variable called line
    //we then check every element in the string using another loop and determine if it is space char or not
    //if it is not a space character we then check if the number is positive and if it is we add it to a vector holding the valid integers
    //if it is a space character move to next char in string
    while (getline(istream, line)){
        ourProcess theProcess;
        vector<int> extractNums;
        theProcess.process.reserve(line.length());
        for(int i = 0; i < line.length(); i++) {
            if(line[i] != ' ') {
                if (line[i] - '0' <= 0) {
                    cout << "A burst number must be bigger than 0" << endl;
                    exit(1);
                }
                extractNums.push_back(line[i] - '0');
            }
        }
        //after we finish the first loop we move to a second loop where we check how many bursts are in the vector
        // if there are an odd number of bursts in the vector extract nums then push the vector into the class variable proccess in ourProcess object
        //if the amount of bursts in the vector are even then exit the code as this is not valid
        //once the vector is pushed into an object's class variable then push the objects into a queue which would store the process in the ready queue
        for(int i = 0; i < extractNums.size(); i++){
            if(extractNums.size() % 2 != 1){
                cout << "There must be an odd number of bursts for each process" << endl;
                exit(1);
            }
            theProcess.process.push_back(extractNums[i]);
        }
        //we are also setting a unique ID to each process to distinguish between each process
        theProcess.processID = incrementID;
        incrementID++;
        readyQueue.push(theProcess);
        // we then loop on this algorithm again to get the next line of the file and perform the same operations until there are no lines to work with
    }
    //we then create a priority queue to represent the blocked queue to pass into our method
    //this priority queue would also be holding ourProcess objects and the priority in this queue would be
    //to sort the objects by their vectors, specifically the first element in each vector
    //we then create our own comparator called the correct order which sorts the elements added to the priority queue by the lowest first element value in each vector
    priority_queue<ourProcess, vector<ourProcess>, ourProcess::correctOrder> blockedQueue;

    //we create a temporary queue, representing the ready queue, so we could iterate through the queue
    //by popping through the queue we are able to iterate and print through the whole queue
    queue <ourProcess> temp = readyQueue;
    while(!temp.empty()) {
        vector<int> temporaryVector = temp.front().process;
        for(int i : temporaryVector) {
            cout << i << " ";
        }
        cout << endl;
        temp.pop();
    }
    //we then create an instance of the scheduler class to be able to access both schedule techniques, round robin and first come first serve
    //we then compare the two strings and if the two strings are identical run the round robin technique otherwise run the default case first come first serve
    scheduler runMethod;
    if(strcmp(getStrategy, "rr") == 0)
    {
        runMethod.rr(readyQueue, blockedQueue, timeQuantum);
    }
    else
    {
        runMethod.fcfs(readyQueue, blockedQueue);
    }

    return 0;
}
