#pragma once
#include <string>
#include <unordered_map>

enum class TokenType {
	IDENTIFIER,
	LET,
	EQUALS,
	PLUS,
	MINUS,
	SEMI_COLON,
	NUMBER,
	NULL_,
	PRINT,
	END_OF_FILE,
};

class Token {
  public:
	TokenType type;
	std::string value;

  public:
	std::string getName() const;
	static std::string getTokenName(TokenType type);

  private:
	static const std::unordered_map<TokenType, std::string> typeToStringMap;
};