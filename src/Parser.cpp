#include "Parser.h"

#include <algorithm>
#include <memory>

#include "AST/AssignmentExpression.h"
#include "AST/BinaryExpression.h"
#include "AST/BooleanLiteral.h"
#include "AST/FunctionDeclarationStatement.h"
#include "AST/Identifier.h"
#include "AST/IncrementDecrementExpression.h"
#include "AST/NullLiteral.h"
#include "AST/NumericLiteral.h"
#include "AST/PrintStatement.h"
#include "AST/Scope.h"
#include "AST/StringLiteral.h"
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

Token Parser::getToken(int advance) const {
	int indexToGet = index + advance;

	if (indexToGet < 0 || indexToGet >= tokens.size()) {
		Error::abort("Could not get token at index " + std::to_string(indexToGet) + ". Valid range is 0 - " +
					 std::to_string(tokens.size() - 1));
		return Token();
	}

	return tokens.at(indexToGet);
}

bool Parser::isEof() const {
	return getToken().type == TokenType::END_OF_FILE;
}

std::unique_ptr<Expression> Parser::parseExpression(bool isStatement) {
	std::unique_ptr<Expression> expression = parseAssignmentExpression(isStatement);

	if (isStatement) {
		expression->isStatement = true;
		eat(TokenType::SEMI_COLON);
	}

	return expression;
}

std::unique_ptr<Expression> Parser::parseAssignmentExpression(bool isStatement) {
	if (getToken().type == TokenType::IDENTIFIER && getToken(1).type == TokenType::EQUALS) {
		std::unique_ptr<Identifier> identifier = parseIdentifier(false);
		Token op = eat(TokenType::EQUALS);

		std::unique_ptr<AssignmentExpression> assignmentExpression = std::make_unique<AssignmentExpression>();
		assignmentExpression->identifier = std::move(identifier);
		assignmentExpression->right = parseOrExpression(false);
		assignmentExpression->isStatement = isStatement;
		assignmentExpression->op = op;

		return assignmentExpression;
	} else {
		return parseOrExpression(isStatement);
	}
}

