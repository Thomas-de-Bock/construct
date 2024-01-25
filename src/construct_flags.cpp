#include "construct_flags.h"
#include "construct_types.h"

using namespace std;

int set_bitwidth(char* argv)
{
  if (strcmp(argv, "elf64") == 0) {
    bitwidth = BIT64;
    return 0;
  }
  if (strcmp(argv, "elf32") == 0) {
    bitwidth = BIT32;
    return 0;
  }
  if (strcmp(argv, "elf16") == 0) {
    bitwidth = BIT16;
    return 0;
  }
  if (strcmp(argv, "elf8") == 0) {
    bitwidth = BIT8;
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
    if (strcmp(argv[i], "-f") == 0 && set_bitwidth(argv[i+1]) == 0) {
      bitwidth_set = true;
      i++;
      continue;
    }
    if (strcmp(argv[i], "-i") == 0) {
      path_set = true;
      i++;
      (*path) = argv[i];
      continue;
    }
    if (strcmp(argv[i], "-o") == 0) {
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
