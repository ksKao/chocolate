#pragma once
#include "Token.h"

#include <string>
#include <vector>
#include <memory>

struct Node
{
    virtual std::string getTypeName() const = 0;
    virtual void print(const std::string &indent) const = 0;
    virtual void generateAssembly() const = 0;
};