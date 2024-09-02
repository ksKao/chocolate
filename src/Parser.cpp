#include "Error.h"
#include "Parser.h"
#include "Token.h"

/// @brief Consume the token at the current location and advance index
/// @return The token consumed
Token Parser::eat(std::optional<TokenType> expectedType = std::nullopt)
{
    if (tokens.size() == 0)
    {
        exitWithError("No tokens found.");
        return Token();
    }

    Token token = tokens.at(index);

    if (expectedType != std::nullopt && token.type != expectedType)
    {
        exitWithError("Invalid token type, expected: " + Token::getTokenName(expectedType.value()) + ", found: " + token.getName());
        return Token();
    }

    index++;
    return token;
}

Token Parser::getToken()
{
    return tokens.at(index);
}

bool Parser::isEof()
{
    return getToken().type == TokenType::END_OF_FILE;
}

std::unique_ptr<Node> Parser::parseExpression()
{
    return parsePrimaryExpression();
}

std::unique_ptr<Node> Parser::parseStatement()
{
    switch (getToken().type)
    {
    case TokenType::LET:
        return parseVariableDeclarationStatement();
    default:
        return parseExpression();
    }
}

std::unique_ptr<VariableDeclarationStatement> Parser::parseVariableDeclarationStatement()
{
    eat(); // eat the let keyword

    Token idenfifier = eat(TokenType::IDENTIFIER); // expects idenfifier

    std::unique_ptr<VariableDeclarationStatement> variableDeclarationStatement = std::make_unique<VariableDeclarationStatement>();
    variableDeclarationStatement->identifier = idenfifier;
    variableDeclarationStatement->value = nullptr;

    Token nextToken = eat();

    // something like `let x;`;
    if (nextToken.type == TokenType::SEMI_COLON)
        return variableDeclarationStatement;

    // if did not return, then means it should be something like `let x = 10;`
    if (nextToken.type == TokenType::EQUALS)
    {
        variableDeclarationStatement->value = parseExpression(); // override the default nullptr assigned initially
        eat(TokenType::SEMI_COLON);
        return variableDeclarationStatement;
    }

    exitWithError("Unexpected token in variable declaration after " + idenfifier.value + ". Expected ';' or '=', received: " + nextToken.getName());
    return nullptr;
}

std::unique_ptr<Node> Parser::parsePrimaryExpression()
{
    switch (getToken().type)
    {
    case TokenType::NUMBER:
    {
        std::unique_ptr<NumericLiteral> numericLiteral = std::make_unique<NumericLiteral>();
        numericLiteral->value = std::stof(eat(TokenType::NUMBER).value);
        return numericLiteral;
    }
    default:
    {
        exitWithError("Unexpected token encountered: " + getToken().getName());
        return nullptr;
    }
    }
}

Program Parser::parse()
{
    Program program;

    while (!isEof())
        program.statements.push_back(parseStatement());

    return program;
}