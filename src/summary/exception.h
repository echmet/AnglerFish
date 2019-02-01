#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>

namespace summary {

class Exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class NoDefaultOptionsException : public Exception {
public:
  using std::runtime_error::runtime_error;
};

} // namespace summary

#endif // EXCEPTION_H
