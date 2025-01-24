#pragma once
#include "Node.h"

struct PrintStatement : Node {
	std::unique_ptr<Node> value;

	inline std::string getTypeName() const override {
		return "Print statement";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};