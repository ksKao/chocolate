#pragma once
#include <string>
#include <vector>

#include "Token.h"

class Lexer {
  public:
	Lexer(const std::string &src) : src(src) {}
	static const std::vector<Token> keywords;  // all the reserved keywords, e.g. let, return...
	std::vector<Token> tokenize();

  private:
	size_t i = 0;		   // current index
	std::string src = "";  // source code

  private:
	void advance();
	char getChar() const;
	// check to see if the character passed in is a valid character in an identifier/keyword
	bool getIsValidWordChar(char character) const;
	char peek() const;

	// parsing
	Token parseNumber();
	Token parseWord();	// used to either parse identifier or keyword
	Token parseSymbol();
};