#ifndef CON_DECONSTRUCT_H
#define CON_DECONSTRUCT_H

#include "construct_debug.h"
#include "construct_error.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stack>

int get_line_indentation(std::string line);
CON_TOKENTYPE get_token_type(std::string line);
CON_COMPARISON str_to_comparison(std::string comp);

std::vector<con_token*> delinearize_tokens(std::vector<con_token*> tokens);

con_macro* parse_macro(linedata* linedata);
con_if* parse_if(linedata* linedata);
con_while* parse_while(linedata* linedata);
con_section* parse_section(linedata* linedata);
con_tag* parse_tag(linedata* linedata);
con_cmd* parse_cmd(linedata* linedata);
con_function* parse_function(linedata* linedata);
con_funcall* parse_funcall(linedata* linedata);
con_token* parse_line(linedata* linedata);
std::vector<con_token*> parse_construct(std::string code);

#endif
