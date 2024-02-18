#include <string>
#include <vector>
#include <stack>
#include <stdexcept>
#include "deconstruct.h"
#include "construct_types.h"

using namespace std;

static const char& FIRST_UPPERCASE_LETTER = 'A';
static const char& LAST_UPPERCASE_LETTER = 'Z';
static bool is_upper(const char& c)
{
  return c >= FIRST_UPPERCASE_LETTER && c <= LAST_UPPERCASE_LETTER;
}
static char to_lower(const char& c)
{
  return is_upper(c) ? c - FIRST_UPPERCASE_LETTER : c;
}
static void to_lower(string& str)
{
  string tmp;
  for (string::iterator it = str.begin(); it != str.end(); ++it) {
    tmp.push_back(to_lower(*it));
  }
  str = tmp;
}

class IsAnyOf
{
private:
  string chars;
public:
  IsAnyOf() = default;
  IsAnyOf(const string& _chars) : chars(_chars) {}
  IsAnyOf(const char*& _chars) : chars(_chars) {}
  ~IsAnyOf() = default;
  IsAnyOf(const IsAnyOf& other) = delete; // should save unique sorted chars for that
  IsAnyOf& operator=(const IsAnyOf& other) = delete;

  bool operator()(const char& c) const {
    for (string::const_iterator it = chars.cbegin(); it != chars.cend(); ++it) {
      if (*it == c) return true;
    }
    return false;
  }
};

template <typename Predicate>
static void split(vector<string>& result, const string& input, const Predicate& pred, const bool& compress_adj_delims = false)
{
  string tmp;
  bool prev_is_delim = false;
  for (string::const_iterator it = input.cbegin(); it != input.cend(); ++it) {
    if (pred(*it)) {
      if (prev_is_delim && compress_adj_delims) continue;
      result.push_back(tmp);
      tmp.clear();
      prev_is_delim = true;
    } else {
      tmp.push_back(*it);
      prev_is_delim = false;
    }
  }
  result.push_back(tmp);
}

int get_line_indentation(string line)
{
  int indentation = 0;
  for (size_t i = 0; i < line.size(); i++) {
    if (line[i] == '\t') {
      indentation++;
      continue;
    }
    return indentation;
  }
  return -1;
}

// Expects formatted line
CON_TOKENTYPE get_token_type(string line)
{
  if (line.substr(0, 8) == "section ")
    return SECTION;
  if (line.find(' ') == string::npos && line[line.size()-1] == ':')
    return TAG;
  if (line.substr(0, 6) == "while ")
    return WHILE;
  if (line.substr(0, 3) == "if ")
    return IF;
  if (line.substr(0, 9) == "function ")
    return FUNCTION;
  if (line[0] == '!')
    return MACRO;
  if (line.substr(0, 5) == "call " && line.find('(') != string::npos && line.find(')') != string::npos)
    return FUNCALL;
  return CMD;
}

CON_COMPARISON str_to_comparison(string comp)
{
  if (comp == "e")
    return E;
  if (comp == "ne")
    return NE;
  if (comp == "l")
    return L;
  if (comp == "g")
    return G;
  if (comp == "le")
    return LE;
  if (comp == "ge")
    return GE;
  throw invalid_argument("Invalid comparison sing: "+comp);
}


vector<con_token*> delinearize_tokens(std::vector<con_token*> tokens)
{
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
  for (size_t i = 0; i < tokens.size(); i++) {
    if (parent_stack.top()->indentation - tokens[i]->indentation >= 0) {
      int indentation_diff = parent_stack.top()->indentation - tokens[i]->indentation+1;
      for (int j = 0; j < indentation_diff; j++) {
        parent_stack.pop();
      }
    }
    if (tokens[i]->indentation == parent_stack.top()->indentation+1) {
      parent_stack.top()->tokens.push_back(tokens[i]);
    }
    if (tokens[i]->tok_type == FUNCTION || tokens[i]->tok_type == IF || tokens[i]->tok_type == WHILE) {
      parent_stack.push(tokens[i]);
    }
  }

  vector<con_token*> delinearized_tokens = parent_token->tokens;

  delete parent_section;
  delete parent_token;

  return delinearized_tokens;
}

