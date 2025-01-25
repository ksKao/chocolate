#pragma once
#include <memory>
#include <optional>
#include <vector>

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
	std::unique_ptr<Node> parseExpression();
	std::unique_ptr<Node> parseStatement();
	std::unique_ptr<Node> parseAdditiveExpression();
	std::unique_ptr<Node> parseMultiplicativeExpression();
	std::unique_ptr<Node> parsePrimaryExpression();
	std::unique_ptr<Node> parseVariableDeclarationStatement();
	std::unique_ptr<Node> parsePrintStatement();
	std::unique_ptr<Node> parseScope();
};