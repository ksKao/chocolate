#pragma once
#include "Node.h"

struct NullLiteral : public Node {
	inline std::string getTypeName() const override {
		return "Null literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};