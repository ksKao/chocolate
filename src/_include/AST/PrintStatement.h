#pragma once
#include "AST/Expression.h"

struct PrintStatement : Node {
	std::unique_ptr<Expression> value;

	inline std::string getTypeName() const override {
		return "Print statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};