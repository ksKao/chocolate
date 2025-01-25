#pragma once

#include "AST/Expression.h"

struct BooleanLiteral : Expression {
	bool value;

	inline std::string getName() const override {
		return "Boolean Literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};