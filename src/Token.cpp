#include "Token.h"

std::string Token::getName() const {
	return typeToStringMap.at(type);
}

std::string Token::getTokenName(TokenType type) {
	return typeToStringMap.at(type);
}

const std::unordered_map<TokenType, std::string> Token::typeToStringMap = {
	{TokenType::IDENTIFIER, "identifier"},
	{TokenType::LET, "let"},
	{TokenType::EQUALS, "="},
	{TokenType::SEMI_COLON, ";"},
	{TokenType::NUMBER, "number"},
	{TokenType::END_OF_FILE, "eof"},
};