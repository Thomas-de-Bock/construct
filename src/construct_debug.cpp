#include "construct_debug.h"

std::string tokentype_to_string(CON_TOKENTYPE type) {
  switch(type) {
    case SECTION:
      return "section";
    case TAG:
      return "tag";
    case WHILE:
      return "while";
    case IF:
      return "if";
    case FUNCTION:
      return "func";
    case CMD:
      return "cmd";
    case MACRO:
      return "macro";
	default:
	  break;
  }
  return "unknown";
}

std::string token_to_string(con_token token) {
  std::string tokstring = "type: " + tokentype_to_string(token.tok_type);
  switch(token.tok_type) {
    case SECTION:
      tokstring += ", name: " + token.tok_section->name;
      break;
    case TAG:
      tokstring += ", name: " + token.tok_tag->name;
      break;
    case WHILE:
      tokstring += ", condition: " + token.tok_while->condition.arg1 + " " + comparison_to_string(token.tok_while->condition.op) + " " + token.tok_while->condition.arg2;
      break;
    case IF:
      tokstring += ", condition: " + token.tok_if->condition.arg1 + " " + comparison_to_string(token.tok_if->condition.op) + " " + token.tok_if->condition.arg2;
      break;
    case FUNCTION:
      tokstring += ", function: " + token.tok_function->name + ", arguments: ";
      for(size_t i = 0; i < token.tok_function->arguments.size(); i++) {
        if(i != 0) {
          tokstring += ", ";
        }
        tokstring += token.tok_function->arguments[i];
      }
      break;
    case CMD:
      if(!token.tok_cmd->arg1.empty() && !token.tok_cmd->arg2.empty()) {
        tokstring += ", cmd: " + token.tok_cmd->command + " " + token.tok_cmd->arg1 + ", " + token.tok_cmd->arg2;
        break;
      }
      if(!token.tok_cmd->arg1.empty()) {
        tokstring += ", cmd: " + token.tok_cmd->command + " " + token.tok_cmd->arg1;
        break;
      }
      tokstring += ", cmd: " + token.tok_cmd->command;
      break;
    case MACRO:
      tokstring += ", macro: " + token.tok_macro->macro + ", value: " + token.tok_macro->value;
      break;
	default:
	  break;
  }
  if(token.tokens.size() > 0) {
    tokstring += ", tokens: {\n";
    for(size_t i = 0; i < token.tokens.size(); i++) {
      tokstring += token_to_string(*token.tokens[i]) + "\n";
    }
    tokstring += "}";
  }
  return tokstring;
}

