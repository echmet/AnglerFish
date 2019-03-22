#ifndef EMPFITTERINTERFACE_H
#define EMPFITTERINTERFACE_H

#include <string>
#include <stdexcept>
#include <vector>

namespace gearbox {
  class Gearbox;
} // namespace gearbox

namespace calculators {

class EMPFitterInterface {
public:
  class MobilityConstraints {
  public:
    double low;
    double high;
  };

  class TracepointInfo {
  public:
    TracepointInfo(const int32_t _TPID, std::string _description) :
      TPID{_TPID},
      description{std::move(_description)}
    {
    }

    TracepointInfo(const TracepointInfo &other) :
      TPID{other.TPID},
      description{other.description}
    {
    }

    TracepointInfo(TracepointInfo &&other) noexcept :
      TPID{other.TPID},
      description{std::move(other.description)}
    {
    }

    TracepointInfo & operator=(const TracepointInfo &other)
    {
      const_cast<int32_t&>(TPID) = other.TPID;
      const_cast<std::string&>(description) = other.description;

      return *this;
    }

    TracepointInfo & operator=(TracepointInfo &&other) noexcept
    {
      const_cast<int32_t&>(TPID) = other.TPID;
      const_cast<std::string&>(description) = std::move(other.description);

      return *this;
    }

    const int32_t TPID;
    const std::string description;
  };

  class TracepointState {
  public:
    TracepointState(const int32_t _TPID, const bool _enabled) :
      TPID{_TPID},
      enabled{_enabled}
    {
    }

    TracepointState(const TracepointState &other) :
      TPID{other.TPID},
      enabled{other.enabled}
    {
    }

    TracepointState & operator=(const TracepointState &other)
    {
      const_cast<int32_t&>(TPID) = other.TPID;
      const_cast<bool&>(enabled) = other.enabled;

      return *this;
    }

    TracepointState & operator=(TracepointState &&other) noexcept
    {
      const_cast<int32_t&>(TPID) = other.TPID;
      const_cast<bool&>(enabled) = other.enabled;

      return *this;
    }

    const int32_t TPID;
    const bool enabled;
  };

  class Exception : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
  };

  explicit EMPFitterInterface(gearbox::Gearbox &gbox, const bool unscaledStdErrs);
  EMPFitterInterface(const EMPFitterInterface &other) = delete;
  ~EMPFitterInterface() = default;
  void fit();
  void propagateRSquared();

  void setTracepoints(const std::vector<TracepointState> &states);
  bool writeTrace(const char *path);

  static MobilityConstraints mobilityConstraints(const double mobility);

  static std::vector<TracepointInfo> tracepointInformation();
  static const char * versionString();

  static const char *ANALYTE_NAME;

private:
  gearbox::Gearbox &h_gbox;
  double m_rSquared;

  const bool m_unscaledStdErrs;
};

} // namespace calculator

#endif // EMPFITTERINTERFACE_H
