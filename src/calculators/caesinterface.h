#ifndef CAESINTERFACE_H
#define CAESINTERFACE_H

#include <stdexcept>

namespace gearbox {
  class IonicEffectsModel;
} // namespace gearbox

namespace gdm {
  class GDM;
} // gdm;

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
    const double bufferCapacity;
  };

  explicit CAESInterface(const gdm::GDM &model, const gearbox::IonicEffectsModel &ionEffs);
  BufferProperties bufferProperties();

private:
  const gdm::GDM &h_model;
  const gearbox::IonicEffectsModel &h_ionEffs;
};

#endif // CAESINTERFACE_H
