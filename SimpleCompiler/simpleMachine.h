/**
    simpleMachine.h
    Author: Yohan Moon and Greylan Smoak
    Date: 10/14/16
    This is the header file for the simple machine
**/

#ifndef SIMPLEMACHINE_H
#define SIMPLEMACHINE_H
#include <array>
#include <vector>

class simpleMachine
{
    public:
        explicit simpleMachine(std::string filename, std::array<int, 100> constants); //constructor
        virtual ~simpleMachine(); //destructor
        void run(); //runs the machine with the processes
    protected:
    private:
        int accumulator; //the accumulator
        int instrIndex; //the index of the instruction vector
        int memIndex; //the index of the memory array
        int instrReg; //the current instruction being processed
        std::vector<int> commands; //the vector for the instructions
        std::array<int, 100> memory; //the memory, holding 99 slots for values
};

#endif // SIMPLEMACHINE_H
