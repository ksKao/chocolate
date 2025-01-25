#pragma once

#include "AST/Expression.h"

struct VariableDeclarationStatement : Node {
	Token identifier;
	std::unique_ptr<Expression> value;

	inline std::string getTypeName() const override {
		return "Variable declaration statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};
