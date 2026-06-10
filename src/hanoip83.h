#pragma once

#include <vector>
#include <string>


#ifdef _WIN32
  #define HANOIP83_EXPORT __declspec(dllexport)
#else
  #define HANOIP83_EXPORT
#endif

HANOIP83_EXPORT void hanoip83();
HANOIP83_EXPORT void hanoip83_print_vector(const std::vector<std::string> &strings);