std::unique_ptr<Expression> Parser::parseOrExpression(bool isStatement) {
	std::unique_ptr<Expression> left = parseAndExpression(isStatement);

	while (getToken().type == TokenType::OR) {
		Token op = eat(TokenType::OR);

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseAndExpression(false);
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseAndExpression(bool isStatement) {
	std::unique_ptr<Expression> left = parseComparisonExpression(isStatement);

	while (getToken().type == TokenType::AND) {
		Token op = eat(TokenType::AND);

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseComparisonExpression(false);
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseComparisonExpression(bool isStatement) {
	std::unique_ptr<Expression> left = parseAdditiveExpression(isStatement);

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
		binaryExpression->right = parseAdditiveExpression(false);
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseAdditiveExpression(bool isStatement) {
	std::unique_ptr<Expression> left = parseMultiplicativeExpression(isStatement);

	// use while loop here to handle chaining multiple operators, e.g. 1 + 2 + 3
	while (getToken().type == TokenType::PLUS || getToken().type == TokenType::MINUS) {
		Token op = eat();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseMultiplicativeExpression(false);
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseMultiplicativeExpression(bool isStatement) {
	std::unique_ptr<Expression> left = parseUnaryExpression(isStatement);

	while (getToken().type == TokenType::MULTIPLY || getToken().type == TokenType::DIVIDE) {
		Token op = eat();

		std::unique_ptr<BinaryExpression> binaryExpression = std::make_unique<BinaryExpression>();
		binaryExpression->left = std::move(left);
		binaryExpression->right = parseUnaryExpression(false);
		binaryExpression->op = op;

		left = std::move(binaryExpression);
	}

	return left;
}

std::unique_ptr<Expression> Parser::parseUnaryExpression(bool isStatement) {
	Token token = getToken();

	switch (token.type) {
		case TokenType::MINUS:
		case TokenType::NOT: {
			Token op = eat();
			std::unique_ptr<UnaryExpression> unaryExpression = std::make_unique<UnaryExpression>();
			unaryExpression->operand = parseIncrementDecrementExpression(false);
			unaryExpression->op = op;
			return unaryExpression;
		}
		default: {
			return parseIncrementDecrementExpression(isStatement);
		}
	}
}

std::unique_ptr<Expression> Parser::parseIncrementDecrementExpression(bool isStatement) {
	Token token = getToken();

	if (token.type == TokenType::INCREMENT || token.type == TokenType::DECREMENT) {
		std::unique_ptr<IncrementDecrementExpression> incrementDecrementExpression =
			std::make_unique<IncrementDecrementExpression>();

		incrementDecrementExpression->op = eat();
		incrementDecrementExpression->identifier = parseIdentifier(false);
		incrementDecrementExpression->isStatement = isStatement;
		incrementDecrementExpression->isPrefix = true;
		return incrementDecrementExpression;
	} else if (token.type == TokenType::IDENTIFIER &&
			   (getToken(1).type == TokenType::INCREMENT || getToken(1).type == TokenType::DECREMENT)) {
		std::unique_ptr<IncrementDecrementExpression> incrementDecrementExpression =
			std::make_unique<IncrementDecrementExpression>();

		incrementDecrementExpression->identifier = parseIdentifier(false);
		incrementDecrementExpression->op = eat();
		incrementDecrementExpression->isStatement = isStatement;
		incrementDecrementExpression->isPrefix = false;
		return incrementDecrementExpression;
	} else {
		return parsePrimaryExpression(isStatement);
	}
}

std::unique_ptr<Expression> Parser::parsePrimaryExpression(bool isStatement) {
	switch (getToken().type) {
		case TokenType::IDENTIFIER: {
			return parseIdentifier(isStatement);
		}
		case TokenType::NUMBER: {
			std::unique_ptr<NumericLiteral> numericLiteral = std::make_unique<NumericLiteral>();
			numericLiteral->value = std::stof(eat(TokenType::NUMBER).value);
			numericLiteral->isStatement = isStatement;
			return numericLiteral;
		}
		case TokenType::OPEN_PARENTHESIS: {
			eat(TokenType::OPEN_PARENTHESIS);
			std::unique_ptr<Expression> result = parseExpression();
			result->isStatement = isStatement;
			eat(TokenType::CLOSE_PARENTHESIS);
			return result;
		}
		case TokenType::TRUE: {
			eat(TokenType::TRUE);
			std::unique_ptr<BooleanLiteral> booleanLiteral = std::make_unique<BooleanLiteral>();
			booleanLiteral->value = true;
			booleanLiteral->isStatement = isStatement;
			return booleanLiteral;
		}
		case TokenType::FALSE: {
			eat(TokenType::FALSE);
			std::unique_ptr<BooleanLiteral> booleanLiteral = std::make_unique<BooleanLiteral>();
			booleanLiteral->value = false;
			booleanLiteral->isStatement = isStatement;
			return booleanLiteral;
		}
		case TokenType::STRING: {
			Token token = eat(TokenType::STRING);
			std::unique_ptr<StringLiteral> stringLiteral = std::make_unique<StringLiteral>();
			stringLiteral->value = token.value;
			stringLiteral->isStatement = isStatement;
			return stringLiteral;
		}
		default: {
			Token token = getToken();
			Error::abortWithLineNumber("Unexpected token encountered: `" + token.getName() + "`", token.lineNumber);
			return nullptr;
		}
	}
}

std::unique_ptr<Identifier> Parser::parseIdentifier(bool isStatement) {
	std::unique_ptr<Identifier> identifier = std::make_unique<Identifier>();
	Token identifierToken = eat(TokenType::IDENTIFIER);
	identifier->token = identifierToken;
	identifier->isStatement = isStatement;
	return identifier;
}

std::unique_ptr<Node> Parser::parseStatement() {
	Token token = getToken();

	switch (token.type) {
		case TokenType::LET:
			return parseVariableDeclarationStatement();
		case TokenType::PRINT:
			return parsePrintStatement();
		case TokenType::OPEN_CURLY:
			return parseScope();
		case TokenType::IF:
			return parseIfStatement();
		case TokenType::WHILE:
			return parseWhileStatement();
		case TokenType::FOR:
			return parseForStatement();
		case TokenType::FUNCTION:
			return parseFunctionDeclarationStatement();
		default:
			return parseExpression(true);
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

std::unique_ptr<WhileStatement> Parser::parseWhileStatement() {
	Token whileToken = eat(TokenType::WHILE);

	eat(TokenType::OPEN_PARENTHESIS);

	std::unique_ptr<WhileStatement> whileStatement = std::make_unique<WhileStatement>();
	whileStatement->whileToken = whileToken;
	whileStatement->condition = parseExpression();

	eat(TokenType::CLOSE_PARENTHESIS);

	whileStatement->scope = parseScope();

	return whileStatement;
}

std::unique_ptr<ForStatement> Parser::parseForStatement() {
	Token forToken = eat(TokenType::FOR);

	eat(TokenType::OPEN_PARENTHESIS);

	std::unique_ptr<ForStatement> forStatement = std::make_unique<ForStatement>();
	forStatement->forToken = forToken;

	forStatement->initStatement = parseStatement();
	forStatement->condition = parseExpression();
	eat(TokenType::SEMI_COLON);

	forStatement->updateExpression = parseExpression();

	eat(TokenType::CLOSE_PARENTHESIS);

	forStatement->scope = parseScope();

	return forStatement;
}

std::unique_ptr<FunctionDeclarationStatement> Parser::parseFunctionDeclarationStatement() {
	Token functionToken = eat(TokenType::FUNCTION);

	std::unique_ptr<FunctionDeclarationStatement> functionDeclarationStatement =
		std::make_unique<FunctionDeclarationStatement>();
	functionDeclarationStatement->functionToken = functionToken;

	Token identifierToken = eat(TokenType::IDENTIFIER);

	functionDeclarationStatement->identifier = std::make_unique<Identifier>();
	functionDeclarationStatement->identifier->token = identifierToken;

	eat(TokenType::OPEN_PARENTHESIS);
	eat(TokenType::CLOSE_PARENTHESIS);

	functionDeclarationStatement->scope = parseScope();

	return functionDeclarationStatement;
}

Scope Parser::parse() {
	Scope program(true);

	while (!isEof()) program.statements.push_back(parseStatement());

	return program;
}
