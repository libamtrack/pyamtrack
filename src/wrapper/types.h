#ifndef WRAPPER_TYPES_H
#define WRAPPER_TYPES_H

#include <functional>
#include <variant>
#include <vector>

// Define the type for the function to be wrapped.
using Func = std::function<double(double)>;
using MultiargumentFunc = std::function<double(const std::vector<std::variant<double, int>>&)>;

#endif
