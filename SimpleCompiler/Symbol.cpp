#include "Symbol.h"
#include <string>

Symbol::Symbol(std::string sym, char type, int loc)
{
    symbol = sym;
    symType = type;
    location = loc;
}

Symbol::~Symbol()
{
    //dtor
}

void Symbol::setSymbol(std::string sym)
{
    symbol = sym;
}

void Symbol::setType(char type)
{
    symType = type;
}

void Symbol::setLocation(int loc)
{
    location = loc;
}

std::string Symbol::getSymbol()
{
    return symbol;
}

char Symbol::getType()
{
    return symType;
}

int Symbol::getLocation()
{
    return location;
}
