#include "SimpleLanguage.h"
#include "simpleMachine.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

/**
    The constructor for the simple language compiler.
    The symbol table is processed in the contstructor as a part of preprocessing
        so that the locations of the machine commands could all be in order
**/
SimpleLanguage::SimpleLanguage(std::string filename)
{
    //Get the input from a file
    std::ifstream file (filename, std::ios::in);

    for (size_t i = 0; i < machineCommands.size(); i = i + 1) //Initializing all elements in the machine commands vector
    {
        machineCommands[i] = 0;
    }

    if (file.is_open())
    {
        std::string instruction; //each line in the file
        int commandIndex = 0; //starting command index
        int memoryIndex = 99; //starting memory index
        int extendedSMLIndex = 100; //starting extended command index for let statements

        while (std::getline(file, instruction)) //Iterating through all of the lines in the file
        {
            if (instruction == "") //Ends if there is an empty line
            {
                return;
            }

            bool skip = false; //boolean controlling skips for errors

            std::vector<std::string> tokens = split(instruction, ' '); //Splitting line by the spaces
            //Syntax Error Checking
            try
            {
                stoi(tokens.at(0)); //Checking to see if tokens.at(0) is a line number
            }
            catch(...)
            {
                errors.push_back("Missing Line Number"); //adds the error of a missing line number to error vector
                skip = true;
            }
            if( tokens.size()==1){ //Checking for empty line numbers
                errors.push_back("Empty Line at "+ tokens.at(0));

                skip=true;

            }

            if( skip == false){ //Checking for other syntax errors within the commands

            if(tokens.at(1) == "input" or tokens.at(1)=="print"){ //Chacking for error in input and print
                if (tokens.size()!= 3){ //If there aren't 3 tokens in the line throw error

                errors.push_back("missing variable or too many variables at "+ tokens.at(0));
                skip = true;

                }
            }
            else if( tokens.at(1) == "goto") //Error checking for goto
            {
                try
                {
                    stoi(tokens.at(2)); //Making sure that goto is going to a line number
                }
                catch (...)
                {
                    errors.push_back("invalid branch at " + tokens.at(0));
                    skip = true;
                }
            }
            else if( tokens.at(1)=="rem"){ //Error checking for rem (nothing needs to be done)

            }
            else if( tokens.at(1)=="end") //Error checking for end
            {
                if (tokens.size() != 2) //If there is extra information other than "[linenumer] end"
                {
                    errors.push_back("invalid use of end at " + tokens.at(0));
                    skip = true;
                }
            }
            else if( tokens.at(1)=="let") //Error checking for let
            {
                //TODO
            }
            else if( tokens.at(1)=="if") //Error checking for if
            {
                //TODO
            }

            else{ //Checking if the command exists or not

            errors.push_back("command does not exist at: " + tokens.at(0));
            skip=true;
            }



            }
            if( skip == false ){ //Setting up the symbol table
            if ((tokens.at(1) != "rem") )
            {

                Symbol sym(tokens.at(0), 'L', commandIndex); //Creating a symbol for the line number
                commandIndex = commandIndex + 1;

                for (Symbol s: symbols) //Making sure that there isn't a duplicate line number
                {
                    if (sym.getSymbol() == s.getSymbol())
                    {
                        string stmnt = sym.getSymbol() + " duplicate line number";
                        errors.push_back(stmnt);
                    }
                }


                symbols.push_back(sym); //adding the linenumber symbol to the symbol table
                int index = std::stoi(tokens.at(0));

                if (tokens.at(1) == "input" or tokens.at(1) == "print") //checking if the command is input or print
                {


                    if(!isalpha(tokens.at(2)[0])){ // checks to seee if the variable is a character take first letter

                    errors.push_back("variable entered must be character at line "+ tokens.at(0));

                    }

                    commands[index] = tokens.at(1) + " " + tokens.at(2); //adding the command at the line number
                    bool createNewVariable = true;

                    for (Symbol s: symbols) //checking to see if a new variable needs to be created
                    {
                        if (s.getType() == 'V') //V is variable type
                        {
                            std::string varName = s.getSymbol();
                            if (tokens.at(2) == varName)
                            {
                                createNewVariable = false;
                            }
                        }
                    }

                    if (createNewVariable == true) //creates new variable if one needs to be created
                    {
                        Symbol symbol(tokens.at(2), 'V', memoryIndex);
                        memoryIndex = memoryIndex - 1;
                        symbols.push_back(symbol); //pushing variable symbol into symbol table
                    }
                }

                if (tokens.at(1) == "goto") //setting up the command for goto
                {
                    commands[index] = tokens.at(1) + " " + tokens.at(2);
                }
                if(tokens.at(1) =="if") //setting up the command for if
                {
                  commands[index] = tokens.at(1) + " " + tokens.at(2)+" "+tokens.at(3) + " " + tokens.at(4)+" "+tokens.at(5) + " " + tokens.at(6);
                  if(tokens.at(3)=="<" or tokens.at(3)==">" or tokens.at(3)=="==" or tokens.at(3)=="!=")
                  {
                  commandIndex=commandIndex+3;
                  if(tokens.at(3)=="!="){
                  commandIndex=commandIndex+3;
                  }
                  }
                  if(tokens.at(3)==">=" or tokens.at(3)=="<=" ){
                  commandIndex=commandIndex+4;
                  }
                }


                if (tokens.at(1) == "end") //sets up the command for end
                {
                    commands[index] = tokens.at(1);
                }

                if (tokens.at(1) == "let") //Let statement
                {
                    commandIndex = commandIndex - 1;
                    string statement = "";

                    commands[index] = "do nothing"; //filler command

                    for (size_t i = 2; i < tokens.size(); i = i + 1) //getting the actual let statement
                    {
                        statement = statement + tokens.at(i);
                    }

                    vector<string> statem = split(statement, '='); //storing value into array without = sign

                    string storedVariable = statem.at(0); //value that let is stored into
                    stack<char> postfix = convertToPostFix(statem.at(1)); //the statement in postfix
                    stack<int> termIndexes; //The indexes for the terms to deal with order

                    int variableIndex;
                    bool needNew = true;
                    for (Symbol variable: symbols) //checking to see if a new variable needs to be created
                    {
                        if (variable.getType() == 'V')
                        {
                            if (variable.getSymbol() == storedVariable)
                            {
                                variableIndex = variable.getLocation();
                                needNew = false;
                            }
                        }
                    }

                    if (needNew == true) //creates a new variable if one needs to be created
                    {
                        Symbol s(storedVariable, 'V', memoryIndex);
                        variableIndex = memoryIndex;
                        memoryIndex = memoryIndex - 1;

                        symbols.push_back(s); //pushes back symbol into the symbol table
                    }

                    while (postfix.size() != 0)
                    {
                        char value = postfix.top(); //getting the top of the stack

                        if (isdigit(value)) //Checking for constants
                        {
                            bool makeNew = true;

                            for (Symbol s: symbols)
                            {
                                if (s.getSymbol() == charToString(value)) //Checking to see if we need new constant
                                {
                                    makeNew = false;

                                    termIndexes.push(s.getLocation()); //pushes the index into the stack
                                }
                            }

                            if (makeNew == true) //Make new constant if it doesn't exist already
                            {
                                Symbol s(charToString(value), 'C', memoryIndex);
                                memoryIndex = memoryIndex - 1;

                                symbols.push_back(s); //pushing back constant into the symbol table

                                termIndexes.push(s.getLocation()); //pushes the index into the stack
                            }

                            Symbol sym(to_string(extendedSMLIndex), 'L', commandIndex); //creating a linenumber symbol with the extended sml index
                            commandIndex = commandIndex + 1;
                            symbols.push_back(sym); //pushing back symbol into the symbol table

                            string h = to_string(termIndexes.top());
                            commands[extendedSMLIndex] = "load " + h; //loading the top of the termindexes stack to the accumulator
                            extendedSMLIndex = extendedSMLIndex + 1; //incrementing extended sml index
                        }


                        else if (isalpha(value)) //Checking to see if it is a variable
                        {
                            bool exists = false;
                            for (Symbol s: symbols) //making sure the variable exists
                            {
                                if (s.getSymbol() == charToString(value))
                                {
                                    exists = true;

                                    termIndexes.push(s.getLocation()); //pushing the index into the indexes stack
                                }
                            }

                            if (exists == false) //create a new variable if one doesn't exist
                            {
                                Symbol s(charToString(value), 'V', memoryIndex);
                                memoryIndex = memoryIndex - 1;
                                symbols.push_back(s);

                                termIndexes.push(s.getLocation()); //pushing the index into the indexes stack
                            }

                            Symbol sym(to_string(extendedSMLIndex), 'L', commandIndex); //creating a new line number to lead the variable into the accumulator
                            commandIndex = commandIndex + 1;
                            symbols.push_back(sym); //pushing the symbol into the symbol table

                            string h = to_string(termIndexes.top()); //creating the command in order to load into the accumulator
                            commands[extendedSMLIndex] = "load " + h;
                            extendedSMLIndex = extendedSMLIndex + 1;
                        }

                        else if (value == '*' or value == '/' or value == '+' or value == '-') //if the top of the postfix stack is an operator
                        {
                            string ind = to_string(termIndexes.top()); //gets the top of the termIndexes stack
                            termIndexes.pop();

                            string firstInd = to_string(termIndexes.top()); //gets the next string in the termIndexes stack
                            termIndexes.pop();

                            Symbol sym(to_string(extendedSMLIndex), 'L', commandIndex); //create a linenumer symbol to load the proper index
                            commandIndex = commandIndex + 1;
                            symbols.push_back(sym); //pushback that symbol into the symbol table

                            commands[extendedSMLIndex] = "load " + firstInd;
                            extendedSMLIndex = extendedSMLIndex + 1;

                            Symbol s(to_string(extendedSMLIndex), 'L', commandIndex); //create a linenumber symbol to evaluate expression
                            commandIndex = commandIndex + 1;
                            symbols.push_back(s);

                            if (value == '+') //for the + operator
                            {
                                commands[extendedSMLIndex] = "add " + ind;
                            }

                            else if (value == '-') //for the - operator
                            {
                                commands[extendedSMLIndex] = "subtract " + ind;
                            }

                            else if (value == '*') //for the * operator
                            {
                                commands[extendedSMLIndex] = "multiply " + ind;
                            }

                            else if (value == '/') //for the / operator
                            {
                                commands[extendedSMLIndex] = "divide " + ind;
                            }

                            extendedSMLIndex = extendedSMLIndex + 1;

                            //Saving value into the variable
                            Symbol si(to_string(extendedSMLIndex), 'L', commandIndex);
                            commandIndex = commandIndex + 1;
                            symbols.push_back(si);

                            commands[extendedSMLIndex] = "saveTo " + to_string(memoryIndex); //creating command to save to a location in memory
                            termIndexes.push(memoryIndex);
                            memoryIndex = memoryIndex - 1;
                            extendedSMLIndex = extendedSMLIndex + 1;
                        }

                        postfix.pop(); //goes on to the next value in postfix
                    }

                    Symbol ss(to_string(extendedSMLIndex), 'L', commandIndex); //saves the whole thing to the desired index
                    commands[extendedSMLIndex] = "saveTo " + to_string(variableIndex);
                    symbols.push_back(ss);

                    extendedSMLIndex = extendedSMLIndex + 1;
                    commandIndex = commandIndex + 1;
                }
            }
            }
        }

    }

    else //checking if the file opened correctly
    {
        std::cerr << "The save file doesn't exist." << std::endl;
    }

    file.close();
}

