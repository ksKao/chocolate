#pragma once
#include <memory>
#include <optional>
#include <vector>

#include "AST/AssignmentStatement.h"
#include "AST/Expression.h"
#include "AST/PrintStatement.h"
#include "AST/Scope.h"
#include "AST/VariableDeclarationStatement.h"
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
	Token getToken() const;
	bool isEof() const;
	std::unique_ptr<Expression> parseExpression();
	std::unique_ptr<Node> parseStatement();
	std::unique_ptr<Expression> parseAdditiveExpression();
	std::unique_ptr<Expression> parseMultiplicativeExpression();
	std::unique_ptr<Expression> parsePrimaryExpression();
	std::unique_ptr<VariableDeclarationStatement> parseVariableDeclarationStatement();
	std::unique_ptr<PrintStatement> parsePrintStatement();
	std::unique_ptr<Scope> parseScope();
	std::unique_ptr<AssignmentStatement> parseAssignmentStatement();
};