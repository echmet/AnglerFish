#ifndef EMPFITTERINTERFACE_H
#define EMPFITTERINTERFACE_H

#include <stdexcept>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace calculators {

class EMPFitterInterface {
public:
  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  explicit EMPFitterInterface(gearbox::Gearbox &gbox);
  ~EMPFitterInterface() = default;
  void fit();

  static const char *ANALYTE_NAME;

private:
  gearbox::Gearbox &h_gbox;
};

} // namespace calculator

#endif // EMPFITTERINTERFACE_H