SimpleLanguage::~SimpleLanguage()
{
    //dtor
}

/**
    This goes through the symbol table and then converts all of it into machine commands
        by reading the commands array and the symbol table
**/
void SimpleLanguage::compile()
{
    for (Symbol sym: symbols) //Iterating through all of thes symbols in the symbol table
    {
        if (sym.getType() == 'L') //if it is a linenumber symbol, then do something...
        {
            int lineNum = sym.getLocation(); //get the command index
            int comIndex = stoi(sym.getSymbol()); //get the linenumber
            std::string command = commands[comIndex]; //get the command that corresponds with that linenumber
            std::vector<std::string> com = split(command, ' '); //splitting the command by its spaces

            if (com.at(0) == "input") //if the command is input
            {
                int index;
                for (Symbol s: symbols) //iterating through the symbol table to find the index associated with the variable
                {
                    if (s.getType() == 'V')
                    {
                        std::string varName = s.getSymbol();
                        if (com[1] == varName)
                        {
                            index = s.getLocation();
                        }
                    }
                }

                int machCom = 1000 + index; //converting to the machine command
                machineCommands[lineNum] = machCom; //adding to the machine commands array
            }

            if (com.at(0) == "print") //command for print
            {
                bool errorornah = false;
                int index;

                for (Symbol s: symbols) //looking to see if variable exists
                {
                    if (s.getType() == 'V')
                    {
                        std::string varName = s.getSymbol();
                        if (com[1] == varName)
                        {
                            index = s.getLocation();
                            errorornah = true;
                        }
                    }
                }

                if (errorornah == true) //if the variable exists
                {
                    int machCom = 1100 + index;
                    machineCommands[lineNum] = machCom;
                }
            }
            bool gotoError= false;
            if (com.at(0) == "goto") //command for goto
            {
                int machCom;

                for (Symbol s: symbols) //finding desired location
                {
                    if (s.getSymbol() == com.at(1))
                    {
                        gotoError = true;
                        machCom = 4000 + s.getLocation();
                    }
                }
                if(gotoError==false){
                errors.push_back("goto error: linenumber does not exist at " + sym.getSymbol());
                }

                machineCommands[lineNum] = machCom;
            }
            if (com.at(0)== "if"){
            int var1Loc; // location for first variable
            int var2Loc; // location in memory for second variable
            bool varTest =false; // tests if variable exists

            for(Symbol s: symbols ) // finds the first variable
            {
            if(s.getSymbol()==com.at(1)){

            var1Loc = s.getLocation();

            varTest =true;
            }

            }
            if(varTest==false)
            {
            // error
            }
            varTest=false;
            for(Symbol s: symbols ) // finds the second variable
            {
            if(s.getSymbol()==com.at(3)){

            var2Loc = s.getLocation();

            varTest =true;
            }

            }
            if(varTest==false)
            {
            // error
            }
           bool LineNumberTest=false; // sees if the location exists
           int branchHere;
            for(Symbol s: symbols ) // finds location in symbol table
            {
            if(s.getSymbol()==com.at(5)){

            branchHere = s.getLocation();

            LineNumberTest =true;
            }

            }
            if(LineNumberTest==false)
            {
            // error
            }
            if(com.at(2)==">"){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum]= 2000+var2Loc;
            machineCommands[lineNum+1]= 3100+var1Loc;
            machineCommands[lineNum+2]= 4100+branchHere-1;
            lineNum=lineNum+2;
            }
            if(com.at(2)=="<"){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum+0]= 2000+var1Loc;
            machineCommands[lineNum+1]= 3100+var2Loc;
            machineCommands[lineNum+2]= 4100+branchHere-1;
            lineNum=lineNum+2;
            }
            if(com.at(2)=="=="){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum+0]= 2000+var2Loc;
            machineCommands[lineNum+1]= 3100+var1Loc;
            machineCommands[lineNum+2]= 4200+branchHere-1;
            lineNum=lineNum+2;
            }
            if(com.at(2)=="!="){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum+0]= 2000+var2Loc;
            machineCommands[lineNum+1]= 3100+var1Loc;
            machineCommands[lineNum+2]= 4100+branchHere-1;
            machineCommands[lineNum+3]= 2000+var1Loc;
            machineCommands[lineNum+4]= 3100+var2Loc;
            machineCommands[lineNum+5]= 4100+branchHere-1;
            lineNum=lineNum+5;
            }
            if(com.at(2)==">="){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum+0]= 2000+var2Loc;
            machineCommands[lineNum+1]= 3100+var1Loc;
            machineCommands[lineNum+2]= 4200+branchHere-1;
            machineCommands[lineNum+3]= 4100+branchHere-1;
            lineNum=lineNum+3;
            }
            if(com.at(2)=="<="){
            // 3100= subtract 4100= branch negative 4200== branch zero 2000 = load
            machineCommands[lineNum+0]= 2000+var1Loc;
            machineCommands[lineNum+1]= 3100+var2Loc;
            machineCommands[lineNum+2]= 4200+branchHere-1;
            machineCommands[lineNum+3]= 4100+branchHere-1;
            lineNum=lineNum+3;
            }
            }

            if (com.at(0) == "load") //command for loading something into the accumulator
            {
                int machCom;

                machCom = 2000 + stoi(com.at(1));

                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "add") //adding a value to the accumulator
            {
                int machCom = 3000 + stoi(com.at(1));

                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "subtract") //subtracting a value from the accumulator
            {
                int machCom = 3100 + stoi(com.at(1));

                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "divide") //dividing a value from the accumulator
            {
                int machCom = 3200 + stoi(com.at(1));

                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "multiply") //multiplying a value to the accumulator
            {
                int machCom = 3300 + stoi(com.at(1));

                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "saveTo") //save value from accumulator into a place in memory
            {
                int machCom = 2100 + stoi(com.at(1));
                machineCommands[lineNum] = machCom;
            }

            if (com.at(0) == "end") //ends the program
            {
                machineCommands[lineNum] = 4300;
            }
        }
    }
}

