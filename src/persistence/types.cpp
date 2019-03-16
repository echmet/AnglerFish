#include "types.h"

namespace persistence {

Target::Target(const Type _type, QString _path) noexcept :
  type{_type},
  path{std::move(_path)}
{
}

}
