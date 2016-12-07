#ifndef SYMBOL_H
#define SYMBOL_H
#include <string>

class Symbol
{
    public:
        explicit Symbol(std::string sym, char type, int loc);
        virtual ~Symbol();
        void setSymbol(std::string sym);
        void setType(char type);
        void setLocation(int loc);
        std::string getSymbol();
        char getType();
        int getLocation();
    protected:
    private:
        std::string symbol;
        char symType;
        int location;
};

#endif // SYMBOL_H
