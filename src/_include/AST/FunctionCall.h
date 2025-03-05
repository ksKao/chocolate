#pragma once

#include "AST/Expression.h"
#include "AST/Identifier.h"

struct FunctionCall : Expression {
	std::unique_ptr<Identifier> identifier;
	std::vector<Expression> parameters;

	inline std::string getName() const override {
		return "Function Call Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
