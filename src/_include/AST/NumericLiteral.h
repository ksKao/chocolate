#pragma once

#include "Node.h"

struct NumericLiteral : public Node {
	float value;

	inline std::string getTypeName() const override {
		return "Numeric literal";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() const override;
};
