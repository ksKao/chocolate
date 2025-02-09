#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "AST/Expression.h"
#include "AST/ForStatement.h"
#include "AST/IfStatement.h"
#include "AST/PrintStatement.h"
#include "AST/Scope.h"
#include "AST/UnaryExpression.h"
#include "AST/VariableDeclarationStatement.h"
#include "AST/WhileStatement.h"
#include "Token.h"

class Parser {
  public:
	Parser(std::vector<Token> tokens) : tokens(tokens) {}
	Scope parse();

  private:
	std::vector<Token> tokens;
	size_t index = 0;

  private:
	Token eat(std::optional<TokenType> type);
	Token getToken(int advance = 0) const;
	bool isEof() const;

	// expressions
	std::unique_ptr<Expression> parseExpression(bool isStatement = false);
	std::unique_ptr<Expression> parseAssignmentExpression(bool isStatement);
	std::unique_ptr<Expression> parseOrExpression(bool isStatement);
	std::unique_ptr<Expression> parseAndExpression(bool isStatement);
	std::unique_ptr<Expression> parseComparisonExpression(bool isStatement);
	std::unique_ptr<Expression> parseAdditiveExpression(bool isStatement);
	std::unique_ptr<Expression> parseMultiplicativeExpression(bool isStatement);
	std::unique_ptr<Expression> parseUnaryExpression(bool isStatement);
	std::unique_ptr<Expression> parsePrimaryExpression(bool isStatement);
	std::unique_ptr<Identifier> parseIdentifier(bool isStatement);

	// statements
	std::unique_ptr<Node> parseStatement();
	std::unique_ptr<VariableDeclarationStatement> parseVariableDeclarationStatement();
	std::unique_ptr<PrintStatement> parsePrintStatement();
	std::unique_ptr<Scope> parseScope();
	std::unique_ptr<IfStatement> parseIfStatement();
	std::unique_ptr<WhileStatement> parseWhileStatement();
	std::unique_ptr<ForStatement> parseForStatement();
};