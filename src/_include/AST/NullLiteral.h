#pragma once
#include "AST/Expression.h"

struct NullLiteral : Expression {
	inline std::string getName() const override {
		return "Null literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};