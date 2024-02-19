#ifndef CON_ERROR_H
#define CON_ERROR_H

#include "construct_types.h"
#include "construct_flags.h"
#include <cstdlib>

enum PARSE_ERROR {
  IF_ERROR,
  WHILE_ERROR
};

void throw_parse_error(linedata* linedata, PARSE_ERROR error_type, size_t tok_index);


#endif
