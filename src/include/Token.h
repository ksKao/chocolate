#pragma once
#include <string>

enum TokenType
{
    IDENTIFIER,
    LET,
    EQUALS,
    SEMI_COLON,
    NUMBER
};

struct Token
{
    TokenType type;
    std::string value;
};