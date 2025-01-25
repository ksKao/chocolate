#pragma once

#include "Node.h"

struct Scope : Node {
	bool isRoot;
	std::vector<std::unique_ptr<Node>> statements;

	Scope(bool isRoot = false) {
		this->isRoot = isRoot;
	}

	inline std::string getName() const override {
		return "Scope node";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};
