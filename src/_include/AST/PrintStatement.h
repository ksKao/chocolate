#pragma once
#include "AST/Expression.h"

struct PrintStatement : Node {
	std::unique_ptr<Expression> value;

	inline std::string getName() const override {
		return "Print statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};