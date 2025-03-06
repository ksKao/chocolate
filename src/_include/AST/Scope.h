#pragma once

#include "AST/FunctionDeclarationStatement.h"
#include "Node.h"

struct FunctionDeclarationStatement;

struct Scope : Node {
	bool isRoot;
	std::vector<std::unique_ptr<Node>> statements;
	std::vector<std::unique_ptr<FunctionDeclarationStatement>> functions;

	Scope(bool isRoot = false) {
		this->isRoot = isRoot;
	}

	inline std::string getName() const override {
		return "Scope node";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
