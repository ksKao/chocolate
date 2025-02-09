#include "AST/Expression.h"
#include "AST/Identifier.h"

// different from unary because the operand must be an identifier
struct IncrementDecrementExpression : Expression {
	std::unique_ptr<Identifier> identifier;
	Token op;
	bool isPrefix;

	inline std::string getName() const override {
		return "Increment Decrement Expression";
	}
	void print(const std::string &indent) const override;
	void generateAssembly() override;
};