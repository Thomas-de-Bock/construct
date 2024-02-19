#ifndef CON_FLAGS_H
#define CON_FLAGS_H

#include <string>
#include <cstring>
#include <iostream>
#include "construct_types.h"

extern CON_BITWIDTH bitwidth;
extern std::string inputfile;
extern std::string outputfile;

int set_bitwidth(char* argv);
int handle_flags(int argc, char** argv);

#endif
