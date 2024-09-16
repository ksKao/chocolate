#pragma once

#include "Node.h"

struct VariableDeclarationStatement : public Node {
	Token identifier;
	std::unique_ptr<Node> value;

	inline std::string getTypeName() const override {
		return "Variable declaration statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};
