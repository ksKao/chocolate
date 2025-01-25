#pragma once

#include "AST/Expression.h"
#include "AST/Scope.h"

struct IfStatement : Node {
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Scope> scope;

	inline std::string getName() const override {
		return "If Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};