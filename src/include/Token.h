#pragma once
#include <string>
#include <unordered_map>

enum class TokenType
{
    IDENTIFIER,
    LET,
    EQUALS,
    SEMI_COLON,
    NUMBER,
    END_OF_FILE,
};

class Token
{
public:
    TokenType type;
    std::string value;

public:
    std::string getName();
    static std::string getTokenName(TokenType type);

private:
    static const std::unordered_map<TokenType, std::string> typeToStringMap;
};