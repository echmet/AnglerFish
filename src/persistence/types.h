#ifndef PERSISTENCE_TYPES_H
#define PERSISTENCE_TYPES_H

#include <stdexcept>

namespace persistence {

class Exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

} // namespace persistence

#endif // PERSISTENCE_TYPES_H
