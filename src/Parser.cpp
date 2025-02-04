#include "Parser.h"

#include <algorithm>

#include "AST/AssignmentStatement.h"
#include "AST/BinaryExpression.h"
#include "AST/BooleanLiteral.h"
#include "AST/Identifier.h"
#include "AST/NullLiteral.h"
#include "AST/NumericLiteral.h"
#include "AST/PrintStatement.h"
#include "AST/Scope.h"
#include "Error.h"
#include "Token.h"

/// @brief Consume the token at the current location and advance index
/// @return The token consumed
Token Parser::eat(std::optional<TokenType> expectedType = std::nullopt) {
	if (tokens.size() == 0) {
		Error::abort("No tokens found.");
		return Token();
	}

	Token token = tokens.at(index);

	if (expectedType != std::nullopt && token.type != expectedType) {
		Error::abortWithLineNumber("Invalid token type, expected: `" + Token::getTokenName(expectedType.value()) +
									   "`, found: `" + token.getName() + "`",
								   token.lineNumber);
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

std::unique_ptr<Expression> Parser::parseExpression() {
	return parseOrExpression();
}

std::unique_ptr<Node> Parser::parseStatement() {
	switch (getToken().type) {
		case TokenType::LET:
			return parseVariableDeclarationStatement();
		case TokenType::PRINT:
			return parsePrintStatement();
		case TokenType::OPEN_CURLY:
			return parseScope();
		case TokenType::IDENTIFIER:
			return parseAssignmentStatement();
		case TokenType::IF:
			return parseIfStatement();
		default:
			return parseExpression();
	}
}

std::unique_ptr<Expression> Parser::parseOrExpression() {
	std::unique_ptr<Expression> left = parseAndExpression();

	while (getToken().type == TokenType::OR) {
		Token op = eat(TokenType::OR);

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseAndExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseAndExpression() {
	std::unique_ptr<Expression> left = parseComparisonExpression();

	while (getToken().type == TokenType::AND) {
		Token op = eat(TokenType::AND);

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseComparisonExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression() {
	std::unique_ptr<Expression> left = parseAdditiveExpression();

	Token token = getToken();
	TokenType comparisonOperators[] = {TokenType::DOUBLE_EQUALS, TokenType::GREATER_THAN,
									   TokenType::GREATER_THAN_OR_EQUALS_TO, TokenType::LESS_THAN,
									   TokenType::LESS_THAN_OR_EQUALS_TO};

	while (std::find_if(std::begin(comparisonOperators), std::end(comparisonOperators),
						[token](TokenType t) { return token.type == t; }) != std::end(comparisonOperators)) {
		Token op = eat();
		token = getToken();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseAdditiveExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression() {
	std::unique_ptr<Expression> left = parseMultiplicativeExpression();

	// use while loop here to handle chaining multiple operators, e.g. 1 + 2 + 3
	while (getToken().type == TokenType::PLUS || getToken().type == TokenType::MINUS) {
		Token op = eat();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseMultiplicativeExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression() {
	std::unique_ptr<Expression> left = parseUnaryExpression();

	while (getToken().type == TokenType::MULTIPLY || getToken().type == TokenType::DIVIDE) {
		Token op = eat();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseUnaryExpression();
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression() {
	Token token = getToken();

	switch (token.type) {
		case TokenType::MINUS:
		case TokenType::NOT: {
			Token op = eat();
			std::unique_ptr<UnaryExpression> unaryExpression = std::make_unique<UnaryExpression>();
			unaryExpression->operand = parsePrimaryExpression();
			unaryExpression->op = op;
			return unaryExpression;
		}
		default: {
			return parsePrimaryExpression();
		}
	}
}

std::unique_ptr<Expression> Parser::parsePrimaryExpression() {
	switch (getToken().type) {
		case TokenType::IDENTIFIER: {
			std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();
			Token identifierToken = eat(TokenType::IDENTIFIER);
			identifier->token = identifierToken;
			return identifier;
		}
		case TokenType::NUMBER: {
			std::unique_ptr<NumericLiteral> numericLiteral = std::make_unique<NumericLiteral>();
			numericLiteral->value = std::stof(eat(TokenType::NUMBER).value);
			return numericLiteral;
		}
		case TokenType::OPEN_PARENTHESIS: {
			eat(TokenType::OPEN_PARENTHESIS);
			std::unique_ptr<Expression> result = parseExpression();
			eat(TokenType::CLOSE_PARENTHESIS);
			return result;
		}
		case TokenType::TRUE: {
			eat(TokenType::TRUE);
			std::unique_ptr<BooleanLiteral> booleanLiteral = std::make_unique<BooleanLiteral>();
			booleanLiteral->value = true;
			return booleanLiteral;
		}
		case TokenType::FALSE: {
			eat(TokenType::FALSE);
			std::unique_ptr<BooleanLiteral> booleanLiteral = std::make_unique<BooleanLiteral>();
			booleanLiteral->value = false;
			return booleanLiteral;
		}
		default: {
			Token token = getToken();
			Error::abortWithLineNumber("Unexpected token encountered: `" + token.getName() + "`", token.lineNumber);
			return nullptr;
		}
	}
}

std::unique_ptr<VariableDeclarationStatement> Parser::parseVariableDeclarationStatement() {
	eat(TokenType::LET);  // eat the let keyword

	Token identifierToken = eat(TokenType::IDENTIFIER);	 // expects idenfifier

	std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();
	identifier->token.value = std::move(identifierToken.value);

	std::unique_ptr<VariableDeclarationStatement> variableDeclarationStatement =
		std::make_unique<VariableDeclarationStatement>();
	variableDeclarationStatement->identifier = std::move(identifier);

	Token nextToken = eat();

	// something like `let x;`;
	// in this case, default it to let x = null;
	if (nextToken.type == TokenType::SEMI_COLON) {
		variableDeclarationStatement->value = std::make_unique<NullLiteral>();
		return variableDeclarationStatement;
	}

	// if did not return, then means it should be something like `let x = 10;`
	if (nextToken.type == TokenType::EQUALS) {
		std::unique_ptr<Expression> rhs = parseExpression();
		variableDeclarationStatement->value = std::move(rhs);

		eat(TokenType::SEMI_COLON);
		return variableDeclarationStatement;
	}

	Error::abortWithLineNumber("Unexpected token in variable declaration after `" + identifier->token.value +
								   "`. Expected `;` or `=`, received: `" + nextToken.getName() + "`",
							   nextToken.lineNumber);
	return nullptr;
}

std::unique_ptr<PrintStatement> Parser::parsePrintStatement() {
	Token printToken = eat(TokenType::PRINT);  // eat the print token

	std::unique_ptr<PrintStatement> printStatement = std::make_unique<PrintStatement>();
	printStatement->printToken = printToken;
	printStatement->value = parseExpression();

	eat(TokenType::SEMI_COLON);

	return printStatement;
}

std::unique_ptr<Scope> Parser::parseScope() {
	eat(TokenType::OPEN_CURLY);

	std::unique_ptr<Scope> scope = std::make_unique<Scope>();

	while (getToken().type != TokenType::CLOSE_CURLY) {
		scope->statements.push_back(parseStatement());
	}

	eat(TokenType::CLOSE_CURLY);

	return scope;
}

std::unique_ptr<AssignmentStatement> Parser::parseAssignmentStatement() {
	Token identifierToken = eat(TokenType::IDENTIFIER);

	std::unique_ptr<AssignmentStatement> assignmentStatement = std::make_unique<AssignmentStatement>();
	std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();

	identifier->token = identifierToken;

	assignmentStatement->identifier = std::move(identifier);

	eat(TokenType::EQUALS);

	assignmentStatement->rhs = parseExpression();

	eat(TokenType::SEMI_COLON);

	return assignmentStatement;
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
	Token ifToken = eat(TokenType::IF);
	eat(TokenType::OPEN_PARENTHESIS);

	std::unique_ptr<IfStatement> ifStatement = std::make_unique<IfStatement>();
	ifStatement->ifToken = ifToken;
	ifStatement->condition = parseExpression();

	eat(TokenType::CLOSE_PARENTHESIS);

	ifStatement->scope = parseScope();

	if (getToken().type == TokenType::ELSE) {
		eat(TokenType::ELSE);
		std::variant<std::unique_ptr<Scope>, std::unique_ptr<IfStatement>> alternate;

		if (getToken().type == TokenType::IF) {
			ifStatement->alternate.emplace(parseIfStatement());
		} else {
			ifStatement->alternate.emplace(parseScope());
		}
	}

	return ifStatement;
}

Scope Parser::parse() {
	Scope program(true);

	while (!isEof()) program.statements.push_back(parseStatement());

	return program;
}