#include "construct_error.h"
#include "construct_flags.h"
#include "construct_types.h"
#include "deconstruct.h"

void string_split(std::string& input, std::vector<std::string>& output, std::string delimiter, bool token_compress_on) {
    std::string token;
    size_t start = 0, end = 0;
    while (start < input.length()) {
        end = input.find_first_of(delimiter, start);
        if (end == std::string::npos) {
            token = input.substr(start);
            if (!token.empty() || !token_compress_on) {
                output.push_back(token);
            }
            break;
        } else {
            token = input.substr(start, end - start);
            if (!token.empty() || !token_compress_on) {
                output.push_back(token);
            }
            start = end + 1;
        }
    }
}
using namespace std;
int get_line_indentation(string line) {
  int indentation = 0;
  for(size_t i = 0; i < line.size(); i++) {
    if(line[i] == '\t') {
      indentation++;
      continue;
    }
    return indentation;
  }
  return -1;
}

// Expects formatted line
CON_TOKENTYPE get_token_type(linedata* linedata) {
  if((*linedata->line)[0] == '!')
    return MACRO;
  if(linedata->line->substr(0, 3) == "if ")
    return IF;
  if(linedata->line->substr(0, 6) == "while ")
    return WHILE;
  if(linedata->line->substr(0, 9) == "function ")
    return FUNCTION;
  if(linedata->line->substr(0, 8) == "section ")
    return SECTION;
  if(linedata->line->substr(0, 5) == "call " && linedata->line->find('(') != string::npos && linedata->line->find(')') != string::npos)
    return FUNCALL;
  if(linedata->line->find(' ') == string::npos && (*linedata->line)[linedata->line->size()-1] == ':')
    return TAG;
  return CMD;
}

CON_COMPARISON str_to_comparison(string comp) {
  if(comp == "e")
    return E;
  if(comp == "ne")
    return NE;
  if(comp == "l")
    return L;
  if(comp == "g")
    return G;
  if(comp == "le")
    return LE;
  if(comp == "ge")
    return GE;
  return COMPARISON_ERROR;
}


vector<con_token*> delinearize_tokens(std::vector<con_token*> tokens) {
  vector<con_token*> dl_tokens;

  // Serves as parent "section" where all tokens belong to, convenient for algo
  con_section* parent_section = new con_section;
  con_token* parent_token = new con_token;
  parent_token->tok_section = parent_section;
  parent_token->tok_type = SECTION;
  parent_token->indentation = -1;

  stack<con_token*> parent_stack;
  parent_stack.push(parent_token);
  dl_tokens.push_back(parent_token);


  // When a new when, if or function is encountered it is pushed to the top of the parent_stack
  // All tokens with the indentation of the top of the parent_stack+1
  // are then added to the elem at the top of the stack (ptr so also to elem in vector).
  // If token is while, if or function it is pushed to stack and becomes new parent.
  // if indentation goes up, new token is pushed to stack, when indentation goes down,
  // tops of stack are popped off by how much it decreased.
  for(size_t i = 0; i < tokens.size(); i++) {
    if(parent_stack.top()->indentation - tokens[i]->indentation >= 0) {
      int indentation_diff = parent_stack.top()->indentation - tokens[i]->indentation+1;
      for(int j = 0; j < indentation_diff; j++) {
        parent_stack.pop();
      }
    }
    if(tokens[i]->indentation == parent_stack.top()->indentation+1) {
      parent_stack.top()->tokens.push_back(tokens[i]);
    }
    if(tokens[i]->tok_type == FUNCTION || tokens[i]->tok_type == IF || tokens[i]->tok_type == WHILE) {
      parent_stack.push(tokens[i]);
    }
  }

  vector<con_token*> delinearized_tokens = parent_token->tokens;

  delete parent_section;
  delete parent_token;

  return delinearized_tokens;
}

con_macro* parse_macro(linedata* linedata) {
  linedata->line_split = NULL;
  con_macro* tok_macro = new con_macro();
  int spacepos = linedata->line->find(' ');
  tok_macro->macro = linedata->line->substr(1, spacepos-1);
  tok_macro->value = linedata->line->substr(spacepos+1, linedata->line->size()-spacepos-1);
  return tok_macro;
}

