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

int handle_flags(int argc, char** argv) {
  bool bitwidth_set = false;
  bool path_set = false;
  bool outpath_set = false;
  bool help_set = false;
  for(size_t i = 1; i < argc; i++) {
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
    if(strcmp(argv[i], "-h") == 0) {
      help_set = true;
      continue;
    }

    path_set = true;
    inputfile = argv[i];
  }
  if(!bitwidth_set && !path_set && !outpath_set) {
    help_set = true;
  }
  if(help_set) {
    cout <<
      "Construct (version 1.1.0)\n"
      "An abstraction over x86 assembly providing useful shortcuts and syntax.\n\n"

      "Usage:\n"
      "-i Input file. Should have valid construct syntax.\n"
      "-f Output format. Can be either elf64, elf32, elf16, or elf8.\n"
      "-o Output file. Specifies where to put the resulting NASM assembly file.\n\n"

      "2024 Thomas de Bock\n"
      "MIT LICENSE\n"
      "https://github.com/Thomas-de-Bock/construct/\n";
    return -1;
  }
  if(!bitwidth_set) {
    cout << "flag -f (format) not set" << endl;
    return -1;
  }
  if(!path_set) {
    cout << "flag -i (input file) not set" << endl;
    return -1;
  }
  if(!outpath_set) {
    cout << "flag -o (output file) not set" << endl;
    return -1;
  }
  return 0;
}
