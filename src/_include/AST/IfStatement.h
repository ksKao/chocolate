#pragma once

#include <optional>
#include <variant>

#include "AST/Expression.h"
#include "AST/Scope.h"

struct IfStatement : Node {
	std::unique_ptr<Expression> condition;
	std::unique_ptr<Scope> scope;
	std::optional<std::variant<std::unique_ptr<IfStatement>, std::unique_ptr<Scope>>> alternate = std::nullopt;
	std::string doneLabel = "";

	inline std::string getName() const override {
		return "If Statement";
	}
	void print(const std::string &ident) const override;
	void generateAssembly() override;
};