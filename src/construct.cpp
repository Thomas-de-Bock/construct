#include "deconstruct.h"
#include "reconstruct.h"
#include "construct_flags.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv) {
  if(handle_flags(argc, argv) != 0) {
    std::cout << "Some flag(s) not set" << std::endl;
    return 0;
  }
  if(inputfile.empty()) {
    std::cout << "No input file specified" << std::endl;
    return 0;
  }

  std::ifstream inpfile(inputfile);
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

  tokens = delinearize_tokens(tokens);
  apply_functions(tokens);
  apply_ifs(tokens);
  apply_whiles(tokens);
  apply_funcalls(tokens);
  std::vector<con_macro> empty_macros;
  apply_macros(tokens, empty_macros);
  linearize_tokens(tokens);

  std::ofstream outfile;
  outfile.open(outputfile);
  outfile << tokens_to_nasm(tokens);
  outfile.close();
}
