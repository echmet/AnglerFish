#include "commonoptions.h"

namespace summary {

CommonOptions::CommonOptions(const bool _abbreviateBuffers,
                             const bool _includeBuffers,
                             const bool _includeCurve,
                             const bool _includeEstimates,
                             const bool _includeIonicEffects) :
  abbreviateBuffers{_abbreviateBuffers},
  includeBuffers{_includeBuffers},
  includeCurve{_includeCurve},
  includeEstimates{_includeEstimates},
  includeIonicEffects{_includeIonicEffects}

{
}

} // namespace summary
