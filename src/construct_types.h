#ifndef CONSTRUCT_TYPES_H_
#define CONSTRUCT_TYPES_H_

#include <string>
#include <vector>

enum CON_BITWIDTH {
  BIT8,
  BIT16,
  BIT32,
  BIT64
};

enum CON_COMPARISON {
  E,
  NE,
  L,
  G,
  LE,
  GE
};

enum CON_TOKENTYPE {
  SECTION,
  TAG,
  WHILE,
  IF,
  FUNCTION,
  CMD,
  MACRO,
  FUNCALL,
  SYSCALL
};

struct _con_condition {
  CON_COMPARISON op;
  std::string arg1;
  std::string arg2;
};


struct con_section {
  std::string name;
};

struct con_tag {
  std::string name;
};

struct con_while {
  _con_condition condition;
};

struct con_if {
  _con_condition condition;
};

struct con_function {
  std::string name;
  std::vector<std::string> arguments;
};

struct con_cmd {
  std::string command;
  std::string arg1;
  std::string arg2;
};

struct con_macro {
  std::string value;
  std::string macro;
};

struct con_funcall {
  std::string funcname;
  std::vector<std::string> arguments;
};

struct con_syscall {
  uint16_t number;
  std::vector<std::string> arguments;
};


struct con_token {
  CON_TOKENTYPE tok_type;
  int indentation;
  con_section* tok_section = nullptr;
  con_tag* tok_tag = nullptr;
  con_while* tok_while = nullptr;
  con_if* tok_if = nullptr;
  con_function* tok_function = nullptr;
  con_cmd* tok_cmd = nullptr;
  con_macro* tok_macro = nullptr;
  con_funcall* tok_funcall = nullptr;
  con_syscall* tok_syscall = nullptr;
  std::vector<con_token*> tokens; // relevant to "if", "while", "function" and "syscall" tokens

  ~con_token() {
    switch (tok_type) {
      case SECTION:
        if (tok_section != nullptr) delete tok_section;
      break;
      case TAG:
        if (tok_tag != nullptr) delete tok_tag;
      break;
      case WHILE:
        if (tok_while != nullptr) delete tok_while;
      break;
      case IF:
        if (tok_if != nullptr) delete tok_if;
      break;
      case FUNCTION:
        if (tok_function != nullptr) delete tok_function;
      break;
      case CMD:
        if (tok_cmd != nullptr) delete tok_cmd;
      break;
      case MACRO:
        if (tok_macro != nullptr) delete tok_macro;
      break;
      case FUNCALL:
        if (tok_funcall != nullptr) delete tok_funcall;
      break;
      case SYSCALL:
        if (tok_syscall != nullptr) delete tok_syscall;
      break;
    }
  }
};

#endif // CONSTRUCT_TYPES_H_