/**
    This splits a string into tokens with a specific delimiter
    return: a vector of string tokens
**/
std::vector<std::string> SimpleLanguage::split(const std::string &str, char delimitor)
{
    std::vector<std::string> strings;
    std::stringstream ss;
    ss.str(str);
    std::string element;
    while (std::getline(ss, element, delimitor))
    {
        strings.push_back(element);
    }
    return strings;
}

/**
    Prints all of the machine commands
**/
void SimpleLanguage::printMachineCommands()
{
    for (size_t t= 0; t < machineCommands.size(); t = t+1)
    {
        if (machineCommands[t] != 0)
        {
            std::cout << machineCommands[t] << std::endl;
        }
    }

}

/**
    Converts a character to a string
    return: a character converted into a string
**/
string SimpleLanguage::charToString(char c)
{
    stringstream ss;
    string s;

    ss << c;
    ss >> s;
    return s;
}

/**
    Converts a statement in string form into postfix in a stack
    return: a stack of characters for the postfix of a statement
**/
stack<char> SimpleLanguage::convertToPostFix(string infix)
{
    stack<char> operators; //the operators in a statement
    stack<string> operands; //the operands in a statement
    stack<char> postfix; //the actual postfix

    for (size_t i = 0; i < infix.size(); i = i + 1)
    {
        if (isalpha(infix.at(i)) or isdigit(infix.at(i))) //checking if character is a number or a letter - put into operands
        {
            operands.push(charToString(infix.at(i)));
        }

//        if (isdigit(infix.at(i)))
//        {
//            string term = "";
//
//            if (i != 0)
//            {
//                if (isdigit(infix.at(i-1)))
//                {
//                    term = operands.top();
//                    operands.pop();
//                }
//            }
//
//            term = term + charToString(infix.at(i));
//            operands.push(term);
//
//        }

        else if (infix.at(i) == '(') //If it is an open paranthesis
        {
            operators.push(infix.at(i));
        }

        else if (infix.at(i) == ')') //if it is a closed paranthesis
        {
            while (operators.top() != '(')
            {
                string a = operands.top();
                operands.pop();

                string b = operands.top();
                operands.pop();

                string op = charToString(operators.top());
                operators.pop();

                string c = b + a + op;
                operands.push(c);
            }

            operators.pop();
        }

        else if (infix.at(i) == '*' or infix.at(i) == '/') //multiply or divide
        {
            if (operators.size() == 0)
            {
                operators.push(infix.at(i));
            }

            else if (operators.top() == '*' or operators.top() == '/')
            {
                string a = operands.top();
                operands.pop();

                string b = operands.top();
                operands.pop();

                string op = charToString(operators.top());
                operators.pop();

                string c = b + a + op;
                operands.push(c);

                operators.push(infix.at(i));
            }

            else
            {
                operators.push(infix.at(i));
            }
        }

        else if (infix.at(i) == '+' or infix.at(i) == '-') //add or subtract
        {
            if (operators.size() == 0)
            {
                operators.push(infix.at(i));
            }

            else if (operators.top() != '(')
            {
                string a = operands.top();
                operands.pop();

                string b = operands.top();
                operands.pop();

                string op = charToString(operators.top());
                operators.pop();

                string c = b + a + op;
                operands.push(c);

                operators.push(infix.at(i));
            }

            else
            {
                operators.push(infix.at(i));
            }
        }
    }

    while (operators.size() > 0) //putting the remaining operators into the operands stack
    {
        operands.push(charToString(operators.top()));
        operators.pop();
    }

    while (operands.size() > 0) //putting everything in the postfix
    {
        string term = operands.top();

        for (int k = term.size(); k > 0; k = k - 1)
        {
            postfix.push(term.at(k-1));
        }

        operands.pop();
    }

    return postfix;
}

/**
    Runs the program by reading the text file created by the compile method and sending it
        into the simpleMachine.cpp to be ran by the console
**/
void SimpleLanguage::run()
{
    if (errors.size() == 0)
    {
        ofstream textFile("machinecode.txt", ios::out); //creates a text file with all the machine code
        std::array<int, 100> constants; //passes an additional array with the constants preloaded into the assembler

        for (Symbol sym: symbols) //getting all of the constants into the array
        {
            if (sym.getType() == 'C')
            {
                constants[sym.getLocation()] = stoi(sym.getSymbol());
            }
        }

        for (size_t t= 0; t < machineCommands.size(); t = t+1) //putting down all of the machine commands
        {
            if (machineCommands[t] != 0)
            {
                textFile << machineCommands[t] << endl;
            }
        }

        simpleMachine machine("machinecode.txt", constants);
        machine.run();

        textFile.close();
    }

    else //if errors exist, list all of the errors instead of going to the assembler
    {
        cout<<"Errors:"<< endl;
        for (string err: errors)
        {
            cout << err << endl;
        }
    }
}
