#ifndef SIMPLELANGUAGE_H
#define SIMPLELANGUAGE_H
#include "Symbol.h"
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <cstdlib>


class SimpleLanguage
{
    public:
        explicit SimpleLanguage(std::string filename);
        virtual ~SimpleLanguage();
        void printMachineCommands();
        void compile();
        std::vector<std::string> split(const std::string &str, char delimitor);
        std::string charToString(char c);
        std::stack<char> convertToPostFix(std::string infix);
        void run();
    protected:
    private:
        std::array<std::string, 1000> commands;
        std::vector<Symbol> symbols;
        std::array<int, 1000> machineCommands;
        std::vector<std::string> errors;
};
#endif // SIMPLELANGUAGE_H
