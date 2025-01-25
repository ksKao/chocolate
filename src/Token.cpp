#include "Token.h"

#include "Error.h"

std::string Token::getName() const {
	if (typeToStringMap.find(type) == typeToStringMap.end())
		exitWithError("Unknown token type: " + std::to_string((int)type));

	return typeToStringMap.at(type);
}

std::string Token::getTokenName(TokenType type) {
	if (typeToStringMap.find(type) == typeToStringMap.end())
		exitWithError("Unknown token type: " + std::to_string((int)type));

	return typeToStringMap.at(type);
}

const std::unordered_map<TokenType, std::string> Token::typeToStringMap = {
	{TokenType::IDENTIFIER, "identifier"},
	{TokenType::LET, "let"},
	{TokenType::EQUALS, "="},
	{TokenType::PLUS, "+"},
	{TokenType::MINUS, "-"},
	{TokenType::MULTIPLY, "*"},
	{TokenType::DIVIDE, "/"},
	{TokenType::SEMI_COLON, ";"},
	{TokenType::NUMBER, "number"},
	{TokenType::OPEN_PARENTHESIS, "("},
	{TokenType::CLOSE_PARENTHESIS, ")"},
	{TokenType::OPEN_CURLY, "{"},
	{TokenType::CLOSE_CURLY, "}"},
	{TokenType::NULL_, "null"},
	{TokenType::PRINT, "print"},
	{TokenType::END_OF_FILE, "eof"},
};