con_if* parse_if(linedata* linedata) {
  con_if* tok_if = new con_if();
  vector<string> line_split;
  string_split(*linedata->line, line_split, " ", false);
  linedata->line_split = &line_split;
  tok_if->condition.arg1 = line_split[1];
  tok_if->condition.op = str_to_comparison(line_split[2]);
  if(tok_if->condition.op == COMPARISON_ERROR) {
    throw_parse_error(linedata, IF_ERROR, 2);
  }
  tok_if->condition.arg2 = line_split[3].substr(0, line_split[3].size()-1);
  return tok_if;
}
con_while* parse_while(linedata* linedata) {
  con_while* tok_while = new con_while();
  vector<string> line_split;
  string_split(*linedata->line, line_split, " ", false);
  linedata->line_split = &line_split;
  tok_while->condition.arg1 = line_split[1];
  tok_while->condition.op = str_to_comparison(line_split[2]);
  if(tok_while->condition.op == COMPARISON_ERROR) {
    throw_parse_error(linedata, WHILE_ERROR, 2);
  }
  tok_while->condition.arg2 = line_split[3].substr(0, line_split[3].size()-1); // to remove :
  return tok_while;
}
con_section* parse_section(linedata* linedata) {
  con_section* tok_section = new con_section();
  vector<string> line_split;
  string_split(*linedata->line, line_split, " ", false);
  linedata->line_split = &line_split;
  tok_section->name = line_split[1];
  return tok_section;
}
con_tag* parse_tag(linedata* linedata) {
  con_tag* tok_tag = new con_tag();
  tok_tag->name = linedata->line->substr(0, linedata->line->size()-1);
  return tok_tag;
}
con_cmd* parse_cmd(linedata* linedata) {
  con_cmd* tok_cmd = new con_cmd();
  vector<string> line_split;
  string_split(*linedata->line, line_split, " ,", false);
  linedata->line_split = &line_split;
  tok_cmd->command = line_split[0];
  if(line_split.size() > 1)
    tok_cmd->arg1 = line_split[1];
  if(line_split.size() > 3) 
    tok_cmd->arg2 = line_split[3];
  return tok_cmd;
}
con_function* parse_function(linedata* linedata) {
  con_function* tok_function = new con_function();
  vector<string> line_split;
  string_split(*linedata->line, line_split, "():,", false);
  linedata->line_split = &line_split;
  tok_function->name = line_split[0].substr(9, line_split[0].size()-9);
  for(size_t i = 1; i < line_split.size()-1; i++) {
    if(line_split[i].empty()) {
      continue;
    }
    tok_function->arguments.push_back(line_split[i]); // macros filter out spaces anyway when applied
  }
  return tok_function;
}
con_funcall* parse_funcall(linedata* linedata) {
  con_funcall* tok_funcall = new con_funcall();
  vector<string> line_split;
  string_split(*linedata->line, line_split, "(),", false);
  linedata->line_split = &line_split;
  tok_funcall->funcname = line_split[0].substr(5, line_split[0].size()-5);
  for(size_t i = 1; i < line_split.size(); i++) {
    if(line_split[i].empty()) {
      continue;
    }
    tok_funcall->arguments.push_back(line_split[i]); // macros filter out spaces anyway when applied
  }
  return tok_funcall;
}

// Does not expect formatted line, only lowercase
con_token* parse_line(linedata* linedata) {
  con_token* token = new con_token;
  //remove multiple spaces from line
  string f_line = "";
  bool caught_space = false;
  for(size_t i = 0; i < linedata->line->size(); i++) {
    if((*linedata->line)[i] == ' ') {
      if(!caught_space) {
        f_line += (*linedata->line)[i];
        caught_space = true;
      }
    } else {
      if((*linedata->line)[i] != '\t') {
        f_line += (*linedata->line)[i];
      }
      caught_space = false;
    }
  }
  linedata->line = &f_line;
  token->tok_type = get_token_type(linedata);
  switch(token->tok_type) {
    case MACRO:
      token->tok_macro = parse_macro(linedata);
      break;
    case IF:
      token->tok_if = parse_if(linedata);
      break;
    case WHILE:
      token->tok_while = parse_while(linedata);
      break;
    case FUNCTION:
      token->tok_function = parse_function(linedata);
      break;
    case FUNCALL:
      token->tok_funcall = parse_funcall(linedata);
      break;
    case SECTION:
      token->tok_section = parse_section(linedata);
      break;
    case TAG:
      token->tok_tag = parse_tag(linedata);
      break;
    case CMD:
      token->tok_cmd = parse_cmd(linedata);
      break;
  }
  return token;
}
vector<con_token*> parse_construct(string code) {
  vector<string> code_split;
  string_split(code, code_split, "\n", true);
  vector<con_token*> tokens;
  bool in_data = false;
  linedata* currentlinedata = NULL;
  for(size_t i = 0; i < code_split.size(); i++) {
    if(currentlinedata != NULL) {
      free(currentlinedata);
      currentlinedata = NULL;
    }
    // Check if it contains any alphabet chars
    if(code_split[i].find_first_of("abcdefghijklmnopqrstuvwxyz!") == std::string::npos) {
      continue;
    }
    currentlinedata = new linedata;
    currentlinedata->line = &code_split[i];
    currentlinedata->filename = &inputfile;
    currentlinedata->line_num = i+1;


    con_token* new_token = parse_line(currentlinedata);
    new_token->indentation = get_line_indentation(code_split[i]);
    tokens.push_back(new_token);
    if(new_token->tok_type == SECTION && (new_token->tok_section->name == ".data" || new_token->tok_section->name == ".bss")) {
      in_data = true;
    } else if(new_token->tok_type == SECTION && new_token->tok_section->name == ".text") {
      in_data = false;
    } else if(in_data) {
      con_cmd* data_cmd = new con_cmd;
      data_cmd->command = code_split[i];
      new_token->tok_type = CMD;
      new_token->tok_cmd = data_cmd;
    }
  }
  return tokens;
}
