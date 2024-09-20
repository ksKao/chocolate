#include "Lexer.h"

#include <optional>
#include <vector>

#include "Error.h"
#include "Token.h"

const std::vector<Token> Lexer::keywords = {
	{TokenType::LET, "let"}, {TokenType::NULL_, "null"}, {TokenType::PRINT, "print"}};

char Lexer::getChar() const {
	if (src.length() <= i) return '\0';
	return src.at(i);
}

bool Lexer::getIsValidWordChar(char character) const {
	// only alphabets, numbers, and _ are allowed to be in a identifier/keyword
	return isalnum(character) || character == '_';
}

char Lexer::peek() const {
	if (src.length() <= i + 1) return '\0';
	return src.at(i + 1);
}

void Lexer::advance() {
	i++;
}

Token Lexer::parseNumber() {
	std::string value = "";

	// parse until non-number
	while (isdigit(getChar())) {
		value.push_back(getChar());
		advance();
	}

	return {TokenType::NUMBER, value};
}

Token Lexer::parseWord() {
	std::string value = "";

	// only allow alphabets, numbers, and underscores as word
	while (getIsValidWordChar(getChar())) {
		value.push_back(getChar());

		// if next character is not a valid character for a word, means we have
		// already found a word. we cannot detect directly by space, since `let
		// i=10` should still be valid syntax
		if (!getIsValidWordChar(peek())) {
			// in this for loop, we try to match every keyword, if a match is
			// found, means that this word is a keyword, otherwise it is an
			// identifier
			for (const Token &keyword : keywords) {
				// if found keyword, can assign the token
				if (keyword.value == value) {
					advance();
					return keyword;
				}
			}

			// reach this case means there are no matching keywords, i.e., the
			// word is an identifier
			advance();
			return {TokenType::IDENTIFIER, value};
		}

		// if no words are matched, just continue to the next character
		advance();
	}

	exitWithError("Invalid symbol encountered: " + value);
	return {};
}

Token Lexer::parseSymbol() {
	char character = getChar();
	Token token;

	switch (character) {
		case '=':
			token = {TokenType::EQUALS, "="};
			break;
		case ';':
			token = {TokenType::SEMI_COLON, ";"};
			break;
		default:
			exitWithError("Invalid symbol encountered: " + character);
			break;
	}

	advance();
	return token;
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> tokens;
	// reset index, in case need to use the same lexer object to tokenize multiple source code
	i = 0;

	while (getChar() != '\0') {
		char character = getChar();

		if (isdigit(character)) {
			tokens.push_back(parseNumber());
		} else if (isalpha(character)) {
			tokens.push_back(parseWord());
		} else if (iswspace(character)) {  // ignore white space
			advance();
			continue;
		} else {
			tokens.push_back(parseSymbol());
		}
	}

	tokens.push_back({TokenType::END_OF_FILE, "EOF"});
	return tokens;
}