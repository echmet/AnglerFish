#include "abstractsummarizer.h"

namespace summary {

AbstractSummarizer::AbstractSummarizer()
{
}

AbstractSummarizer::~AbstractSummarizer()
{
}

SpecificOptionsPtr AbstractSummarizer::options()
{
  return nullptr;
}

} // namespace summary
