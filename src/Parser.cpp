#include "Parser.h"

#include "AST/BinaryExpression.h"
#include "AST/Identifier.h"
#include "AST/NullLiteral.h"
#include "AST/NumericLiteral.h"
#include "AST/PrintStatement.h"
#include "Error.h"
#include "Token.h"

/// @brief Consume the token at the current location and advance index
/// @return The token consumed
Token Parser::eat(std::optional<TokenType> expectedType = std::nullopt) {
	if (tokens.size() == 0) {
		exitWithError("No tokens found.");
		return Token();
	}

	Token token = tokens.at(index);

	if (expectedType != std::nullopt && token.type != expectedType) {
		exitWithError("Invalid token type, expected: " + Token::getTokenName(expectedType.value()) +
					  ", found: " + token.getName());
		return Token();
	}

	index++;
	return token;
}

Token Parser::getToken() const {
	return tokens.at(index);
}

bool Parser::isEof() const {
	return getToken().type == TokenType::END_OF_FILE;
}

std::unique_ptr<Node> Parser::parseExpression() {
	return parseAdditiveExpression();
}

std::unique_ptr<Node> Parser::parseStatement() {
	switch (getToken().type) {
		case TokenType::LET:
			return parseVariableDeclarationStatement();
		case TokenType::PRINT:
			return parsePrintStatement();
		default:
			return parseExpression();
	}
}

std::unique_ptr<Node> Parser::parseAdditiveExpression() {
	std::unique_ptr<Node> left = parsePrimaryExpression();

	// use while loop here to handle chaining multiple operators, e.g. 1 + 2 + 3
	while (getToken().type == TokenType::PLUS || getToken().type == TokenType::MINUS) {
		Token op = eat();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parsePrimaryExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Node> Parser::parsePrimaryExpression() {
	switch (getToken().type) {
		case TokenType::IDENTIFIER: {
			std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();
			identifier->name = eat(TokenType::IDENTIFIER).value;
			return identifier;
		}
		case TokenType::NUMBER: {
			std::unique_ptr<NumericLiteral> numericLiteral = std::make_unique<NumericLiteral>();
			numericLiteral->value = std::stof(eat(TokenType::NUMBER).value);
			return numericLiteral;
		}
		default: {
			exitWithError("Unexpected token encountered: " + getToken().getName());
			return nullptr;
		}
	}
}

std::unique_ptr<Node> Parser::parseVariableDeclarationStatement() {
	eat(TokenType::LET);  // eat the let keyword

	Token idenfifier = eat(TokenType::IDENTIFIER);	// expects idenfifier

	std::unique_ptr<VariableDeclarationStatement> variableDeclarationStatement =
		std::make_unique<VariableDeclarationStatement>();
	variableDeclarationStatement->identifier = idenfifier;

	Token nextToken = eat();

	// something like `let x;`;
	// in this case, default it to let x = null;
	if (nextToken.type == TokenType::SEMI_COLON) {
		variableDeclarationStatement->value = std::make_unique<NullLiteral>();
		return variableDeclarationStatement;
	}

	// if did not return, then means it should be something like `let x = 10;`
	if (nextToken.type == TokenType::EQUALS) {
		variableDeclarationStatement->value = parseExpression();
		eat(TokenType::SEMI_COLON);
		return variableDeclarationStatement;
	}

	exitWithError("Unexpected token in variable declaration after " + idenfifier.value +
				  ". Expected ';' or '=', received: " + nextToken.getName());
	return nullptr;
}

std::unique_ptr<Node> Parser::parsePrintStatement() {
	eat(TokenType::PRINT);	// eat the print token

	std::unique_ptr<PrintStatement> printStatement = std::make_unique<PrintStatement>();
	printStatement->value = parseExpression();

	eat(TokenType::SEMI_COLON);

	return printStatement;
}

Program Parser::parse() {
	Program program;

	while (!isEof()) program.statements.push_back(parseStatement());

	return program;
}