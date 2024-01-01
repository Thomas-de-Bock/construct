#include "deconstruct.h"
#include "reconstruct.h"

int main(int argc, char** argv) {
  std::ifstream inpfile(argv[1]);
  std::stringstream buffer;
  buffer << inpfile.rdbuf();
  std::vector<con_token*> tokens = parse_construct(buffer.str());
  
  // Make _start global
  con_token* glob_tok = new con_token;
  glob_tok->tok_type = CMD;
  con_cmd* glob_cmd = new con_cmd;
  glob_tok->tok_cmd = glob_cmd;
  glob_cmd->command = "global _start";

  tokens.insert(tokens.begin(), glob_tok);

  tokens = delinearize_tokens(tokens);
  apply_functions(tokens);
  apply_ifs(tokens);
  apply_whiles(tokens);
  std::vector<con_macro> empty_macros;
  apply_macros(tokens, empty_macros);
  linearize_tokens(tokens);

  std::cout << tokens_to_nasm(tokens);
}
