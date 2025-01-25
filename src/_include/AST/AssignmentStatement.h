#pragma once

#include "AST/Expression.h"
#include "AST/Identifier.h"
#include "Token.h"

struct AssignmentStatement : Node {
	std::unique_ptr<Identifier> identifier;
	std::unique_ptr<Expression> rhs;

	inline std::string getName() const override {
		return "Assignment Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};