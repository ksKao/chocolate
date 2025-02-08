#pragma once

#include "AST/Expression.h"
#include "AST/Scope.h"

struct ForStatement : Node {
	std::unique_ptr<Node> initStatement;
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Expression> updateExpression;
	std::unique_ptr<Scope> scope;
	Token forToken;

	inline std::string getName() const override {
		return "For Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};