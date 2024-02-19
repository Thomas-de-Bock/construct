#include "construct_flags.h"
#include "construct_types.h"

using namespace std;

CON_BITWIDTH bitwidth = BIT64;
std::string inputfile;
std::string outputfile;

int set_bitwidth(char* argv) {
  if(strcmp(argv, "elf64") == 0) {
    bitwidth = BIT64;
    return 0;
  }
  if(strcmp(argv, "elf32") == 0) {
    bitwidth = BIT32;
    return 0;
  }
  if(strcmp(argv, "elf16") == 0) {
    bitwidth = BIT16;
    return 0;
  }
  if(strcmp(argv, "elf8") == 0) {
    bitwidth = BIT8;
    return 0;
  }
  cout << "\"" << argv << "\" not a supported format" << endl;
  return -1;
}

/**
 * Returns the output path for a given input path.
 *
 * If the input path has no extension, ".construct.asm" is appended.
 * If the input path has an extension:
 * - If the extension is ".asm", it is replaced with ".construct.asm"
 *   (e.g. "foo.asm" -> "foo.construct.asm")
 * - Otherwise, the extension is removed and ".construct.asm" is appended
 *   (e.g. "foo.con" -> "foo.asm")
 */
string get_outpath(string path) {
  if (path.find(".") == string::npos) {
    return path + ".construct.asm";
  }
  int last_dot = path.find_last_of(".");
  std::string extension = path.substr(last_dot);
  if (extension == ".asm") {
    return path.substr(0, last_dot) + ".construct.asm";
  }
  return path.substr(0, last_dot) + ".asm";
}

int handle_flags(int argc, char** argv) {
  bool bitwidth_set = false;
  bool path_set = false;
  bool outpath_set = false;
  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-f") == 0 && set_bitwidth(argv[i+1]) == 0) {
      bitwidth_set = true;
      i++;
      continue;
    }
    if(strcmp(argv[i], "-i") == 0) {
      path_set = true;
      i++;
      inputfile = argv[i];
      continue;
    }
    if(strcmp(argv[i], "-o") == 0) {
      outpath_set = true;
      i++;
      outputfile = argv[i];
      continue;
    }
    if(!inputfile.empty()) {
      path_set = true;
      inputfile = argv[i];
    }
  }
  if(!bitwidth_set) {
    cerr << "flag -f (format) not set" << endl;
    return -1;
  }
  if(!path_set) {
    cerr << "flag -i (input file) not set" << endl;
    return -1;
  }
  if(!outpath_set) {
    outputfile = get_outpath(inputfile);
    cerr << "flag -o (output file) not set, using " << outputfile << endl;
  }
  return 0;
}
