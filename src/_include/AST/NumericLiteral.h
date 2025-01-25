#pragma once

#include "AST/Expression.h"

struct NumericLiteral : Expression {
	float value;

	inline std::string getTypeName() const override {
		return "Numeric literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};
