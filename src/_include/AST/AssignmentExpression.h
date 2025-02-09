#pragma once

#include "AST/Expression.h"
#include "AST/Identifier.h"

struct AssignmentExpression : Expression {
	std::unique_ptr<Identifier> identifier;
	std::unique_ptr<Expression> right;
	Token op;

	inline std::string getName() const override {
		return "Binary Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};