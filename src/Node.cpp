#include "Node.h"
#include "Generator.h"

#include <iostream>
#include "Error.h"

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

void VariableDeclarationStatement::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << ": name = " << identifier.value << std::endl;

    value->print(indent + "\t");
}

void VariableDeclarationStatement::generateAssembly() const
{
    // check if variable has already been declared
    if (Generator::getVariable(identifier.value).has_value())
        exitWithError("Identifier " + identifier.value + " already exists.");

    value->generateAssembly();
    Generator::addVariable(identifier.value);
    Generator::incrementStack();
    Generator::appendOutput("movss DWORD [rsp], xmm0"); // move the value stored in xmm into the top stack pointer
}

void NumericLiteral::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << ": value = " << value << std::endl;
}

void NumericLiteral::generateAssembly() const
{
    std::string dataName = Generator::getDataName(std::to_string(value));
    Generator::appendOutput("movss xmm0, DWORD [rel " + dataName + "]");
}

void NullLiteral::print(const std::string &indent) const
{
    std::cout << indent << getTypeName() << std::endl;
}

void NullLiteral::generateAssembly() const
{
    Generator::appendOutput("movss xmm0, DWORD [rel null]");
}