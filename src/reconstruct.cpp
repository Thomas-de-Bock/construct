#include <string>
#include <vector>
#include <stdexcept>
#include "reconstruct.h"
#include "construct_types.h"

using namespace std;
int if_amnt = 0;
int while_amnt = 0;
CON_BITWIDTH bitwidth = BIT64;

string reg_to_str(uint8_t call_num)
{
  switch (bitwidth) {
    case BIT8:
      switch (call_num) {
        case 0:
          return "dil";
        break;
        case 1:
          return "sil";
        break;
        case 2:
          return "dl";
        break;
        case 3:
          return "cl";
        break;
        case 4:
          return "r8b";
        break;
        case 5:
          return "r9b";
        break;
      }
    break;
    case BIT16:
      switch (call_num) {
        case 0:
          return "di";
        break;
        case 1:
          return "si";
        break;
        case 2:
          return "dx";
        break;
        case 3:
          return "cx";
        break;
        case 4:
          return "r8w";
        break;
        case 5:
          return "r9w";
        break;
      }
    break;
    case BIT32:
      switch (call_num) {
        case 0:
          return "edi";
        break;
        case 1:
          return "esi";
        break;
        case 2:
          return "edx";
        break;
        case 3:
          return "ecx";
        break;
        case 4:
          return "r8d";
        break;
        case 5:
          return "r9d";
        break;
      }
    break;
    case BIT64:
      switch (call_num) {
        case 0:
          return "rdi";
        break;
        case 1:
          return "rsi";
        break;
        case 2:
          return "rdx";
        break;
        case 3:
          return "rcx";
        break;
        case 4:
          return "r8";
        break;
        case 5:
          return "r9";
        break;
      }
    break;
  }
  throw invalid_argument("Invalid bitwidth or call_num: bitwidth="+to_string(static_cast<int>(bitwidth))+" call_num="+to_string(static_cast<int>(call_num)));
}

string comparison_to_string(CON_COMPARISON condition)
{
  switch (condition) {
    case E:
      return "e";
    case NE:
      return "ne";
    case L:
      return "l";
    case G:
      return "g";
    case LE:
      return "le";
    case GE:
      return "ge";
  }
  throw invalid_argument("Invalid comparison value: "+to_string(static_cast<int>(condition)));
}

CON_COMPARISON get_comparison_inverse(CON_COMPARISON condition)
{
  switch (condition) {
    case E:
      return NE;
    case NE:
      return E;
    case L:
      return GE;
    case G:
      return LE;
    case LE:
      return G;
    case GE:
      return L;
  }
  throw invalid_argument("Invalid comparison value: "+to_string(static_cast<int>(condition)));
}

void apply_macro_to_token(con_token& token, vector<con_macro> macros)
{
  if (token.tok_type != WHILE && token.tok_type != IF && token.tok_type != CMD) {
    return;
  }
  // Unoptimal, but more clear imo
  for (size_t i = 0; i < macros.size(); i++) {
    con_macro* crntmacro = &macros[i];
    size_t pos;
    switch (token.tok_type) {
      case WHILE:
        if (!token.tok_while->condition.arg1.empty() &&
            (pos = token.tok_while->condition.arg1.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_while->condition.arg1[pos-1])) &&
            (pos == token.tok_while->condition.arg1.size()-1 || !isalpha(token.tok_while->condition.arg1[pos+crntmacro->macro.size()]))) {

          token.tok_while->condition.arg1.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
        if (!token.tok_while->condition.arg2.empty() &&
            (pos = token.tok_while->condition.arg2.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_while->condition.arg2[pos-1])) &&
            (pos == token.tok_while->condition.arg2.size()-1 || !isalpha(token.tok_while->condition.arg2[pos+crntmacro->macro.size()]))) {
          token.tok_while->condition.arg2.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
      break;
      case IF:
        if (!token.tok_if->condition.arg1.empty() &&
            (pos = token.tok_if->condition.arg1.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_if->condition.arg1[pos-1])) &&
            (pos == token.tok_if->condition.arg1.size()-1 || !isalpha(token.tok_if->condition.arg1[pos+crntmacro->macro.size()]))) {
          token.tok_if->condition.arg1.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
        if (!token.tok_if->condition.arg2.empty() &&
            (pos = token.tok_if->condition.arg2.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_if->condition.arg2[pos-1])) &&
            (pos == token.tok_if->condition.arg2.size()-1 || !isalpha(token.tok_if->condition.arg2[pos+crntmacro->macro.size()]))) {
          token.tok_if->condition.arg2.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
      break;
      case CMD:
        if (!token.tok_cmd->arg1.empty() &&
            (pos = token.tok_cmd->arg1.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_cmd->arg1[pos-1])) &&
            (pos == token.tok_cmd->arg1.size()-1 || !isalpha(token.tok_cmd->arg1[pos+crntmacro->macro.size()]))) {
          token.tok_cmd->arg1.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
        if (!token.tok_cmd->arg2.empty() &&
            (pos = token.tok_cmd->arg2.find(crntmacro->macro)) != string::npos &&
            (pos == 0 || !isalpha(token.tok_cmd->arg2[pos-1])) &&
            (pos == token.tok_cmd->arg2.size()-1 || !isalpha(token.tok_cmd->arg2[pos+crntmacro->macro.size()]))) {
          token.tok_cmd->arg2.replace(pos, crntmacro->macro.size(), crntmacro->value);
        }
      break;
      default:
      break;
    }
  }
}
void apply_funcalls(std::vector<con_token*>& tokens)
{
  for (size_t i = 0; i < tokens.size(); i++) {
    apply_funcalls(tokens[i]->tokens);
    if (tokens[i]->tok_type != FUNCALL) {
      continue;
    }
    vector<string>* args = &tokens[i]->tok_funcall->arguments;
    vector<con_token*> arg_tokens;
    for (size_t j = 0; j < args->size(); j++) {
      con_token* arg_tok = new con_token();
      arg_tok->tok_type = CMD;
      con_cmd* arg_cmd = new con_cmd();
      arg_tok->tok_cmd = arg_cmd;
      arg_cmd->command = "mov";
      arg_cmd->arg1 = reg_to_str(j);
      arg_cmd->arg2 = (*args)[j];
      arg_tokens.push_back(arg_tok);
    }
    con_token* call_tok = new con_token();
    call_tok->tok_type = CMD;
    con_cmd* call_cmd = new con_cmd();
    call_tok->tok_cmd = call_cmd;
    call_cmd->command = "call";
    call_cmd->arg1 = tokens[i]->tok_funcall->funcname;
    arg_tokens.push_back(call_tok);

    tokens.insert(tokens.begin()+i+1, arg_tokens.begin(), arg_tokens.end());
  }
}

