#include "Lexer.h"

#include <optional>
#include <vector>

#include "Error.h"
#include "Token.h"

const std::vector<Token> Lexer::keywords = {
	{TokenType::LET, "let"},   {TokenType::NULL_, "null"},	{TokenType::PRINT, "print"},
	{TokenType::IF, "if"},	   {TokenType::TRUE, "true"},	{TokenType::FALSE, "false"},
	{TokenType::ELSE, "else"}, {TokenType::WHILE, "while"}, {TokenType::FOR, "for"},
};

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
	if (getChar() == '\n') currentLineNumber++;
	i++;
}

Token Lexer::parseNumber() {
	std::string value = "";

	// parse until non-number
	while (isdigit(getChar())) {
		value.push_back(getChar());
		advance();
	}

	return {TokenType::NUMBER, value, currentLineNumber};
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
					return {keyword.type, keyword.value, currentLineNumber};
				}
			}

			// reach this case means there are no matching keywords, i.e., the
			// word is an identifier
			advance();
			return {TokenType::IDENTIFIER, value, currentLineNumber};
		}

		// if no words are matched, just continue to the next character
		advance();
	}

	Error::abortWithLineNumber("Invalid symbol encountered: " + value, currentLineNumber);
	return {};
}

Token Lexer::parseSymbol() {
	char character = getChar();
	Token token;

	switch (character) {
		case '=':
			if (peek() == '=') {
				token = {TokenType::DOUBLE_EQUALS, "=="};
				advance();
			} else {
				token = {TokenType::EQUALS, "="};
			}
			break;
		case '+':
			token = {TokenType::PLUS, "+"};
			break;
		case '-':
			token = {TokenType::MINUS, "-"};
			break;
		case '*':
			token = {TokenType::MULTIPLY, "*"};
			break;
		case '/':
			token = {TokenType::DIVIDE, "/"};
			break;
		case ';':
			token = {TokenType::SEMI_COLON, ";"};
			break;
		case '(':
			token = {TokenType::OPEN_PARENTHESIS, "("};
			break;
		case ')':
			token = {TokenType::CLOSE_PARENTHESIS, ")"};
			break;
		case '{':
			token = {TokenType::OPEN_CURLY, "{"};
			break;
		case '}':
			token = {TokenType::CLOSE_CURLY, "}"};
			break;
		case '!':
			token = {TokenType::NOT, "!"};
			break;
		case '>': {
			if (peek() == '=') {
				token = {TokenType::GREATER_THAN_OR_EQUALS_TO, ">="};
				advance();
			} else {
				token = {TokenType::GREATER_THAN, ">"};
			}
			break;
		}
		case '<': {
			if (peek() == '=') {
				token = {TokenType::LESS_THAN_OR_EQUALS_TO, "<="};
				advance();
			} else {
				token = {TokenType::LESS_THAN, "<"};
			}
			break;
		}
		case '|': {
			if (peek() == '|') {
				token = {TokenType::OR, "||"};
				advance();
			} else {
				Error::abortWithLineNumber("Invalid symbol encountered: |", currentLineNumber);
			}
			break;
		}
		case '&': {
			if (peek() == '&') {
				token = {TokenType::AND, "&&"};
				advance();
			} else {
				Error::abortWithLineNumber("Invalid symbol encountered: &", currentLineNumber);
			}
			break;
		}
		default:
			std::string errorMsg("Invalid symbol encountered: ");
			errorMsg.push_back(character);
			Error::abortWithLineNumber(errorMsg, currentLineNumber);
			break;
	}

	advance();
	token.lineNumber = currentLineNumber;
	return token;
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> tokens;
	// reset index, in case need to use the same lexer object to tokenize multiple source code
	i = 0;
	currentLineNumber = 1;

	while (getChar() != '\0') {
		char character = getChar();

		if (isdigit(character)) {
			tokens.emplace_back(parseNumber());
		} else if (isalpha(character)) {
			tokens.emplace_back(parseWord());
		} else if (iswspace(character)) {  // ignore white space
			advance();
			continue;
		} else if (character == '/' && peek() == '/') {
			advance();
			while (getChar() != '\n' && getChar() != '\0') advance();
		} else {
			tokens.emplace_back(parseSymbol());
		}
	}

	tokens.push_back({TokenType::END_OF_FILE, "EOF", currentLineNumber});
	return tokens;
}