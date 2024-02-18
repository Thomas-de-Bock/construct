#include <string>
#include <iostream>
#include "construct_flags.h"
#include "construct_types.h"

extern CON_BITWIDTH bitwidth;

using namespace std;

int set_bitwidth(char* argv)
{
  if (string(argv) == "elf8") {
    bitwidth = BIT8;
    return 0;
  }
  if (string(argv) == "elf16") {
    bitwidth = BIT16;
    return 0;
  }
  if (string(argv) == "elf32") {
    bitwidth = BIT32;
    return 0;
  }
  if (string(argv) == "elf64") {
    bitwidth = BIT64;
    return 0;
  }
  cout << "\"" << argv << "\" not a supported format" << endl;
  return -1;
}

int handle_flags(int argc, char** argv, string* path, string* outpath)
{
  bool bitwidth_set = false;
  bool path_set = false;
  bool outpath_set = false;
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "-f" && set_bitwidth(argv[i+1]) == 0) {
      bitwidth_set = true;
      i++;
      continue;
    }
    if (string(argv[i]) == "-i") {
      path_set = true;
      i++;
      (*path) = argv[i];
      continue;
    }
    if (string(argv[i]) == "-o") {
      outpath_set = true;
      i++;
      (*outpath) = argv[i];
      continue;
    }
    if (path != NULL) {
      path_set = true;
      (*path) = argv[i];
    }
  }
  if (!bitwidth_set) {
    cout << "flag -f (format) not set" << endl;
    return -1;
  }
  if (!path_set) {
    cout << "flag -i (input file) not set" << endl;
    return -1;
  }
  if (!outpath_set) {
    cout << "flag -o (output file) not set" << endl;
    return -1;
  }
  return 0;
}
