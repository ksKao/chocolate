#include "Token.h"

#include "Error.h"

std::string Token::getName() const {
	if (tokenTypeToStringMap.find(type) == tokenTypeToStringMap.end())
		Error::abort("Unknown token type: " + std::to_string((int)type));

	return tokenTypeToStringMap.at(type);
}

std::string Token::getTokenName(TokenType type) {
	if (tokenTypeToStringMap.find(type) == tokenTypeToStringMap.end())
		Error::abort("Unknown token type: " + std::to_string((int)type));

	return tokenTypeToStringMap.at(type);
}

const std::unordered_map<TokenType, std::string> Token::tokenTypeToStringMap = {
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
	{TokenType::IF, "if"},
	{TokenType::OPEN_CURLY, "{"},
	{TokenType::CLOSE_CURLY, "}"},
	{TokenType::TRUE, "true"},
	{TokenType::FALSE, "false"},
	{TokenType::DOUBLE_EQUALS, "=="},
	{TokenType::NOT, "!"},
	{TokenType::GREATER_THAN, ">"},
	{TokenType::GREATER_THAN_OR_EQUALS_TO, ">="},
	{TokenType::LESS_THAN, "<"},
	{TokenType::LESS_THAN_OR_EQUALS_TO, "<="},
	{TokenType::OR, "||"},
	{TokenType::AND, "&&"},
	{TokenType::NULL_, "null"},
	{TokenType::PRINT, "print"},
	{TokenType::END_OF_FILE, "eof"},
};