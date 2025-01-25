#pragma once
#include "AST/Expression.h"

struct NullLiteral : Expression {
	inline std::string getTypeName() const override {
		return "Null literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};