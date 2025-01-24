#pragma once

#include "AST/Node.h"

struct BinaryExpression : Node {
	std::unique_ptr<Node> left;
	std::unique_ptr<Node> right;
	Token op;

	inline std::string getTypeName() const override {
		return "Binary Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};