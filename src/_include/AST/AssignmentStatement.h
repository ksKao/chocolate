#pragma once

#include "AST/Node.h"
#include "Token.h"

struct AssignmentStatement : Node {
	Token identifier;
	std::unique_ptr<Node> rhs;

	inline std::string getTypeName() const override {
		return "Assignment Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() const override;
};