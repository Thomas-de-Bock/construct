#ifndef CON_TYPES_H
#define CON_TYPES_H

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
  GE,
  ERROR,
};

enum CON_TOKENTYPE {
  SECTION,
  TAG,
  WHILE,
  IF,
  FUNCTION,
  CMD,
  MACRO,
  FUNCALL
};


struct con_token {
  CON_TOKENTYPE tok_type;
  int indentation;
  struct con_section* tok_section;
  struct con_tag* tok_tag;
  struct con_while* tok_while;
  struct con_if* tok_if;
  struct con_function* tok_function;
  struct con_funcall* tok_funcall;
  struct con_cmd* tok_cmd;
  struct con_macro* tok_macro;
  std::vector<con_token*> tokens; // Only non-empty for if, while and function tokens
};

struct con_macro {
  std::string value;
  std::string macro;
};

struct con_section {
  std::string name;
};

struct con_tag {
  std::string name;
};

struct con_condition {
  CON_COMPARISON op;
  std::string arg1;
  std::string arg2;
};

struct con_while {
  con_condition condition;
};

struct con_if {
  con_condition condition;
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

struct con_funcall {
  std::string funcname;
  std:: vector<std::string> arguments;
};

#endif
