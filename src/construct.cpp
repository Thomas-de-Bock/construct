#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "construct_types.h"
#include "deconstruct.h"     // parse_construct()
#include "reconstruct.h"     // linearize_tokens()
#include "construct_flags.h" // handle_flags()

int main(int argc, char** argv) {
  std::string path;
  std::string outpath;
  if (handle_flags(argc, argv, &path, &outpath) != 0) {
    std::cout << "Some flag(s) not set" << std::endl;
    return 0;
  }
  if (path.empty()) {
    std::cout << "No input file specified" << std::endl;
    return 0;
  }

  std::ifstream inpfile(path);
  std::stringstream buffer;
  buffer << inpfile.rdbuf();
  std::vector<con_token*> tokens = parse_construct(buffer.str());

  // Make _start global
  con_token* glob_tok = new con_token();
  glob_tok->tok_type = CMD;
  con_cmd* glob_cmd = new con_cmd;
  glob_tok->tok_cmd = glob_cmd;
  glob_cmd->command = "global _start";
  tokens.insert(tokens.begin(), glob_tok);

  // Order dependant: some tokens are replaced with macros, so apply_macro() must be at the end.
  tokens = delinearize_tokens(tokens);
  apply_functions(tokens);
  apply_ifs(tokens);
  apply_whiles(tokens);
  apply_funcalls(tokens);
  std::vector<con_macro> empty_macros;
  apply_macros(tokens, empty_macros);
  linearize_tokens(tokens);

  std::ofstream outfile;
  outfile.open(outpath);
  outfile << tokens_to_nasm(tokens);
  outfile.close();
}
