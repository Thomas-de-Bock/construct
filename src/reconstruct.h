#include<string>
#include<iostream>
#include<stdint.h>
#include "construct_types.h"

// Used for naming tags
extern int if_amnt;
extern int while_amnt;

std::string reg_to_str(uint8_t call_num, CON_BITWIDTH bitwidth);
std::string comparison_to_string(CON_COMPARISON condition);
CON_COMPARISON get_comparison_inverse(CON_COMPARISON condition);

// During linearization, the construct parent tokens are removed
void linearize_tokens(std::vector<con_token*>& tokens);
void apply_macro_to_token(con_token& token, std::vector<con_macro> macros);

// The following functions transform the construct specific tokens to nasm ones, 
// the parent construct tokens remain, but are removed during linearization

// Converts args to macros and adds tag with same name to child tokens
void apply_functions(std::vector<con_token*>& tokens);
void apply_funcalls(std::vector<con_token*>& tokens);
void apply_whiles(std::vector<con_token*>& tokens);
void apply_ifs(std::vector<con_token*>& tokens);
void apply_macros(std::vector<con_token*>& tokens, std::vector<con_macro> macros);

std::string tokens_to_nasm(std::vector<con_token*>& tokens);
