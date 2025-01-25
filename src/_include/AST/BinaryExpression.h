#pragma once

#include "AST/Expression.h"

struct BinaryExpression : Expression {
	std::unique_ptr<Expression> left;
	std::unique_ptr<Expression> right;
	Token op;

	inline std::string getName() const override {
		return "Binary Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};