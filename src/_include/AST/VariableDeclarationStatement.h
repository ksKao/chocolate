#pragma once

#include "AST/Identifier.h"

struct VariableDeclarationStatement : Node {
	std::unique_ptr<Identifier> identifier;
	std::unique_ptr<Expression> value;

	inline std::string getName() const override {
		return "Variable declaration statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
