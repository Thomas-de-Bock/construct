#ifndef DECONSTRUCT_H_
#define DECONSTRUCT_H_

#include <string>
#include <vector>
#include "construct_types.h"

int get_line_indentation(std::string line);
CON_TOKENTYPE get_token_type(std::string line);
CON_COMPARISON str_to_comparison(std::string comp);

std::vector<con_token*> delinearize_tokens(std::vector<con_token*> tokens);

con_section* parse_section(std::string line);
con_tag* parse_tag(std::string line);
con_while* parse_while(std::string line);
con_if* parse_if(std::string line);
con_function* parse_function(std::string line);
con_cmd* parse_cmd(std::string line);
con_macro* parse_macro(std::string line);
con_funcall* parse_funcall(std::string line);
con_syscall* parse_syscall(std::string line);

con_token* parse_line(std::string line);
std::vector<con_token*> parse_construct(std::string code);

#endif // DECONSTRUCT_H_
