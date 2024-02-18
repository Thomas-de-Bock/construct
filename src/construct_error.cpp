#include "construct_error.h"
#include "construct_flags.h"
#include <string>

const char* parse_error_strings[2] = {"Failed to parse if-statement", "Failed to parse while-loop"};

void throw_parse_error(linedata* linedata, PARSE_ERROR error_type, size_t tok_index) {
  std::cerr << inputfile << ": " << parse_error_strings[error_type] << "\n"
  << linedata->line_num << " | ";
  size_t space_count = 3 + std::to_string(linedata->line_num).size();
  for(size_t i = 0; i < linedata->line_split->size(); i++) {
    std::cerr << (*linedata->line_split)[i] << " ";
    if(i >= tok_index) {
      continue;
    }
    space_count += (*linedata->line_split)[i].size() + 1;
  }
  std::cerr << "\n" << std::string(space_count, ' ') << "^\n";
  exit(EXIT_FAILURE);
}
