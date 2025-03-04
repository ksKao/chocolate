#pragma once

#include "AST/Identifier.h"
#include "AST/Scope.h"

struct FunctionDeclarationStatement : Node {
	std::unique_ptr<Identifier> identifier;
	std::unique_ptr<Scope> scope;
	Token functionToken;

	inline std::string getName() const override {
		return "Function declaration statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
