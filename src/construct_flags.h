#ifndef CONSTRUCT_FLAGS_H_
#define CONSTRUCT_FLAGS_H_

#include <string>

int set_bitwidth(char* argv);
int handle_flags(int argc, char** argv, std::string* path, std::string* outpath);

#endif // CONSTRUCT_FLAGS_H_
