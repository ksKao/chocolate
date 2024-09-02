#include "Node.h"

#include <iostream>

void Program::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << std::endl;

    for (int i = 0; i < statements.size(); i++)
    {
        statements.at(i)->print("\t");
    }
}

void VariableDeclarationStatement::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << ": name = " << identifier.value << std::endl;

    if (value != nullptr)
        value->print(indent + "\t");
    else
        std::cout << indent << "\tNo value." << std::endl;
}

void NumericLiteral::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << ": value = " << value << std::endl;
}