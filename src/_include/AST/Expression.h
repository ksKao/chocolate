#pragma once

#include "AST/Node.h"

enum class Type { UNKNOWN, NUMBER };

struct Expression : Node {
  public:
	Type type;
	static const std::unordered_map<Type, std::string> typeToStringMap;

	inline std::string getTypeName() const {
		return typeToStringMap.at(type);
	}
	virtual std::string getName() const = 0;
	virtual void print(const std::string &indent) const = 0;
	virtual void generateAssembly() = 0;
};