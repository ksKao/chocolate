#pragma once

#include "AST/Expression.h"

struct StringLiteral : Expression {
	std::string value;

	inline std::string getName() const override {
		return "String Literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};