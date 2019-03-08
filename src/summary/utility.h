#ifndef SUMMARY_UTILITY_H
#define SUMMARY_UTILITY_H

#include <string>

namespace gdm {
  class GDM;
} // namespace gdm

namespace summary {

class Utility {
public:
  Utility() = delete;

  static std::string bufferToString(const gdm::GDM *buf, const bool abbreviate);
};

} // namespace summary

#endif // SUMMARY_UTILITY_H
