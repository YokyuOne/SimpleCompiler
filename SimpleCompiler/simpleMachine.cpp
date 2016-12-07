/**
    simpleMachine.cpp
    Author: Yohan Moon and Greylan Smoak
    Date: 10/14/16
    A simple machine that runs the instructions found in a simple machine
**/

#include "simpleMachine.h"
#include <array>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <iostream>

/**
   Constructor for the Simple Machine
   input: the filename for the text file containing the instructions
**/
simpleMachine::simpleMachine(std::string filename, std::array<int, 100> constants)
{
    //Get the input from a file
    std::ifstream file (filename, std::ios::in);

    if (file.is_open()) //Reads all of the instructions in the file and puts it in the instruction vector
    {
        int instruction;

        while (file >> instruction)
        {
            commands.push_back(instruction);
        }
    }
    else //checking if the file opened correctly
    {
        std::cerr << "The save file doesn't exist." << std::endl;
    }

    file.close();

    accumulator = 0;
    instrIndex = 0;
    memIndex = 0;
    instrReg = 0;

    memory = constants;
}

simpleMachine::~simpleMachine()
{
    //dtor
}

/**
    Runs the simple machine
**/
void simpleMachine::run()
{
    bool cont = true;

    while (cont == true)
    {
        instrReg = commands[instrIndex];
        int command = instrReg / 100;
        memIndex = commands[instrIndex] % 100;

        if (((instrReg / 10000) == 0) and (instrReg != 10000))
        {
            switch(command)
            {
                case 10: //Getting user input
                {
                    int usrInput = 0;
                    std::cout << "Enter a number: ";
                    std::cin >> usrInput;

//                    if (usrInput / 100 == 0)
//                    {
//                        memory[memIndex] = usrInput;
//                    }
//                    else
//                    {
//                        std::cerr << "You entered a value that was too large." << std::endl;
//                        cont = false;
//                    }

                    memory[memIndex] = usrInput;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 11: //writes a word on to the monitor screen
                {
                    int value = memory[memIndex];
                    std::cout << value << std::endl;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 20: //loads a word from memory into the accumulator
                {
                    int value = memory[memIndex];
                    accumulator = value;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 21: //stores the value in the accumulator into memory
                {
                    memory[memIndex] = accumulator;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 30: //adds a value from memory into the accumulator
                {
                    int value = memory[memIndex];
                    accumulator = accumulator + value;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 31: //subtracts a value from memory from the accumulator
                {
                    int value = memory[memIndex];
                    accumulator = accumulator - value;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 32: //divides a value from memory from the accumulator
                {
                    int value = memory[memIndex];
                    accumulator = accumulator / value;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 33: //multiplies a value from memory to the accumulator
                {
                    int value = memory[memIndex];
                    accumulator = accumulator * value;

                    instrIndex = instrIndex + 1;
                    break;
                }

                case 40: //branches to a specific point in memory
                {
                    instrIndex = memIndex;
                    break;
                }

                case 41: //branches if the accumulator is negative
                {
                    if (accumulator < 0)
                    {
                        instrIndex = memIndex;
                    }
                    else
                    {
                        instrIndex = instrIndex + 1;
                    }

                    break;
                }

                case 42: //branches is the accumulator is zero
                {
                    if (accumulator == 0)
                    {
                        instrIndex = memIndex;
                    }
                    else
                    {
                        instrIndex = instrIndex + 1;
                    }

                    break;
                }

                case 43: //ends the program
                {
                    cont = false;
                    break;
                }

                default: //if the user enters a command that is invalid
                {
                    cont = false;
                    std::cerr << "You entered an invalid command." << std::endl;
                    break;
                }
            }
        }

        else //catches if the instruction is bigger than four digits
        {
            std::cerr << "There was a value that was too large." << std::endl;
            cont = false;
        }
    }
}
