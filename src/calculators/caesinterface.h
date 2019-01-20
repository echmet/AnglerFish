#ifndef CAESINTERFACE_H
#define CAESINTERFACE_H

#include <stdexcept>

namespace gdm {
  class GDM;
}

class CAESInterface {
public:
  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  class BufferProperties {
  public:
    const double pH;
    const double ionicStrength;
  };

  explicit CAESInterface(const gdm::GDM &model);
  BufferProperties bufferProperties();

private:
  const gdm::GDM &h_model;
};

#endif // CAESINTERFACE_H
