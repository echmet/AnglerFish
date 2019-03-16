#ifndef PERSISTENCE_TYPES_H
#define PERSISTENCE_TYPES_H

#include <QString>
#include <stdexcept>

namespace persistence {

class Exception : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};

class Target {
public:
  enum Type {
    TT_CLIPBOARD,
    TT_FILE
  };

  Target(const Type _type, QString _path) noexcept;

  const Type type;
  const QString path;
};

} // namespace persistence

#endif // PERSISTENCE_TYPES_H
