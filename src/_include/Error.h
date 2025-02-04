#pragma once
#include <string>

namespace Error {
void abort(const std::string &message);
void abortWithLineNumber(const std::string &message, size_t lineNo);
};	// namespace Error