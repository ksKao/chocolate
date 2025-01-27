#include "AST/Expression.h"

struct UnaryExpression : Expression {
	std::unique_ptr<Expression> operand;
	Token op;

	inline std::string getName() const override {
		return "Binary Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};