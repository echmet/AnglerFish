#ifndef SCALARFITRESULTSMAPPING_H
#define SCALARFITRESULTSMAPPING_H

#include <mappers/floatmappermodel.h>

namespace gearbox {
namespace ScalarFitResultsMapping {

enum class Items {
  R_SQUARED,
  LAST_INDEX
};

class MapperModel : public FloatMapperModel<Items> {
public:
  using FloatMapperModel<Items>::FloatMapperModel;
};

} // namespace ScalarFitResultsMapping
} // namespace gearbox

#endif // SCALARFITRESULTSMAPPING_H
