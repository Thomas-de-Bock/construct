#include <string>
#include <vector>
#include <stdexcept>
#include "reconstruct.h"
#include "construct_types.h"

using namespace std;

#define min(a,b) ((a)<=(b) ? (a) : (b))

int if_amnt = 0;
int while_amnt = 0;
CON_BITWIDTH bitwidth = BIT64;

string comparison_to_string(CON_COMPARISON condition) {
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
CON_COMPARISON get_comparison_inverse(CON_COMPARISON condition) {
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

static string reg_to_str(uint8_t call_num, CON_BITWIDTH bitwidth) {
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
  throw invalid_argument("Invalid bitwidth or call_num: bitwidth="+to_string(static_cast<int>(bitwidth))
                         +" call_num="+to_string(static_cast<int>(call_num)));
}
static uint8_t str_to_reg(string reg_name)
{
  if (reg_name=="dil" ||reg_name=="di" || reg_name=="edi" || reg_name=="rdi")
    return 0;
  if (reg_name=="sil" ||reg_name=="si" || reg_name=="esi" || reg_name=="rsi")
    return 1;
  if (reg_name=="dl" || reg_name=="dx" || reg_name=="edx" || reg_name=="rdx")
    return 2;
  if (reg_name=="cl" || reg_name=="cx" || reg_name=="ecx" || reg_name=="rcx")
    return 3;
  if (reg_name=="r8b" || reg_name=="r8w" || reg_name=="r8d" || reg_name=="r8")
    return 4;
  if (reg_name=="r9b" || reg_name=="r9w" || reg_name=="r9d" || reg_name=="r9")
    return 5;
  return 6;
}

static size_t find_macro_in_arg(string arg, string macro)
{
  size_t pos = arg.find(macro);
  if ((pos == 0 || (arg[pos-1]!='_' && !isalpha(arg[pos-1])))
      && (pos+macro.size()-1 == arg.size()-1 || (arg[pos+macro.size()]!='_' && !isalpha(arg[pos+macro.size()])))) {
    return pos;
  }
  return string::npos;
}
static void apply_macro_to_token(con_token& token, const vector<con_macro*>& macros) {
  if (token.tok_type != WHILE && token.tok_type != IF && token.tok_type != CMD) {
    return;
  }
  // Unoptimal, but more clear imo
  for (size_t i = 0; i < macros.size(); i++) {
    const string& macro = macros[i]->macro;
    const string& value = macros[i]->value;
    size_t pos;
    switch (token.tok_type) {
      case WHILE:
        pos = find_macro_in_arg(token.tok_while->condition.arg1, macro);
        while (pos != string::npos) {
          token.tok_while->condition.arg1.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_while->condition.arg1, macro);
        }
        pos = find_macro_in_arg(token.tok_while->condition.arg2, macro);
        while (pos != string::npos) {
          token.tok_while->condition.arg2.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_while->condition.arg2, macro);
        }
      break;
      case IF:
        pos = find_macro_in_arg(token.tok_if->condition.arg1, macro);
        while (pos != string::npos) {
          token.tok_if->condition.arg1.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_if->condition.arg1, macro);
        }
        pos = find_macro_in_arg(token.tok_if->condition.arg2, macro);
        while (pos != string::npos) {
          token.tok_if->condition.arg2.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_if->condition.arg2, macro);
        }
      break;
      case CMD:
        pos = find_macro_in_arg(token.tok_cmd->arg1, macro);
        while (pos != string::npos) {
          token.tok_cmd->arg1.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_cmd->arg1, macro);
        }
        pos = find_macro_in_arg(token.tok_cmd->arg2, macro);
        while (pos != string::npos) {
          token.tok_cmd->arg2.replace(pos, macro.size(), value);
          pos = find_macro_in_arg(token.tok_cmd->arg2, macro);
        }
      break;
      default:
      break;
    }
  }
}
static vector<con_token*> push_args(vector<string>& args, CON_BITWIDTH bitwidth)
{
  vector<con_token*> arg_tokens;

  // stack args;
  for (size_t i = 6; i < args.size() ; ++i) {
    size_t i_rev = args.size()+5 - i;
    con_token* arg_tok = new con_token();
    arg_tok->tok_type = CMD;
    con_cmd* arg_cmd = new con_cmd();
    arg_tok->tok_cmd = arg_cmd;
    arg_cmd->command = "pushq"; // bitwidth
    arg_cmd->arg1 = args[i_rev];
    arg_tokens.push_back(arg_tok);
  }

  // register args;
  size_t reg_args_size = min(args.size(),6);
  uint8_t first_read[7] = {6,6,6,6,6,6,6}; // cell 6 is garbage to hold not special-regs
  for (size_t i = 0; i < reg_args_size; ++i) {
    uint8_t reg_num = str_to_reg(args[i]);
    first_read[reg_num] = min(first_read[reg_num],i);
  }
  // sort regs by first-read
  uint8_t read_order[6] = {6,6,6,6,6,6};
  for (size_t fr = 0; fr < reg_args_size; ++fr) {
    for (size_t reg = 0; reg < reg_args_size; ++reg) {
      if ((fr == first_read[reg]) && (first_read[reg] > reg)) { //next in turn and will be pushed to stack
        read_order[fr]=reg;
      }
    }
  }
  // push reversed to pop order
  for (size_t fr = 0; fr < 6; ++fr) {
    size_t fr_rev = 5 - fr; // reverse the order
    if (read_order[fr_rev] != 6) { // there is a regester first read i arg number fr, and will be deleted before
      con_token* arg_tok = new con_token();
      arg_tok->tok_type = CMD;
      con_cmd* arg_cmd = new con_cmd();
      arg_tok->tok_cmd = arg_cmd;
      arg_cmd->command = "push";
      arg_cmd->arg1 = reg_to_str(read_order[fr_rev], bitwidth);
      arg_tokens.push_back(arg_tok);
    }
  }
  // set each arg and track values places
  uint8_t current_val_place[6] = {0,1,2,3,4,5}; // 6 means stack
  for (size_t reg = 0; reg < reg_args_size; ++reg) {
    if (first_read[reg] > reg) {
      current_val_place[reg] = 6;
    }
  }
  for (size_t i = 0; i < reg_args_size; i++) {
    con_token* arg_tok = new con_token();
    arg_tok->tok_type = CMD;
    con_cmd* arg_cmd = new con_cmd();
    arg_tok->tok_cmd = arg_cmd;
    uint8_t wanted_reg = str_to_reg(args[i]);
    if (wanted_reg==6) {
      arg_cmd->command = "mov";
      arg_cmd->arg1 = reg_to_str(i, bitwidth);
      arg_cmd->arg2 = args[i];
      // if regi was read before, then current_val_place[i] is a previous register (correct)
      // if regi isn't read yet, then current_val_place[i] is stack (correct)
    } else {
      if (current_val_place[wanted_reg] == 6) {
        arg_cmd->command = "pop";
        arg_cmd->arg1 = reg_to_str(i, bitwidth);
        current_val_place[wanted_reg] = i; // wanted_reg moved from stack to regi
      } else {
        if (i != current_val_place[wanted_reg]) {
          arg_cmd->command = "mov";
          arg_cmd->arg1 = reg_to_str(i, bitwidth);
          arg_cmd->arg2 = reg_to_str(current_val_place[wanted_reg], bitwidth);
          // if regi was read before, then current_val_place[i] is a previous register (correct)
          // if regi isn't read yet, then current_val_place[i] is stack (correct)
          current_val_place[wanted_reg] = min(current_val_place[wanted_reg],i);
        } else {
          arg_cmd->command = "nop";
        }
      }
    }
    if (arg_cmd->command == "nop") {
      delete arg_cmd;
      arg_cmd = nullptr;
      delete arg_tok;
      arg_tok = nullptr;
    } else {
      arg_tokens.push_back(arg_tok);
    }
  }
  return arg_tokens;
}

