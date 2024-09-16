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

struct Program : public Node
{
    std::vector<std::unique_ptr<Node>> statements;

    inline std::string getTypeName() const override { return "Program node"; }
    void print(const std::string &indent) const override;
    void generateAssembly() const override;
};

struct VariableDeclarationStatement : public Node
{
    Token identifier;
    std::unique_ptr<Node> value;

    inline std::string getTypeName() const override { return "Variable declaration statement"; }
    void print(const std::string &indent) const override;
    void generateAssembly() const override;
};

struct NumericLiteral : public Node
{
    float value;

    inline std::string getTypeName() const override { return "Numeric literal"; }
    void print(const std::string &indent) const override;
    void generateAssembly() const override;
};

struct NullLiteral : public Node
{
    inline std::string getTypeName() const override { return "Null literal"; }
    void print(const std::string &indent) const override;
    void generateAssembly() const override;
};