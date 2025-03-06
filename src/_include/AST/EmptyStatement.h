#pragma once

#include "AST/Node.h"

struct EmptyStatement : Node {
	inline std::string getName() const override {
		return "Empty Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};
