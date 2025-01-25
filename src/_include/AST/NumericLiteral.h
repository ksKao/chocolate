#pragma once

#include "AST/Expression.h"

struct NumericLiteral : Expression {
	float value;

	inline std::string getName() const override {
		return "Numeric literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
