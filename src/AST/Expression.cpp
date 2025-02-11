#include "AST/Expression.h"

const std::unordered_map<Type, std::string> Expression::typeToStringMap = {
	{Type::UNKNOWN, "unknown"}, {Type::NUMBER, "number"}, {Type::BOOLEAN, "boolean"}, {Type::STRING, "string"}};