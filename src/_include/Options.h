#pragma once

#include <string>

class Options {
  public:
	static inline std::string inputFilename;
	static inline std::string outputFilename = "out.asm";

  public:
	static void parse(int argc, char** argv);
};