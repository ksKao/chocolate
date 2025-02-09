#pragma once
#include <string>
#include <unordered_map>

enum class TokenType {
	IDENTIFIER,
	LET,
	EQUALS,
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	SEMI_COLON,
	NUMBER,
	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS,
	OPEN_CURLY,
	CLOSE_CURLY,
	IF,
	TRUE,
	FALSE,
	DOUBLE_EQUALS,
	NOT,
	GREATER_THAN,
	GREATER_THAN_OR_EQUALS_TO,
	LESS_THAN,
	LESS_THAN_OR_EQUALS_TO,
	OR,
	AND,
	ELSE,
	WHILE,
	FOR,
	INCREMENT,
	DECREMENT,
	NULL_,
	PRINT,
	END_OF_FILE,
};

class Token {
  public:
	TokenType type;
	std::string value;
	size_t lineNumber;

  public:
	std::string getName() const;
	static std::string getTokenName(TokenType type);

  private:
	static const std::unordered_map<TokenType, std::string> tokenTypeToStringMap;
};