#include "Generator.h"
#include "AST/Program.h"

#include <iostream>
#include <memory>

void Program::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << std::endl;

    for (int i = 0; i < statements.size(); i++)
    {
        statements.at(i)->print("\t");
    }
}

void Program::generateAssembly() const
{
    // initialize stack frame
    Generator::appendOutput("push rbp");
    Generator::appendOutput("mov rbp, rsp");

    for (const std::unique_ptr<Node> &statement : statements)
    {
        statement->generateAssembly();
    }
}