con_section* parse_section(string line)
{
  con_section* tok_section = new con_section();
  vector<string> line_split;
  split(line_split, line, IsAnyOf(" "));
  tok_section->name = line_split[1];
  return tok_section;
}
con_tag* parse_tag(string line)
{
  con_tag* tok_tag = new con_tag();
  tok_tag->name = line.substr(0, line.size()-1);
  return tok_tag;
}
con_while* parse_while(string line)
{
  con_while* tok_while = new con_while();
  vector<string> line_split;
  split(line_split, line, IsAnyOf(" "));
  tok_while->condition.arg1 = line_split[1];
  tok_while->condition.op = str_to_comparison(line_split[2]);
  tok_while->condition.arg2 = line_split[3].substr(0, line_split[3].size()-1); // to remove :
  return tok_while;
}
con_if* parse_if(string line)
{
  con_if* tok_if = new con_if();
  vector<string> line_split;
  split(line_split, line, IsAnyOf(" "));
  tok_if->condition.arg1 = line_split[1];
  tok_if->condition.op = str_to_comparison(line_split[2]);
  tok_if->condition.arg2 = line_split[3].substr(0, line_split[3].size()-1);
  return tok_if;
}
con_function* parse_function(string line)
{
  con_function* tok_function = new con_function();
  vector<string> line_split;
  split(line_split, line, IsAnyOf("():,"));
  tok_function->name = line_split[0].substr(9, line_split[0].size()-9);
  for (size_t i = 1; i < line_split.size()-2; i++) {
    if (line_split[i].empty()) {
      continue;
    }
    tok_function->arguments.push_back(line_split[i]); // macros filter out spaces anyway when applied
  }
  return tok_function;
}
con_cmd* parse_cmd(string line)
{
  con_cmd* tok_cmd = new con_cmd();
  vector<string> line_split;
  split(line_split, line, IsAnyOf(" ,"));
  tok_cmd->command = line_split[0];
  if (line_split.size() > 1)
    tok_cmd->arg1 = line_split[1];
  if (line_split.size() > 3)
    tok_cmd->arg2 = line_split[3];
  return tok_cmd;
}
con_macro* parse_macro(string line)
{
  con_macro* tok_macro = new con_macro();
  int spacepos = line.find(' ');
  tok_macro->macro = line.substr(1, spacepos-1);
  tok_macro->value = line.substr(spacepos+1, line.size()-spacepos-1);
  return tok_macro;
}
con_funcall* parse_funcall(string line)
{
  con_funcall* tok_funcall = new con_funcall();
  vector<string> line_split;
  split(line_split, line, IsAnyOf("(),"));
  tok_funcall->funcname = line_split[0].substr(5, line_split[0].size()-5);
  for (size_t i = 1; i < line_split.size()-1; i++) {
    if (line_split[i].empty()) {
      continue;
    }
    tok_funcall->arguments.push_back(line_split[i]); // macros filter out spaces anyway when applied
  }
  return tok_funcall;
}

// Does not expect formatted line, only lowercase
con_token* parse_line(string line)
{
  con_token* token = new con_token;
  //remove multiple spaces from line
  string f_line = "";
  bool caught_space = false;
  for (size_t i = 0; i < line.size(); i++) {
    if (line[i] == ' ') {
      if (!caught_space) {
        f_line += line[i];
        caught_space = true;
      }
    } else {
      if (line[i] != '\t') {
        f_line += line[i];
      }
      caught_space = false;
    }
  }
  token->tok_type = get_token_type(f_line);
  switch (token->tok_type) {
    case SECTION:
      token->tok_section = parse_section(f_line);
      break;
    case TAG:
      token->tok_tag = parse_tag(f_line);
      break;
    case WHILE:
      token->tok_while = parse_while(f_line);
      break;
    case IF:
      token->tok_if = parse_if(f_line);
      break;
    case FUNCTION:
      token->tok_function = parse_function(f_line);
      break;
    case CMD:
      token->tok_cmd = parse_cmd(f_line);
      break;
    case MACRO:
      token->tok_macro = parse_macro(f_line);
      break;
    case FUNCALL:
      token->tok_funcall = parse_funcall(f_line);
  }
  return token;
}
vector<con_token*> parse_construct(string code)
{
  vector<string> code_split;
  split(code_split, code, IsAnyOf("\n"), true);
  to_lower(code);
  vector<con_token*> tokens;
  bool in_data = false;
  for (size_t i = 0; i < code_split.size(); i++) {
    // Check if it contains any alphabet chars
    if (code_split[i].find_first_of("abcdefghijklmnopqrstuvwxyz!") == std::string::npos) {
      continue;
    }
    con_token* new_token = parse_line(code_split[i]);
    new_token->indentation = get_line_indentation(code_split[i]);
    tokens.push_back(new_token);
    if (new_token->tok_type == SECTION && (new_token->tok_section->name == ".data" || new_token->tok_section->name == ".bss")) {
      in_data = true;
    } else if (new_token->tok_type == SECTION && new_token->tok_section->name == ".text") {
      in_data = false;
    } else if (in_data) {
      //TODO free original con_x
      con_cmd* data_cmd = new con_cmd;
      data_cmd->command = code_split[i];
      new_token->tok_type = CMD;
      new_token->tok_cmd = data_cmd;
    }
  }
  return tokens;
}
