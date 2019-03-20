#ifndef COMMONOPTIONS_H
#define COMMONOPTIONS_H

#include <string>

namespace summary {

class CommonOptions {
public:
  CommonOptions(const bool _abbreviateBuffers,
                const bool _includeBuffers,
                const bool _includeCurve,
                const bool _includeEstimates,
                const bool _includeIonicEffects,
                const bool _includeBufferCapacity,
                std::string _title);

  const bool abbreviateBuffers;
  const bool includeBuffers;
  const bool includeCurve;
  const bool includeEstimates;
  const bool includeIonicEffects;
  const bool includeBufferCapacity;
  const std::string title;
};

} // namespace summary

#endif // COMMONOPTIONS_H
