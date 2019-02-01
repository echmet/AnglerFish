#ifndef COMMONOPTIONS_H
#define COMMONOPTIONS_H

namespace summary {

class CommonOptions {
public:
  CommonOptions(const bool _abbreviateBuffers,
                const bool _includeBuffers,
                const bool _includeCurve,
                const bool _includeEstimates,
                const bool _includeIonicEffects);

  const bool abbreviateBuffers;
  const bool includeBuffers;
  const bool includeCurve;
  const bool includeEstimates;
  const bool includeIonicEffects;
};

} // namespace summary

#endif // COMMONOPTIONS_H