void apply_functions(std::vector<con_token*>& tokens)
{
  vector<con_token*>* subtokens = &tokens;
  for (size_t i = 0; i < subtokens->size(); i++) {
    if ((*subtokens)[i]->tok_type != FUNCTION) {
      continue;
    }
    con_function* crntfunc = (*subtokens)[i]->tok_function;
    if (crntfunc->name == "main") {
      crntfunc->name = "_start";
    }

    con_token* tag_tok = new con_token;
    tag_tok->tok_type = TAG;
    con_tag* functag = new con_tag;
    tag_tok->tok_tag = functag;
    functag->name = crntfunc->name;
    for (size_t j = 0; j < crntfunc->arguments.size(); j++) {
      con_token* arg_tok = new con_token;
      arg_tok->tok_type = MACRO;
      con_macro* arg_macro = new con_macro;
      arg_macro->value = reg_to_str(j);
      arg_macro->macro = crntfunc->arguments[j];
      arg_tok->tok_macro = arg_macro;

      (*subtokens)[i]->tokens.insert((*subtokens)[i]->tokens.begin(), arg_tok);
    }
    (*subtokens)[i]->tokens.insert((*subtokens)[i]->tokens.begin(), tag_tok);
    con_token* ret_tok = new con_token;
    ret_tok->tok_type = CMD;
    con_cmd* ret_cmd = new con_cmd;
    ret_tok->tok_cmd = ret_cmd;
    ret_cmd->command = "ret";
    (*subtokens)[i]->tokens.push_back(ret_tok);
  }
}
void apply_macros(vector<con_token*>& tokens, vector<con_macro> knownmacros)
{
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type == MACRO) {
      // Filter spaces from macro and value pair
      con_macro* f_macro = new con_macro();
      f_macro->macro = "";
      f_macro->value = "";
      for (size_t j = 0; j < tokens[i]->tok_macro->macro.size(); j++) {
        if (tokens[i]->tok_macro->macro[j] != ' ') {
          f_macro->macro += tokens[i]->tok_macro->macro[j];
        }
      }
      for (size_t j = 0; j < tokens[i]->tok_macro->value.size(); j++) {
        if (tokens[i]->tok_macro->value[j] != ' ')
          f_macro->value += tokens[i]->tok_macro->value[j];
      }
      knownmacros.push_back(*f_macro);
      delete f_macro;
      continue;
    }
    apply_macro_to_token(*tokens[i], knownmacros);
    if (tokens[i]->tok_type == IF || tokens[i]->tok_type == WHILE || tokens[i]->tok_type == FUNCTION) {
      apply_macros(tokens[i]->tokens, knownmacros);
    }
  }
}
void apply_whiles(vector<con_token*>& tokens)
{
  for (size_t i = 0; i< tokens.size(); i++) {
    apply_whiles(tokens[i]->tokens);
    if (tokens[i]->tok_type != WHILE) {
      continue;
    }
    con_token* cmp_tok = new con_token;
    cmp_tok->tok_type = CMD;
    con_cmd* cmp_cmd = new con_cmd;
    cmp_tok->tok_cmd = cmp_cmd;
    cmp_cmd->command = "cmp";
    cmp_cmd->arg1 = tokens[i]->tok_while->condition.arg1;
    cmp_cmd->arg2 = tokens[i]->tok_while->condition.arg2;

    string endtag_name = "endwhile" + to_string(while_amnt);
    string starttag_name = "startwhile" + to_string(while_amnt);

    con_token* jmp_tok = new con_token;
    jmp_tok->tok_type = CMD;
    con_cmd* jmp_cmd = new con_cmd;
    jmp_tok->tok_cmd = jmp_cmd;
    jmp_cmd->command = "j" + comparison_to_string(get_comparison_inverse(tokens[i]->tok_while->condition.op));
    jmp_cmd->arg1 = endtag_name;

    con_token* jmpbck_tok = new con_token;
    jmpbck_tok->tok_type = CMD;
    con_cmd* jmpbck_cmd = new con_cmd;
    jmpbck_tok->tok_cmd = jmpbck_cmd;
    jmpbck_cmd->command = "jmp";
    jmpbck_cmd->arg1 = starttag_name;

    con_token* endwhile_tok = new con_token;
    endwhile_tok->tok_type = TAG;
    con_tag* endwhile_tag = new con_tag;
    endwhile_tok->tok_tag = endwhile_tag;
    endwhile_tag->name = endtag_name;

    con_token* startwhile_tok = new con_token;
    startwhile_tok->tok_type = TAG;
    con_tag* startwhile_tag = new con_tag;
    startwhile_tok->tok_tag = startwhile_tag;
    startwhile_tag->name = starttag_name;

    while_amnt++;
    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), jmp_tok);
    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), cmp_tok);
    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), startwhile_tok);
    tokens[i]->tokens.push_back(jmpbck_tok);
    tokens[i]->tokens.push_back(endwhile_tok);
    // so: starttag, cmp, jmp ... jmp, endtag
  }
}
void apply_ifs(vector<con_token*>& tokens)
{
  for (size_t i = 0; i< tokens.size(); i++) {
    apply_ifs(tokens[i]->tokens);
    if (tokens[i]->tok_type != IF) {
      continue;
    }
    con_token* cmp_tok = new con_token;
    cmp_tok->tok_type = CMD;
    con_cmd* cmp_cmd = new con_cmd;
    cmp_tok->tok_cmd = cmp_cmd;
    cmp_cmd->command = "cmp";
    cmp_cmd->arg1 = tokens[i]->tok_if->condition.arg1;
    cmp_cmd->arg2 = tokens[i]->tok_if->condition.arg2;

    string tagname = "endif" + to_string(if_amnt);

    con_token* jmp_tok = new con_token;
    jmp_tok->tok_type = CMD;
    con_cmd* jmp_cmd = new con_cmd;
    jmp_tok->tok_cmd = jmp_cmd;
    jmp_cmd->command = "j" + comparison_to_string(get_comparison_inverse(tokens[i]->tok_if->condition.op));
    jmp_cmd->arg1 = tagname;

    con_token* endif_tok = new con_token;
    endif_tok->tok_type = TAG;
    con_tag* endif_tag = new con_tag;
    endif_tok->tok_tag = endif_tag;
    endif_tag->name = tagname;
    if_amnt++;

    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), jmp_tok);
    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), cmp_tok);
    tokens[i]->tokens.push_back(endif_tok);
  }
}
void linearize_tokens(vector<con_token*>& tokens)
{
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type != IF && tokens[i]->tok_type != WHILE && tokens[i]->tok_type != FUNCTION) {
      continue;
    }
    vector<con_token*>* subtokens = &tokens[i]->tokens;
    tokens.insert(tokens.begin()+i+1, subtokens->begin(), subtokens->end());
    subtokens->clear();
    tokens.erase(tokens.begin()+i);
  }
}

std::string tokens_to_nasm(std::vector<con_token*>& tokens)
{
  string output = "";
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type == IF || tokens[i]->tok_type == WHILE
        || tokens[i]->tok_type == FUNCTION || tokens[i]->tok_type == MACRO
        || tokens[i]->tok_type == FUNCALL) {
      continue;
    }
    output += "\n";
    if (tokens[i]->tok_type == CMD) {
      output += tokens[i]->tok_cmd->command;
      if (!tokens[i]->tok_cmd->arg1.empty()) {
        output += " " + tokens[i]->tok_cmd->arg1;
      }
      if (!tokens[i]->tok_cmd->arg2.empty()) {
        output += ", " + tokens[i]->tok_cmd->arg2;
      }
      continue;
    }
    if (tokens[i]->tok_type == TAG) {
      output += tokens[i]->tok_tag->name + ":";
      continue;
    }
    if (tokens[i]->tok_type == SECTION) {
      output += "section " + tokens[i]->tok_section->name;
      continue;
    }
  }
  return output;
}
