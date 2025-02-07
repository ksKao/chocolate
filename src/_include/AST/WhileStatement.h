#pragma once

#include "AST/Expression.h"
#include "AST/Scope.h"

struct WhileStatement : Node {
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Scope> scope;
	Token whileToken;

	inline std::string getName() const override {
		return "While Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};