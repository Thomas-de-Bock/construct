#include "deconstruct.h"
#include "reconstruct.h"
#include "construct_flags.h"
#include<iostream>
#include<fstream>

int main(int argc, char** argv) {
  std::string path;
  std::string outpath;
  if(handle_flags(argc, argv, &path, &outpath) != 0) {
    std::cout << "Some flag(s) not set" << std::endl;
    return 0;
  }
  if(path.empty()) {
    std::cout << "No input file specified" << std::endl;
    return 0;
  }

  std::ifstream inpfile(path);
  if (!inpfile.is_open()) {
	std::cout << "Could not open input file" << std::endl;
	return 0;
  }
  if (!inpfile.good()) {
	std::cout << "Input file is not good" << std::endl;
	return 0;
  }

  std::ofstream outfile;
  outfile.open(outpath);
  if (!outfile.is_open()) {
	std::cout << "Could not open output file" << std::endl;
	return 0;
  }
  if (!outfile.good()) {
	std::cout << "Output file is not good" << std::endl;
	return 0;
  }

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

  outfile << tokens_to_nasm(tokens);
  outfile.close();
}