void apply_whiles(vector<con_token*>& tokens) {
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
void apply_ifs(vector<con_token*>& tokens) {
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
void apply_functions(std::vector<con_token*>& tokens) {
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type != FUNCTION) {
      continue;
    }
    con_function* crntfunc = tokens[i]->tok_function;
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
      arg_macro->value = reg_to_str(j, bitwidth);
      arg_macro->macro = crntfunc->arguments[j];
      arg_tok->tok_macro = arg_macro;

      tokens[i]->tokens.insert(tokens[i]->tokens.begin(), arg_tok);
    }
    tokens[i]->tokens.insert(tokens[i]->tokens.begin(), tag_tok);
    con_token* ret_tok = new con_token;
    ret_tok->tok_type = CMD;
    con_cmd* ret_cmd = new con_cmd;
    ret_tok->tok_cmd = ret_cmd;
    ret_cmd->command = "ret";
    tokens[i]->tokens.push_back(ret_tok);
  }
}
void apply_macros(vector<con_token*>& tokens, vector<con_macro*>& knownmacros) {
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type == MACRO) {
      knownmacros.push_back(tokens[i]->tok_macro);
      continue;
    }
    apply_macro_to_token(*tokens[i], knownmacros);
    if (tokens[i]->tok_type == IF || tokens[i]->tok_type == WHILE || tokens[i]->tok_type == FUNCTION) {
      apply_macros(tokens[i]->tokens, knownmacros);
    }
  }
}
void apply_funcalls(std::vector<con_token*>& tokens) {
  for (size_t i = 0; i < tokens.size(); i++) {
    apply_funcalls(tokens[i]->tokens);
    if (tokens[i]->tok_type != FUNCALL) {
      continue;
    }
    vector<con_token*> arg_tokens = push_args(tokens[i]->tok_funcall->arguments, bitwidth);
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
void apply_syscalls(std::vector<con_token*>& tokens)
{
  for (size_t i = 0; i < tokens.size(); i++) {
    apply_syscalls(tokens[i]->tokens);
    if (tokens[i]->tok_type != SYSCALL) {
      continue;
    }
    vector<con_token*> arg_tokens = push_args(tokens[i]->tok_syscall->arguments, bitwidth);
    con_token* call_tok1 = new con_token();
    call_tok1->tok_type = CMD;
    con_cmd* call_cmd = new con_cmd();
    call_tok1->tok_cmd = call_cmd;
    call_cmd->command = "mov";
    call_cmd->arg1 = "rax";
    call_cmd->arg2 = to_string(tokens[i]->tok_syscall->number);
    arg_tokens.push_back(call_tok1);
    con_token* call_tok2 = new con_token();
    call_tok2->tok_type = CMD;
    con_cmd* call_sys = new con_cmd();
    call_tok2->tok_cmd = call_sys;
    call_sys->command = "syscall";
    arg_tokens.push_back(call_tok2);

    tokens.insert(tokens.begin()+i+1, arg_tokens.begin(), arg_tokens.end());
  }
}

void linearize_tokens(vector<con_token*>& tokens) {
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

std::string tokens_to_nasm(std::vector<con_token*>& tokens) {
  string output = "";
  for (size_t i = 0; i < tokens.size(); i++) {
    if (tokens[i]->tok_type == WHILE || tokens[i]->tok_type == IF
        || tokens[i]->tok_type == FUNCTION || tokens[i]->tok_type == MACRO
        || tokens[i]->tok_type == FUNCALL || tokens[i]->tok_type == SYSCALL) {
      continue;
    }
    if (tokens[i]->tok_type == SECTION) {
      output += "section " + tokens[i]->tok_section->name;
    } else if (tokens[i]->tok_type == TAG) {
      output += tokens[i]->tok_tag->name + ":";
    } else if (tokens[i]->tok_type == CMD) {
      output += tokens[i]->tok_cmd->command;
      if (!tokens[i]->tok_cmd->arg1.empty()) {
        output += " " + tokens[i]->tok_cmd->arg1;
      }
      if (!tokens[i]->tok_cmd->arg2.empty()) {
        output += ", " + tokens[i]->tok_cmd->arg2;
      }
    }
    output += "\n";
  }
  return output;
}
