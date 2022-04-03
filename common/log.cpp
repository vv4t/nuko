#include "log.h"

#include <stdlib.h>

void log_debug(const std::string &str)
{
  std::cout << "[DEBUG] " << str << std::endl;
}

void log_error(const std::string &str)
{
  std::cerr << "[ERROR] " << str << std::endl;
  exit(1);
}
