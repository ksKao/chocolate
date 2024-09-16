#pragma once

#include "Node.h"

struct Program : public Node
{
    std::vector<std::unique_ptr<Node>> statements;

    inline std::string getTypeName() const override { return "Program node"; }
    void print(const std::string &indent) const override;
    void generateAssembly() const override;
};
