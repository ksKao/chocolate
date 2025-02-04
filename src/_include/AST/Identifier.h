#pragma once
#include "AST/Expression.h"

struct Identifier : Expression {
	Token token;

	inline std::string getName() const override {
		return "Identifier";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};